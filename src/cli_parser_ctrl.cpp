/**
 * SPDX-FileComment: CLI parsing implementation for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file cli_parser_ctrl.cpp
 * @version 1.0.0
 * @date 2026-03-28
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "cli_parser_ctrl.hpp"
#include <vector>
#include <string_view>
#include <filesystem>

namespace ju {

Expected<Config> CliParserCtrl::parse(int argc, char** argv) {
    Config config;
    std::vector<std::string_view> args(argv + 1, argv + argc);

    // Default ENV path: <program_dir>/../data/json_uploader.env
    std::filesystem::path prog_path = std::filesystem::absolute(argv[0]);
    config.env_path = prog_path.parent_path().parent_path() / "data" / "json_uploader.env";

    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--json" && i + 1 < args.size()) {
            config.json_path = args[++i];
        } else if (args[i] == "--schema" && i + 1 < args.size()) {
            config.schema_path = args[++i];
        } else if (args[i] == "--email") {
            config.send_email = true;
        } else if (args[i] == "--env" && i + 1 < args.size()) {
            config.env_path = args[++i];
        } else if (args[i] == "--encrypted") {
            config.env_encrypted = true;
        } else if (args[i] == "--env-pass-var" && i + 1 < args.size()) {
            config.env_pass_var = args[++i];
        }
    }

    if (config.json_path.empty()) {
        return std::unexpected(Error{ErrorCode::InvalidArguments, "Missing --json parameter"});
    }
    if (config.schema_path.empty()) {
        return std::unexpected(Error{ErrorCode::InvalidArguments, "Missing --schema parameter"});
    }

    return config;
}

} // namespace ju
