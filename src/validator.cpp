/**
 * SPDX-FileComment: Validator implementation for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file validator.cpp
 * @version 1.0.0
 * @date 2026-03-28
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "validator.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <valijson/adapters/nlohmann_json_adapter.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>
#include <valijson/validation_results.hpp>

namespace ju {

Validator::Validator(const Config& config) : config_(config) {}
Validator::~Validator() = default;

Expected<void> Validator::load_schema() {
    std::ifstream schema_file(config_.schema_path);
    if (!schema_file.is_open()) {
        return std::unexpected(Error{ErrorCode::FileReadError, "Could not open schema file: " + config_.schema_path.string()});
    }

    try {
        nlohmann::json schema_json = nlohmann::json::parse(schema_file);
        valijson::adapters::NlohmannJsonAdapter schema_adapter(schema_json);
        valijson::SchemaParser parser;
        parser.populateSchema(schema_adapter, schema_);
    } catch (const std::exception& e) {
        return std::unexpected(Error{ErrorCode::SchemaValidationError, "Failed to parse schema: " + std::string(e.what())});
    }

    return {};
}

Expected<void> Validator::validate(std::string_view json_data) {
    try {
        // Use nlohmann::json for validation (valijson requirement)
        // But simdjson is used in the uploader for the main parsing flow as requested.
        // For strict schema validation, we adapt to valijson.
        nlohmann::json doc = nlohmann::json::parse(json_data);
        valijson::adapters::NlohmannJsonAdapter doc_adapter(doc);
        
        valijson::Validator validator;
        valijson::ValidationResults results;
        if (!validator.validate(schema_, doc_adapter, &results)) {
            std::string errors;
            valijson::ValidationResults::Error error;
            while (results.popError(error)) {
                errors += error.description + "; ";
            }
            return std::unexpected(Error{ErrorCode::SchemaValidationError, "Validation failed: " + errors});
        }
    } catch (const std::exception& e) {
        return std::unexpected(Error{ErrorCode::JsonParseError, "JSON Parse Error during validation: " + std::string(e.what())});
    }

    return {};
}

} // namespace ju
