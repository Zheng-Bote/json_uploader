/**
 * SPDX-FileComment: Environment utility implementation for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file env_util.cpp
 * @version 0.2.8
 * @date 2026-03-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "env_util.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#define setenv(k, v, o) _putenv_s(k, v)
#endif

namespace ju {

Expected<void> load_env_file(const std::filesystem::path& env_path) {
    if (!std::filesystem::exists(env_path)) {
        return std::unexpected(Error{ErrorCode::EnvFileError, "ENV file does not exist: " + env_path.string()});
    }

    std::ifstream file(env_path);
    if (!file.is_open()) {
        return std::unexpected(Error{ErrorCode::EnvFileError, "Could not open ENV file"});
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines or comments
        if (line.empty() || line[0] == '#') continue;

        size_t delimiter_pos = line.find('=');
        if (delimiter_pos == std::string::npos) continue;

        std::string key = line.substr(0, delimiter_pos);
        std::string value = line.substr(delimiter_pos + 1);

        // Basic trimming (optional but recommended)
        auto trim = [](std::string& s) {
            s.erase(0, s.find_first_not_of(" \t\r\n"));
            s.erase(s.find_last_not_of(" \t\r\n") + 1);
        };
        trim(key);
        trim(value);

        if (!key.empty()) {
#ifdef _WIN32
            _putenv_s(key.c_str(), value.c_str());
#else
            setenv(key.c_str(), value.c_str(), 1);
#endif
        }
    }

    return {};
}

std::string get_env(const std::string& key, const std::string& default_val) {
    if (const char* val = std::getenv(key.c_str())) {
        return std::string(val);
    }
    return default_val;
}

} // namespace ju
