/**
 * SPDX-FileComment: Validator for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file validator.hpp
 * @version 1.0.0
 * @date 2026-03-28
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "config.hpp"
#include "error.hpp"
#include <simdjson.h>
#include <valijson/adapters/nlohmann_json_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>

namespace ju {

/**
 * @brief Class to validate JSON streaming using simdjson.
 */
class Validator {
public:
    explicit Validator(const Config& config);
    ~Validator();

    /**
     * @brief Loads the schema.
     * @return Expected<void>
     */
    [[nodiscard]] Expected<void> load_schema();

    /**
     * @brief Validates a single JSON document (can be called repeatedly for streaming).
     * @param json_data String containing a single JSON object.
     * @return Expected<void>
     */
    [[nodiscard]] Expected<void> validate(std::string_view json_data);

private:
    const Config& config_;
    valijson::Schema schema_;
};

} // namespace ju
