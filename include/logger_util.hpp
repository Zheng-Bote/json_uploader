/**
 * SPDX-FileComment: Logger utility for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file logger_util.hpp
 * @brief Logging utility based on spdlog with daily rotation.
 * @version 1.0.0
 * @date 2026-03-28
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "error_type.hpp"
#include <filesystem>
#include <spdlog/spdlog.h>

namespace ju {

/**
 * @brief Initializes the global logger.
 * 
 * @param log_dir Directory where logs will be stored.
 * @param log_level String representation of the log level (e.g., "Info", "Debug").
 * @return Expected<void>
 */
[[nodiscard]] Expected<void> init_logger(const std::filesystem::path& log_dir, std::string_view log_level = "Info");

/**
 * @brief Shuts down the logger.
 */
void shutdown_logger();

} // namespace ju
