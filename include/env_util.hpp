/**
 * SPDX-FileComment: Environment utility for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file env_util.hpp
 * @brief Utilities to load and parse .env files.
 * @version 0.2.8
 * @date 2026-03-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "error_type.hpp"
#include <filesystem>
#include <string>

namespace ju {

/**
 * @brief Loads environment variables from a file into the current process.
 * 
 * @param env_path Path to the .env file.
 * @return Expected<void>
 */
[[nodiscard]] Expected<void> load_env_file(const std::filesystem::path& env_path);

/**
 * @brief Retrieves an environment variable.
 * 
 * @param key Variable name.
 * @param default_val Optional default value.
 * @return std::string The value or empty string.
 */
[[nodiscard]] std::string get_env(const std::string& key, const std::string& default_val = "");

} // namespace ju
