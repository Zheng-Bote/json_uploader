/**
 * SPDX-FileComment: Email service for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file email_srv.hpp
 * @version 0.2.8
 * @date 2026-03-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "config_type.hpp"
#include "error_type.hpp"
#include <string>

namespace ju {

/**
 * @brief Service to send notification emails via a Mail-Gateway.
 */
class EmailSrv {
public:
    explicit EmailSrv(const Config& config);
    ~EmailSrv();

    /**
     * @brief Sends a status email.
     * @param success Whether the operation was successful.
     * @param message Detailed error message if not successful.
     * @return Expected<void>
     */
    [[nodiscard]] Expected<void> send_status(bool success, std::string_view message = "");

private:
    const Config& config_;
};

} // namespace ju
