/**
 * SPDX-FileComment: Main entry point for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file main.cpp
 * @brief Orchestrates the JSON validation, compression, and upload process.
 * @version 1.1.0
 * @date 2026-03-29
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "auth_srv.hpp"
#include "cli_parser_ctrl.hpp"
#include "email_srv.hpp"
#include "env_util.hpp"
#include "logger_util.hpp"
#include "uploader_srv.hpp"
#include <curl/curl.h>
#include <print>
#include <spdlog/spdlog.h>

// Project Configuration
#include "rz_config.hpp"

// Update Checker
#include <check_gh-update.hpp>

/**
 * @brief Main function orchestrating the workflow.
 */
int main(int argc, char **argv) {
  // 1. CLI Parsing
  auto config_res = ju::CliParserCtrl::parse(argc, argv);
  if (!config_res) {
    std::println(stderr, "CLI Error: {}", config_res.error().message);
    return 1;
  }
  auto &config = *config_res;

  // 1.1 Handle special flags
  if (config.show_version) {
    std::println("{} v{}", rz::config::PROG_LONGNAME, rz::config::VERSION);
    return 0;
  }

  if (config.check_version) {
    std::cerr << "[Info] Checking for updates...\n";
    try {
      const std::string repo_url(rz::config::PROJECT_HOMEPAGE_URL);
      const std::string current_version(rz::config::VERSION);

      if (!repo_url.empty() && !current_version.empty()) {
        auto result = ghupdate::check_github_update(repo_url, current_version);
        if (result.hasUpdate) {
          std::println("\n[Update] A new version is available: {} (current: {})",
                       result.latestVersion, current_version);
          std::println("[Update] Download: {}\n", repo_url);
        } else {
          std::println("\n[Update] {} is up to date.\n",
                       rz::config::EXECUTABLE_NAME);
        }
      }
    } catch (const std::exception &ex) {
      std::cerr << "[Error] Update check failed: " << ex.what() << "\n";
    }
    return 0;
  }

  // 2. Load ENV file
  auto env_res = ju::load_env_file(config);
  if (!env_res) {
    std::println(stderr, "Warning: {}", env_res.error().message);
  }

  // 2.1 Load Metadata from ENV (after .env is processed)
  config.metadata = ju::get_meta_env();

  // 3. Complete Config from ENV
  config.api_login_url = ju::get_env("API_LOGIN_URL");
  config.api_upload_url = ju::get_env("API_UPLOAD_URL");
  config.api_user = ju::get_env("API_USER");
  config.api_password = ju::get_env("API_PASSWORD");
  config.api_email = ju::get_env("API_EMAIL");
  config.metajson_object_template = ju::get_env("METAJSON_OBJECT", "none");

  std::string comp_val = ju::get_env("API_COMPRESSION", "none");
  std::transform(
      comp_val.begin(), comp_val.end(), comp_val.begin(),
      [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

  if (comp_val == "zstd")
    config.api_compression = ju::CompressionType::Zstd;
  else if (comp_val == "gzip")
    config.api_compression = ju::CompressionType::Gzip;
  else
    config.api_compression = ju::CompressionType::None;

  std::string meta_val = ju::get_env("API_META", "object");
  std::transform(
      meta_val.begin(), meta_val.end(), meta_val.begin(),
      [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

  if (meta_val == "object")
    config.api_meta = ju::ApiMetaType::Object;
  else if (meta_val == "single")
    config.api_meta = ju::ApiMetaType::Single;
  else
    config.api_meta = ju::ApiMetaType::None;

  // SMTP
  config.smtp_server = ju::get_env("SMTP_SERVER");
  try {
    std::string port_str = ju::get_env("SMTP_PORT", "587");
    config.smtp_port = std::stoi(port_str);
  } catch (...) {
    config.smtp_port = 587;
  }
  config.smtp_user = ju::get_env("SMTP_USER");
  config.smtp_password = ju::get_env("SMTP_PASSWORD");
  config.smtp_from = ju::get_env("SMTP_FROM");
  config.smtp_starttls = ju::get_env("SMTP_STARTTLS", "true") == "true";

  config.log_path = ju::get_env("LOG_PATH", "logs");
  config.log_level = ju::get_env("LOG_LEVEL", "Info");

  if (config.api_login_url.empty() || config.api_upload_url.empty() ||
      config.api_user.empty() || config.api_password.empty()) {
    std::println(stderr, "Error: Missing required API credentials in ENV");
    return 1;
  }

  // 4. Logger Init
  auto log_res = ju::init_logger(config.log_path, config.log_level);
  if (!log_res) {
    std::println(stderr, "Logger Error: {}", log_res.error().message);
    return 1;
  }

  spdlog::info("JSON Uploader starting...");
  curl_global_init(CURL_GLOBAL_ALL);

  auto workflow = [&]() -> ju::Expected<void> {
    ju::AuthSrv auth(config);
    auto token_res = auth.login();
    if (!token_res)
      return std::unexpected(token_res.error());

    ju::UploaderSrv uploader(config, *token_res);
    return uploader.upload();
  };

  auto result = workflow();
  bool success = result.has_value();
  std::string err_msg;
  if (!success) {
    err_msg = result.error().message;
    spdlog::error("Workflow failed: {}", err_msg);
  } else {
    std::println("Upload erfolgreich");
  }

  // 5. Optional Email via SMTP
  if (config.send_email) {
    ju::EmailSrv email_srv(config);
    auto email_res = email_srv.send_status(success, err_msg);
    if (!email_res) {
      spdlog::error("Email notification failed: {}", email_res.error().message);
    }
  }

  curl_global_cleanup();
  ju::shutdown_logger();
  return success ? 0 : 1;
}
