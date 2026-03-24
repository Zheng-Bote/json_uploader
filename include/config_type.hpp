/**
 * SPDX-FileComment: Configuration types for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file config_type.hpp
 * @brief Configuration structure.
 * @version 0.4.1
 * @date 2026-03-24
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

namespace ju {

/**
 * @brief Configuration parameters for the application.
 */
struct Config {
    std::filesystem::path json_path;
    std::filesystem::path schema_path;
    std::filesystem::path env_path;
    bool send_email = false;

    // API
    std::string api_login_url;
    std::string api_upload_url;
    std::string api_user;
    std::string api_password;
    bool api_compression = false;
    
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
};

} // namespace ju
