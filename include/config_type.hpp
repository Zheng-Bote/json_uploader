/**
 * SPDX-FileComment: Configuration types for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file config_type.hpp
 * @brief Configuration structure.
 * @version 1.1.0
 * @date 2026-03-29
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <string>
#include <optional>
#include <filesystem>
#include <map>

namespace ju {

/**
 * @brief Compression types for API upload.
 */
enum class CompressionType {
    None,
    Zstd,
    Gzip
};

/**
 * @brief API Metadata behavior types.
 */
enum class ApiMetaType {
    None,
    Object,
    Single
};

/**
 * @brief Configuration parameters for the application.
 */
struct Config {
    std::filesystem::path json_path;
    std::filesystem::path schema_path;
    std::filesystem::path env_path;
    bool env_encrypted = false;
    std::string env_pass_var = "ENV_PASS";
    bool send_email = false;
    bool show_version = false;
    bool check_version = false;

    // METAJSON_OBJECT template
    std::string metajson_object_template = "none";

    // API
    std::string api_login_url;
    std::string api_upload_url;
    std::string api_user;
    std::string api_password;
    CompressionType api_compression = CompressionType::None;
    ApiMetaType api_meta = ApiMetaType::None;
    
    // SMTP (for Email)
    std::optional<std::string> api_email;
    std::string smtp_server;
    int smtp_port = 587;
    std::string smtp_user;
    std::string smtp_password;
    std::string smtp_from;
    bool smtp_starttls = true;

    // Logging
    std::filesystem::path log_path;
    std::string log_level = "Info";

    // Metadata
    std::map<std::string, std::string> metadata;
};

} // namespace ju
