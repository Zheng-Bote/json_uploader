/**
 * SPDX-FileComment: Authentication client for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file auth_client.hpp
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

class AuthClient {
public:
    explicit AuthClient(const Config& config);
    ~AuthClient();

    /**
     * @brief Performs login to get Bearer Token.
     * @return Expected<std::string> The token or error.
     */
    [[nodiscard]] Expected<std::string> login();

private:
    const Config& config_;
};

} // namespace ju
