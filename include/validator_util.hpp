/**
 * SPDX-FileComment: Validator utility for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file validator_util.hpp
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
#include <valijson/schema.hpp>

namespace ju {

/**
 * @brief Utility for JSON validation against a schema.
 */
class ValidatorUtil {
public:
    explicit ValidatorUtil(const Config& config);
    ~ValidatorUtil();

    /**
     * @brief Loads the validation schema.
     * @return Expected<void>
     */
    [[nodiscard]] Expected<void> load_schema();

    /**
     * @brief Validates a JSON string.
     * @param json_data The JSON data.
     * @return Expected<void>
     */
    [[nodiscard]] Expected<void> validate(std::string_view json_data);

private:
    const Config& config_;
    valijson::Schema schema_;
};

} // namespace ju
