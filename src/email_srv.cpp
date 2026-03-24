/**
 * SPDX-FileComment: Email service implementation for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file email_srv.cpp
 * @version 0.2.8
 * @date 2026-03-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "email_srv.hpp"
#include <curl/curl.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace ju {

struct UploadData {
    std::string content;
    size_t pos = 0;
};

static size_t payload_source(void* ptr, size_t size, size_t nmemb, void* userp) {
    auto* data = static_cast<UploadData*>(userp);
    size_t buffer_size = size * nmemb;

    if (data->pos < data->content.size()) {
        size_t to_copy = std::min(buffer_size, data->content.size() - data->pos);
        memcpy(ptr, data->content.c_str() + data->pos, to_copy);
        data->pos += to_copy;
        return to_copy;
    }
    return 0;
}

EmailSrv::EmailSrv(const Config& config) : config_(config) {}
EmailSrv::~EmailSrv() = default;

Expected<void> EmailSrv::send_status(bool success, std::string_view message) {
    if (!config_.api_email || config_.smtp_server.empty()) {
        return {};
    }

    spdlog::info("Sending status email via SMTP to {}", *config_.api_email);

    CURL* curl = curl_easy_init();
    if (!curl) return std::unexpected(Error{ErrorCode::InternalError, "Email CURL init failed"});

    // Prepare email headers and body
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << "Date: " << std::put_time(std::gmtime(&now), "%a, %d %b %Y %H:%M:%S +0000") << "\r\n";
    ss << "To: " << *config_.api_email << "\r\n";
    ss << "From: " << config_.smtp_from << "\r\n";
    ss << "Subject: " << (success ? "Upload erfolgreich" : "Upload fehlgeschlagen") << "\r\n";
    ss << "\r\n"; // End of headers
    ss << (success ? "Der Upload wurde erfolgreich abgeschlossen." : 
                     "Der Upload ist fehlgeschlagen. Fehler: " + std::string(message)) << "\r\n";
    
    UploadData upload_data{ ss.str(), 0 };

    struct curl_slist* recipients = nullptr;
    recipients = curl_slist_append(recipients, config_.api_email->c_str());

    std::string smtp_url = config_.smtp_server;
    if (smtp_url.find("://") == std::string::npos) {
        smtp_url = "smtp://" + smtp_url;
    }

    curl_easy_setopt(curl, CURLOPT_URL, smtp_url.c_str());
    if (config_.smtp_port != 0) {
        curl_easy_setopt(curl, CURLOPT_PORT, static_cast<long>(config_.smtp_port));
    }
    
    if (!config_.smtp_user.empty()) {
        curl_easy_setopt(curl, CURLOPT_USERNAME, config_.smtp_user.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, config_.smtp_password.c_str());
    }

    if (config_.smtp_starttls) {
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
    } else {
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_NONE);
    }
    
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, config_.smtp_from.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_data);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return std::unexpected(Error{ErrorCode::EmailFailed, "SMTP error: " + std::string(curl_easy_strerror(res))});
    }

    spdlog::info("Email sent successfully via SMTP");
    return {};
}

} // namespace ju
