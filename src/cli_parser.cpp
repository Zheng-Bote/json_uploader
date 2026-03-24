/**
 * SPDX-FileComment: CLI parsing implementation for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file cli_parser.cpp
 * @version 0.2.8
 * @date 2026-03-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "cli_parser.hpp"
#include <cstdlib>
#include <string_view>
#include <vector>

namespace ju {

static std::optional<std::string> get_env(const char* key) {
    if (const char* val = std::getenv(key)) {
        return std::string(val);
    }
    return std::nullopt;
}

Expected<Config> parse_cli(int argc, char** argv) {
    Config config;
    std::vector<std::string_view> args(argv + 1, argv + argc);

    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--json" && i + 1 < args.size()) {
            config.json_path = args[++i];
        } else if (args[i] == "--schema" && i + 1 < args.size()) {
            config.schema_path = args[++i];
        } else if (args[i] == "--email") {
            config.send_email = true;
        }
    }

    if (config.json_path.empty()) {
        return std::unexpected(Error{ErrorCode::InvalidArguments, "Missing --json parameter"});
    }
    if (config.schema_path.empty()) {
        return std::unexpected(Error{ErrorCode::InvalidArguments, "Missing --schema parameter"});
    }

    auto api_url = get_env("API_URL");
    auto api_user = get_env("API_USER");
    auto api_password = get_env("API_PASSWORD");
    auto api_email = get_env("API_EMAIL");

    if (!api_url || !api_user || !api_password) {
        return std::unexpected(Error{ErrorCode::EnvironmentMissing, 
            "Required ENV variables missing (API_URL, API_USER, API_PASSWORD)"});
    }

    config.api_url = *api_url;
    config.api_user = *api_user;
    config.api_password = *api_password;
    config.api_email = api_email;

    if (config.send_email && !config.api_email) {
        return std::unexpected(Error{ErrorCode::EnvironmentMissing, 
            "--email set but API_EMAIL ENV variable missing"});
    }

    return config;
}

} // namespace ju
