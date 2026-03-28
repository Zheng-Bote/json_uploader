/**
 * SPDX-FileComment: CLI parsing for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file cli_parser_ctrl.hpp
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

namespace ju {

/**
 * @brief Controller for CLI parsing.
 */
class CliParserCtrl {
public:
    /**
     * @brief Parses CLI arguments.
     * 
     * @param argc Argument count.
     * @param argv Argument vector.
     * @return Expected<Config>
     */
    [[nodiscard]] static Expected<Config> parse(int argc, char** argv);
};

} // namespace ju
