/**
 * SPDX-FileComment: Main entry point for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file main.cpp
 * @brief Orchestrates the JSON validation, compression, and upload process.
 * @version 0.4.1
 * @date 2026-03-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "cli_parser_ctrl.hpp"
#include "env_util.hpp"
#include "logger_util.hpp"
#include "auth_srv.hpp"
#include "uploader_srv.hpp"
#include "email_srv.hpp"
#include <curl/curl.h>
#include <spdlog/spdlog.h>
#include <print>

/**
 * @brief Main function orchestrating the workflow.
 */
int main(int argc, char** argv) {
    // 1. CLI Parsing
    auto config_res = ju::CliParserCtrl::parse(argc, argv);
    if (!config_res) {
        std::println(stderr, "CLI Error: {}", config_res.error().message);
        return 1;
    }
    auto& config = *config_res;

    // 2. Load ENV file
    auto env_res = ju::load_env_file(config.env_path);
    if (!env_res) {
        std::println(stderr, "Warning: {}", env_res.error().message);
    }

    // 3. Complete Config from ENV
    config.api_login_url = ju::get_env("API_LOGIN_URL");
    config.api_upload_url = ju::get_env("API_UPLOAD_URL");
    config.api_user = ju::get_env("API_USER");
    config.api_password = ju::get_env("API_PASSWORD");
    config.api_email = ju::get_env("API_EMAIL");
    config.api_compression = ju::get_env("API_COMPRESSION", "false") == "true";
    
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

    if (config.api_login_url.empty() || config.api_upload_url.empty() || 
        config.api_user.empty() || config.api_password.empty()) {
        std::println(stderr, "Error: Missing required API credentials in ENV");
        return 1;
    }

    // 4. Logger Init
    auto log_res = ju::init_logger(config.log_path);
    if (!log_res) {
        std::println(stderr, "Logger Error: {}", log_res.error().message);
        return 1;
    }

    spdlog::info("JSON Uploader starting...");
    curl_global_init(CURL_GLOBAL_ALL);

    auto workflow = [&]() -> ju::Expected<void> {
        ju::AuthSrv auth(config);
        auto token_res = auth.login();
        if (!token_res) return std::unexpected(token_res.error());

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
