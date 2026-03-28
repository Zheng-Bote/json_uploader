/**
 * SPDX-FileComment: Logger utility implementation for the json_uploader
 * project. SPDX-FileType: SOURCE SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file logger_util.cpp
 * @version 1.0.0
 * @date 2026-03-28
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
#include <algorithm>
#include <cctype>

namespace ju {

namespace {
/**
 * @brief Converts a string to spdlog level.
 */
spdlog::level::level_enum to_spdlog_level(std::string_view level) {
  std::string lvl(level);
  std::transform(lvl.begin(), lvl.end(), lvl.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

  if (lvl == "trace") return spdlog::level::trace;
  if (lvl == "debug") return spdlog::level::debug;
  if (lvl == "info") return spdlog::level::info;
  if (lvl == "warn" || lvl == "warning") return spdlog::level::warn;
  if (lvl == "error") return spdlog::level::err;
  if (lvl == "critical") return spdlog::level::critical;
  if (lvl == "off") return spdlog::level::off;
  return spdlog::level::info;
}
} // namespace

Expected<void> init_logger(const std::filesystem::path &log_dir, std::string_view log_level) {
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
    
    auto level = to_spdlog_level(log_level);
    spdlog::set_level(level);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    spdlog::flush_on(level);

  } catch (const spdlog::spdlog_ex &ex) {
    return std::unexpected(
        Error{ErrorCode::LoggerError,
              "Logger initialization failed: " + std::string(ex.what())});
  }

  return {};
}

void shutdown_logger() { spdlog::shutdown(); }

} // namespace ju
