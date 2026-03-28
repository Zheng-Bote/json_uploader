/**
 * SPDX-FileComment: Authentication service for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file auth_srv.hpp
 * @version 1.0.0
 * @date 2026-03-28
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
 * @brief Service to handle API authentication.
 */
class AuthSrv {
public:
    explicit AuthSrv(const Config& config);
    ~AuthSrv();

    /**
     * @brief Performs login to obtain Bearer Token.
     * @return Expected<std::string>
     */
    [[nodiscard]] Expected<std::string> login();

private:
    const Config& config_;
};

} // namespace ju
