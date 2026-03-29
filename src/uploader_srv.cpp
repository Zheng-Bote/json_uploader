/**
 * SPDX-FileComment: Uploader service implementation for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file uploader_srv.cpp
 * @version 1.1.0
 * @date 2026-03-29
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "uploader_srv.hpp"
#include <curl/curl.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <print>

namespace ju {

UploaderSrv::UploaderSrv(const Config& config, std::string bearer_token)
    : config_(config), token_(std::move(bearer_token)), validator_(config) {}

UploaderSrv::~UploaderSrv() = default;

bool UploaderSrv::compress_helper(StreamState* state, std::string_view data, FlushMode flush) {
    if (!data.empty()) {
        state->debug_full_json.append(data);
    }
    
    if (state->config && state->config->api_compression == CompressionType::None) {
        if (!data.empty()) {
            state->output_buffer.insert(state->output_buffer.end(), data.begin(), data.end());
            state->output_size = state->output_buffer.size();
        }
        if (flush == FlushMode::Finish) state->compression_finished = true;
        return true;
    }

    if (state->config->api_compression == CompressionType::Zstd) {
        ZSTD_EndDirective directive = ZSTD_e_continue;
        if (flush == FlushMode::Sync) directive = ZSTD_e_flush;
        else if (flush == FlushMode::Finish) directive = ZSTD_e_end;

        ZSTD_inBuffer input = { data.data(), data.size(), 0 };
        while (input.pos < input.size || (directive != ZSTD_e_continue && !state->compression_finished)) {
            ZSTD_outBuffer output = { state->output_buffer.data() + state->output_size, 
                                      state->output_buffer.size() - state->output_size, 0 };
            
            if (output.size == 0) {
                state->output_buffer.resize(state->output_buffer.size() + ZSTD_CStreamOutSize());
                output.dst = state->output_buffer.data() + state->output_size;
                output.size = state->output_buffer.size() - state->output_size;
            }

            size_t const ret = ZSTD_compressStream2(state->cstream, &output, &input, directive);
            if (ZSTD_isError(ret)) {
                state->has_error = true;
                state->last_error = {ErrorCode::CompressionError, ZSTD_getErrorName(ret)};
                return false;
            }
            state->output_size += output.pos;
            if (directive == ZSTD_e_end && ret == 0) {
                state->compression_finished = true;
                break;
            }
            if (input.pos == input.size && output.pos == 0) break; 
        }
    } else if (state->config->api_compression == CompressionType::Gzip) {
        int zflush = Z_NO_FLUSH;
        if (flush == FlushMode::Sync) zflush = Z_SYNC_FLUSH;
        else if (flush == FlushMode::Finish) zflush = Z_FINISH;

        state->gstream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data.data()));
        state->gstream.avail_in = static_cast<uInt>(data.size());

        while (state->gstream.avail_in > 0 || (zflush != Z_NO_FLUSH && !state->compression_finished)) {
            if (state->output_size >= state->output_buffer.size()) {
                state->output_buffer.resize(state->output_buffer.size() + 16384);
            }
            
            state->gstream.next_out = reinterpret_cast<Bytef*>(state->output_buffer.data() + state->output_size);
            state->gstream.avail_out = static_cast<uInt>(state->output_buffer.size() - state->output_size);
            
            int ret = deflate(&state->gstream, zflush);
            if (ret == Z_STREAM_ERROR) {
                state->has_error = true;
                state->last_error = {ErrorCode::CompressionError, "Gzip deflate error"};
                return false;
            }
            
            state->output_size = state->output_buffer.size() - state->gstream.avail_out;
            
            if (ret == Z_STREAM_END) {
                state->compression_finished = true;
                break;
            }
            if (state->gstream.avail_in == 0 && state->gstream.avail_out != 0) break;
        }
    }

    return true;
}

size_t UploaderSrv::ResponseCallback(char* ptr, size_t size, size_t nmemb, void* userp) {
    auto* state = static_cast<StreamState*>(userp);
    size_t total_size = size * nmemb;
    state->response_body.append(ptr, total_size);
    return total_size;
}

size_t UploaderSrv::ReadCallback(char* ptr, size_t size, size_t nmemb, void* userp) {
    auto* state = static_cast<StreamState*>(userp);
    size_t total_size = size * nmemb;

    if (state->has_error) return CURL_READFUNC_ABORT;

    while (true) {
        if (state->output_pos < state->output_size) {
            size_t to_copy = std::min(total_size, state->output_size - state->output_pos);
            std::copy(state->output_buffer.begin() + static_cast<long>(state->output_pos), 
                      state->output_buffer.begin() + static_cast<long>(state->output_pos + to_copy), ptr);
            state->output_pos += to_copy;
            
            if (state->output_pos >= state->output_size) {
                state->output_pos = 0;
                state->output_size = 0;
                state->output_buffer.clear();
            }
            return to_copy;
        }

        if (state->stage == Stage::Done) return 0;

        state->output_pos = 0;
        state->output_size = 0;

        switch (state->stage) {
            case Stage::Init:
                state->stage = Stage::ProcessDoc;
                if (!compress_helper(state, "[", FlushMode::Sync)) return CURL_READFUNC_ABORT;
                break;

            case Stage::ProcessDoc:
                if (!state->stream_at_end) {
                    auto doc_res = *(state->stream_it);
                    if (doc_res.error()) {
                        state->stream_at_end = true;
                        continue;
                    }
                    auto doc = doc_res.value();
                    simdjson::ondemand::json_type type;
                    if (doc.type().get(type) == simdjson::SUCCESS && type == simdjson::ondemand::json_type::array) {
                        auto array_res = doc.get_array();
                        if (array_res.error()) {
                            state->has_error = true;
                            state->last_error = {ErrorCode::JsonParseError, "Failed to get array"};
                            return CURL_READFUNC_ABORT;
                        }
                        state->array = array_res.value();
                        state->array_it = state->array.begin();
                        state->stage = Stage::ArrayElement;
                    } else {
                        std::string_view json_view;
                        if (doc.raw_json().get(json_view) == simdjson::SUCCESS) {
                            auto val_res = state->validator->validate(json_view);
                            if (!val_res) {
                                state->has_error = true;
                                state->last_error = val_res.error();
                                return CURL_READFUNC_ABORT;
                            }
                            if (!state->is_first_element) {
                                if (!compress_helper(state, ",", FlushMode::Sync)) return CURL_READFUNC_ABORT;
                            }
                            try {
                                auto j = nlohmann::json::parse(json_view);
                                
                                // Merge metadata from Config
                                if (!state->config->metadata.empty() && state->config->api_meta != ApiMetaType::None) {
                                    if (state->config->api_meta == ApiMetaType::Object) {
                                        if (!j.contains("metadata")) {
                                            j["metadata"] = nlohmann::json::object();
                                        }
                                        for (const auto& [k, v] : state->config->metadata) {
                                            j["metadata"][k] = v;
                                        }
                                    } else if (state->config->api_meta == ApiMetaType::Single) {
                                        for (const auto& [k, v] : state->config->metadata) {
                                            j[k] = v;
                                        }
                                    }
                                }

                                std::string cleaned = j.dump();
                                if (!compress_helper(state, cleaned, FlushMode::Sync)) return CURL_READFUNC_ABORT;
                            } catch (...) {
                                if (!compress_helper(state, json_view, FlushMode::Sync)) return CURL_READFUNC_ABORT;
                            }
                            state->is_first_element = false;
                        }
                        ++(state->stream_it);
                        if ((*(state->stream_it)).error()) state->stream_at_end = true;
                    }
                } else {
                    state->stage = Stage::FlushEnd;
                    if (!compress_helper(state, "]", FlushMode::Sync)) return CURL_READFUNC_ABORT;
                }
                break;

            case Stage::ArrayElement:
                if (state->array_it != state->array.end()) {
                    auto element_res = *(state->array_it);
                    if (element_res.error()) {
                        state->has_error = true;
                        state->last_error = {ErrorCode::JsonParseError, "Array element error"};
                        return CURL_READFUNC_ABORT;
                    }
                    auto element = element_res.value();
                    std::string_view json_view;
                    if (element.raw_json().get(json_view) == simdjson::SUCCESS) {
                        auto val_res = state->validator->validate(json_view);
                        if (!val_res) {
                            state->has_error = true;
                            state->last_error = val_res.error();
                            return CURL_READFUNC_ABORT;
                        }
                        if (!state->is_first_element) {
                            if (!compress_helper(state, ",", FlushMode::Sync)) return CURL_READFUNC_ABORT;
                        }
                        try {
                            auto j = nlohmann::json::parse(json_view);

                            // Merge metadata from Config
                            if (!state->config->metadata.empty() && state->config->api_meta != ApiMetaType::None) {
                                if (state->config->api_meta == ApiMetaType::Object) {
                                    if (!j.contains("metadata")) {
                                        j["metadata"] = nlohmann::json::object();
                                    }
                                    for (const auto& [k, v] : state->config->metadata) {
                                        j["metadata"][k] = v;
                                    }
                                } else if (state->config->api_meta == ApiMetaType::Single) {
                                    for (const auto& [k, v] : state->config->metadata) {
                                        j[k] = v;
                                    }
                                }
                            }

                            std::string cleaned = j.dump();
                            if (!compress_helper(state, cleaned, FlushMode::Sync)) return CURL_READFUNC_ABORT;
                        } catch (...) {
                            if (!compress_helper(state, json_view, FlushMode::Sync)) return CURL_READFUNC_ABORT;
                        }
                        state->is_first_element = false;
                    }
                    ++(state->array_it);
                    if (state->output_size > 0) continue;
                } else {
                    ++(state->stream_it);
                    if ((*(state->stream_it)).error()) state->stream_at_end = true;
                    state->stage = Stage::ProcessDoc;
                }
                break;

            case Stage::FlushEnd:
                if (!compress_helper(state, "", FlushMode::Finish)) return CURL_READFUNC_ABORT;
                state->stage = Stage::Done;
                break;

            default:
                state->stage = Stage::Done;
                break;
        }
    }
}

Expected<void> UploaderSrv::upload() {
    std::string comp_str = "NONE";
    if (config_.api_compression == CompressionType::Zstd) comp_str = "ZSTD";
    else if (config_.api_compression == CompressionType::Gzip) comp_str = "GZIP";
    
    spdlog::info("Starting upload of {} (Compression: {})", config_.json_path.string(), comp_str);
    
    auto load_res = validator_.load_schema();
    if (!load_res) return load_res;

    StreamState state;
    state.validator = &validator_;
    state.config = &config_;
    
    auto json_load_error = simdjson::padded_string::load(config_.json_path.string()).get(state.json_content);
    if (json_load_error) return std::unexpected(Error{ErrorCode::FileReadError, "Failed to load JSON file"});

    auto stream_error = state.parser.iterate_many(state.json_content).get(state.stream);
    if (stream_error) return std::unexpected(Error{ErrorCode::JsonParseError, "Failed to start JSON stream"});
    
    state.stream_it = state.stream.begin();
    if ((*(state.stream_it)).error()) {
        state.stream_at_end = true;
    }
    
    state.output_buffer.reserve(32768);
    if (config_.api_compression == CompressionType::Zstd) {
        state.cstream = ZSTD_createCStream();
        ZSTD_initCStream(state.cstream, 1);
    } else if (config_.api_compression == CompressionType::Gzip) {
        state.gstream.zalloc = Z_NULL;
        state.gstream.zfree = Z_NULL;
        state.gstream.opaque = Z_NULL;
        if (deflateInit2(&state.gstream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
            return std::unexpected(Error{ErrorCode::InternalError, "Gzip init failed"});
        }
        state.gstream_init = true;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        if (state.cstream) ZSTD_freeCStream(state.cstream);
        if (state.gstream_init) deflateEnd(&state.gstream);
        return std::unexpected(Error{ErrorCode::InternalError, "CURL init failed"});
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + token_).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    if (config_.api_compression == CompressionType::Zstd) {
        headers = curl_slist_append(headers, "Content-Encoding: zstd");
    } else if (config_.api_compression == CompressionType::Gzip) {
        headers = curl_slist_append(headers, "Content-Encoding: gzip");
    }
    headers = curl_slist_append(headers, "Transfer-Encoding: chunked");

    curl_easy_setopt(curl, CURLOPT_URL, config_.api_upload_url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadCallback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &state);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ResponseCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &state);
    
    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    if (state.cstream) ZSTD_freeCStream(state.cstream);
    if (state.gstream_init) deflateEnd(&state.gstream);

    if (state.has_error) return std::unexpected(state.last_error);
    if (res != CURLE_OK) return std::unexpected(Error{ErrorCode::UploadFailed, curl_easy_strerror(res)});
    
    if (http_code != 200 && http_code != 201) {
        std::println(stderr, "\n--- DEBUG: Generated JSON payload ---");
        std::println(stderr, "{}", state.debug_full_json);
        std::println(stderr, "--- END DEBUG ---\n");

        std::string err_msg = "Upload HTTP error: " + std::to_string(http_code);
        if (!state.response_body.empty()) {
            err_msg += " (Server: " + state.response_body + ")";
        }
        return std::unexpected(Error{ErrorCode::UploadFailed, err_msg});
    }

    spdlog::info("Upload completed successfully");
    return {};
}

} // namespace ju
