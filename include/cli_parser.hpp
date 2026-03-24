/**
 * SPDX-FileComment: CLI parsing for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file cli_parser.hpp
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

namespace ju {

/**
 * @brief Parses CLI arguments and loads environment variables.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Expected<Config> The configuration or an error.
 */
[[nodiscard]] Expected<Config> parse_cli(int argc, char** argv);

} // namespace ju
