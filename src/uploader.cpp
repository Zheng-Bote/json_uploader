/**
 * SPDX-FileComment: Uploader implementation for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file uploader.cpp
 * @version 1.0.0
 * @date 2026-03-28
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "uploader.hpp"
#include <curl/curl.h>
#include <fstream>
#include <print>

namespace ju {

Uploader::Uploader(const Config& config, std::string bearer_token)
    : config_(config), token_(std::move(bearer_token)), validator_(config) {}

Uploader::~Uploader() = default;

size_t Uploader::ReadCallback(char* ptr, size_t size, size_t nmemb, void* userp) {
    auto* state = static_cast<StreamState*>(userp);
    size_t total_size = size * nmemb;

    if (state->has_error) return CURL_READFUNC_ABORT;
    if (state->finished && state->output_pos >= state->output_size) return 0;

    // 1. If we have data in the output buffer, serve it first
    if (state->output_pos < state->output_size) {
        size_t to_copy = std::min(total_size, state->output_size - state->output_pos);
        std::copy(state->output_buffer.begin() + static_cast<long>(state->output_pos), 
                  state->output_buffer.begin() + static_cast<long>(state->output_pos + to_copy), ptr);
        state->output_pos += to_copy;
        return to_copy;
    }

    // 2. If finished, nothing more to do
    if (state->finished) return 0;

    // 3. Process next JSON object from simdjson stream
    state->output_pos = 0;
    state->output_size = 0;

    if (state->stream_it != state->stream.end()) {
        auto doc = *(state->stream_it);
        std::string_view json_view;
        auto error = doc.raw_json().get(json_view);
        if (error) {
            state->has_error = true;
            state->last_error = {ErrorCode::JsonParseError, "simdjson stream error"};
            return CURL_READFUNC_ABORT;
        }

        // Validate object
        auto val_res = state->validator->validate(json_view);
        if (!val_res) {
            state->has_error = true;
            state->last_error = val_res.error();
            return CURL_READFUNC_ABORT;
        }

        // Compress
        ZSTD_inBuffer input = { json_view.data(), json_view.size(), 0 };
        while (input.pos < input.size) {
            ZSTD_outBuffer output = { state->output_buffer.data(), state->output_buffer.size(), 0 };
            size_t const ret = ZSTD_compressStream2(state->cstream, &output, &input, ZSTD_e_continue);
            if (ZSTD_isError(ret)) {
                state->has_error = true;
                state->last_error = {ErrorCode::CompressionError, ZSTD_getErrorName(ret)};
                return CURL_READFUNC_ABORT;
            }
            state->output_size = output.pos;
        }

        ++(state->stream_it);
    } else {
        // End of stream, flush zstd
        ZSTD_inBuffer input = { nullptr, 0, 0 };
        ZSTD_outBuffer output = { state->output_buffer.data(), state->output_buffer.size(), 0 };
        size_t const ret = ZSTD_compressStream2(state->cstream, &output, &input, ZSTD_e_end);
        if (ZSTD_isError(ret)) {
             state->has_error = true;
             state->last_error = {ErrorCode::CompressionError, ZSTD_getErrorName(ret)};
             return CURL_READFUNC_ABORT;
        }
        state->output_size = output.pos;
        state->finished = true;
    }

    // Recurse to copy the newly generated data to ptr
    return ReadCallback(ptr, size, nmemb, userp);
}

Expected<void> Uploader::upload() {
    auto load_res = validator_.load_schema();
    if (!load_res) return load_res;

    StreamState state;
    state.validator = &validator_;
    
    auto json_load_error = simdjson::padded_string::load(config_.json_path.string()).get(state.json_content);
    if (json_load_error) {
        return std::unexpected(Error{ErrorCode::FileReadError, "Failed to load JSON file"});
    }

    auto stream_error = state.parser.iterate_many(state.json_content).get(state.stream);
    if (stream_error) {
        return std::unexpected(Error{ErrorCode::JsonParseError, "Failed to start JSON stream"});
    }
    state.stream_it = state.stream.begin();

    state.cstream = ZSTD_createCStream();
    ZSTD_initCStream(state.cstream, 1);
    state.output_buffer.resize(ZSTD_CStreamOutSize());

    CURL* curl = curl_easy_init();
    if (!curl) {
        ZSTD_freeCStream(state.cstream);
        return std::unexpected(Error{ErrorCode::InternalError, "Failed to init CURL"});
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + token_).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Content-Encoding: zstd");
    headers = curl_slist_append(headers, "Transfer-Encoding: chunked");

    curl_easy_setopt(curl, CURLOPT_URL, (config_.api_url + "/upload").c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadCallback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &state);
    
    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    ZSTD_freeCStream(state.cstream);

    if (state.has_error) return std::unexpected(state.last_error);
    if (res != CURLE_OK) return std::unexpected(Error{ErrorCode::UploadFailed, curl_easy_strerror(res)});
    if (http_code != 200 && http_code != 201) {
        return std::unexpected(Error{ErrorCode::UploadFailed, "Upload failed with HTTP " + std::to_string(http_code)});
    }

    return {};
}

} // namespace ju
