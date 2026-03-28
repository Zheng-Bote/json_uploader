/**
 * SPDX-FileComment: Error handling types for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file error_type.hpp
 * @brief Central error handling for the project using std::expected.
 * @version 1.0.0
 * @date 2026-03-28
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <string>
#include <expected>

namespace ju {

/**
 * @brief Error codes for the application.
 */
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
    InternalError,
    EnvFileError,
    LoggerError
};

/**
 * @brief Represents an error with a code and a descriptive message.
 */
struct Error {
    ErrorCode code;
    std::string message;
};

/**
 * @brief Template alias for expected results.
 * 
 * @tparam T The type of the successful value.
 */
template <typename T>
using Expected = std::expected<T, Error>;

} // namespace ju
