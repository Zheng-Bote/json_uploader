/**
 * SPDX-FileComment: Authentication service implementation for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file auth_srv.cpp
 * @version 0.2.8
 * @date 2026-03-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "auth_srv.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace ju {

AuthSrv::AuthSrv(const Config& config) : config_(config) {}
AuthSrv::~AuthSrv() = default;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

Expected<std::string> AuthSrv::login() {
    spdlog::info("Authenticating with API at {}", config_.api_login_url);

    CURL* curl = curl_easy_init();
    if (!curl) return std::unexpected(Error{ErrorCode::AuthFailed, "Failed to initialize CURL"});

    std::string readBuffer;
    std::string login_url = config_.api_login_url;
    
    // Corrected fields as per requirement: {"user": "...", "password": "..."}
    nlohmann::json login_data = {
        {"user", config_.api_user},
        {"password", config_.api_password}
    };
    std::string login_payload = login_data.dump();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, login_url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, login_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return std::unexpected(Error{ErrorCode::AuthFailed, "CURL login error: " + std::string(curl_easy_strerror(res))});
    }

    if (http_code != 200) {
        return std::unexpected(Error{ErrorCode::AuthFailed, "Login failed (HTTP " + std::to_string(http_code) + ")"});
    }

    try {
        auto json_res = nlohmann::json::parse(readBuffer);
        if (json_res.contains("token")) {
            spdlog::info("Authentication successful");
            return json_res["token"].get<std::string>();
        }
        return std::unexpected(Error{ErrorCode::AuthFailed, "Token missing in login response"});
    } catch (const std::exception& e) {
        return std::unexpected(Error{ErrorCode::AuthFailed, "Failed to parse login response: " + std::string(e.what())});
    }
}

} // namespace ju
