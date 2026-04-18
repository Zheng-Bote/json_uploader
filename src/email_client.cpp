/**
 * SPDX-FileComment: Email client implementation for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file email_client.cpp
 * @version 1.2.0
 * @date 2026-03-28
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "email_client.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>

namespace ju {

EmailClient::EmailClient(const Config &config) : config_(config) {}
EmailClient::~EmailClient() = default;

Expected<void> EmailClient::send_status(bool success,
                                        std::string_view message) {
  if (!config_.api_email)
    return {};

  CURL *curl = curl_easy_init();
  if (!curl)
    return std::unexpected(
        Error{ErrorCode::InternalError, "Failed to init CURL for email"});

  nlohmann::json email_payload = {
      {"to", *config_.api_email},
      {"subject", success ? "Upload erfolgreich" : "Upload fehlgeschlagen"},
      {"body", success ? "Der Upload wurde erfolgreich abgeschlossen."
                       : "Der Upload ist fehlgeschlagen. Fehler: " +
                             std::string(message)}};
  std::string payload_str = email_payload.dump();

  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");

  curl_easy_setopt(curl, CURLOPT_URL, (config_.api_url + "/email").c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_str.c_str());
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

  CURLcode res = curl_easy_perform(curl);
  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  if (res != CURLE_OK || (http_code != 200 && http_code != 202)) {
    return std::unexpected(
        Error{ErrorCode::EmailFailed,
              "Failed to send email. HTTP " + std::to_string(http_code)});
  }

  return {};
}

} // namespace ju
