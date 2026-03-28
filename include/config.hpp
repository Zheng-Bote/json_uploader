/**
 * SPDX-FileComment: Configuration types for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file config.hpp
 * @brief Configuration structure and ENV variable loader.
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
#include <optional>
#include <filesystem>

namespace ju {

struct Config {
    std::filesystem::path json_path;
    std::filesystem::path schema_path;
    bool send_email = false;

    std::string api_url;
    std::string api_user;
    std::string api_password;
    std::optional<std::string> api_email;
};

} // namespace ju
