/**
 * SPDX-FileComment: Error handling types for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file error.hpp
 * @brief Central error handling for the project using std::expected.
 * @version 0.2.8
 * @date 2026-03-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <string>
#include <string_view>
#include <expected>
#include <print>

namespace ju {

enum class ErrorCode {
    Success = 0,
    InvalidArguments,
    EnvironmentMissing,
    AuthFailed,
    FileReadError,
    JsonParseError,
    SchemaValidationError,
    CompressionError,
    UploadFailed,
    EmailFailed,
    InternalError
};

struct Error {
    ErrorCode code;
    std::string message;

    void log() const {
        std::println(stderr, "Error [{}]: {}", static_cast<int>(code), message);
    }
};

template <typename T>
using Expected = std::expected<T, Error>;

} // namespace ju
