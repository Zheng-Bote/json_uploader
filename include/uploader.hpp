/**
 * SPDX-FileComment: Uploader for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file uploader.hpp
 * @version 0.2.8
 * @date 2026-03-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "config.hpp"
#include "error.hpp"
#include "validator.hpp"
#include <string>
#include <vector>
#include <zstd.h>

namespace ju {

class Uploader {
public:
    explicit Uploader(const Config& config, std::string bearer_token);
    ~Uploader();

    /**
     * @brief Performs the streaming upload of the JSON file.
     * @return Expected<void>
     */
    [[nodiscard]] Expected<void> upload();

private:
    const Config& config_;
    std::string token_;
    Validator validator_;

    // Context for libcurl callback
    struct StreamState {
        std::ifstream file;
        simdjson::ondemand::parser parser;
        simdjson::padded_string json_content;
        simdjson::ondemand::document_stream stream;
        simdjson::ondemand::document_stream::iterator stream_it;

        ZSTD_CStream* cstream = nullptr;
        std::vector<char> compress_buffer;
        std::vector<char> output_buffer;
        size_t output_pos = 0;
        size_t output_size = 0;
        bool finished = false;
        
        Validator* validator = nullptr;
        Error last_error;
        bool has_error = false;
    };

    static size_t ReadCallback(char* ptr, size_t size, size_t nmemb, void* userp);
};

} // namespace ju
