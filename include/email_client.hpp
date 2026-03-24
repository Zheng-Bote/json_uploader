/**
 * SPDX-FileComment: Email client for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file email_client.hpp
 * @version 0.2.8
 * @date 2026-03-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "config.hpp"
#include "error.hpp"
#include <string>

namespace ju {

class EmailClient {
public:
    explicit EmailClient(const Config& config);
    ~EmailClient();

    /**
     * @brief Sends a status email.
     * @param success Whether the upload was successful.
     * @param message Error message if failed.
     * @return Expected<void>
     */
    [[nodiscard]] Expected<void> send_status(bool success, std::string_view message = "");

private:
    const Config& config_;
};

} // namespace ju
