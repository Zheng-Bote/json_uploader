/**
 * SPDX-FileComment: Uploader service for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file uploader_srv.hpp
 * @version 0.4.2
 * @date 2026-03-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "config_type.hpp"
#include "error_type.hpp"
#include "validator_util.hpp"
#include <string>
#include <vector>
#include <zstd.h>
#include <simdjson.h>

namespace ju {

/**
 * @brief Service for streaming JSON upload with compression.
 */
class UploaderSrv {
public:
    explicit UploaderSrv(const Config& config, std::string bearer_token);
    ~UploaderSrv();

    /**
     * @brief Performs the streaming upload.
     * @return Expected<void>
     */
    [[nodiscard]] Expected<void> upload();

private:
    const Config& config_;
    std::string token_;
    ValidatorUtil validator_;

    enum class Stage {
        Init,
        ProcessArray,
        ArrayElement,
        ArrayNewline,
        ProcessDoc,
        DocNewline,
        FlushEnd,
        Done
    };

    struct StreamState {
        simdjson::ondemand::parser parser;
        simdjson::padded_string json_content;
        simdjson::ondemand::document_stream stream;
        simdjson::ondemand::document_stream::iterator stream_it;
        bool stream_at_end = false;

        Stage stage = Stage::Init;
        bool is_first_element = true;
        simdjson::ondemand::array array;
        simdjson::ondemand::array_iterator array_it;

        ZSTD_CStream* cstream = nullptr;
        std::vector<char> output_buffer;
        size_t output_pos = 0;
        size_t output_size = 0;
        bool zstd_finished = false;
        
        ValidatorUtil* validator = nullptr;
        const Config* config = nullptr;
        Error last_error;
        bool has_error = false;

        std::string response_body;
        std::string debug_full_json;
    };

    static bool compress_helper(StreamState* state, std::string_view data, ZSTD_EndDirective directive);
    static size_t ReadCallback(char* ptr, size_t size, size_t nmemb, void* userp);
    static size_t ResponseCallback(char* ptr, size_t size, size_t nmemb, void* userp);
};

} // namespace ju
