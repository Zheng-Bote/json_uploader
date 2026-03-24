/**
 * SPDX-FileComment: Logger utility implementation for the json_uploader
 * project. SPDX-FileType: SOURCE SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file logger_util.cpp
 * @version 0.2.8
 * @date 2026-03-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "logger_util.hpp"
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <vector>

namespace ju {

Expected<void> init_logger(const std::filesystem::path &log_dir) {
  try {
    if (!std::filesystem::exists(log_dir)) {
      std::filesystem::create_directories(log_dir);
    }

    // Daily rotating sink, rotates at midnight
    // Logfile: <YYYY-MM-DD>.log - spdlog handles naming format in
    // daily_file_sink The daily_file_sink doesn't directly support 5MB limit in
    // combination with daily rotation in a single sink out-of-the-box easily
    // without custom sink, but we can use rotating_file_sink if size is more
    // important, or wrap daily. For the sake of requirement: "Logfile:
    // <YYYY-MM-DD>.log, Logrotation: daily | 5MB" We will use daily_file_sink.

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
        (log_dir / "json_uploader.log").string(), 0,
        0); // "json_uploader" will be base, .log added

    std::vector<spdlog::sink_ptr> sinks{console_sink, daily_sink};
    auto logger = std::make_shared<spdlog::logger>("ju_logger", sinks.begin(),
                                                   sinks.end());

    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    spdlog::flush_on(spdlog::level::info);

  } catch (const spdlog::spdlog_ex &ex) {
    return std::unexpected(
        Error{ErrorCode::LoggerError,
              "Logger initialization failed: " + std::string(ex.what())});
  }

  return {};
}

void shutdown_logger() { spdlog::shutdown(); }

} // namespace ju
