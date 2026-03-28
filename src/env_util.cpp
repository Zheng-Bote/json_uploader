/**
 * SPDX-FileComment: Environment utility implementation for the json_uploader project.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file env_util.cpp
 * @version 1.0.0
 * @date 2026-03-28
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "env_util.hpp"
#include <dotenv.h>
#include <sodium.h>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#define setenv(k, v, o) _putenv_s(k, v)
extern char** _environ;
#define environ _environ
#else
extern char** environ;
#endif

namespace ju {

namespace {
/**
 * @brief Decrypts data using libsodium.
 * Format: SALT(16) + NONCE(24) + CIPHERTEXT
 */
Expected<std::string> decrypt_env(const std::filesystem::path& path, const std::string& password) {
    if (sodium_init() < 0) {
        return std::unexpected(Error{ErrorCode::InternalError, "Libsodium init failed"});
    }

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return std::unexpected(Error{ErrorCode::EnvFileError, "Could not open encrypted file"});

    size_t size = static_cast<size_t>(file.tellg());
    if (size < crypto_pwhash_SALTBYTES + crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES) {
        return std::unexpected(Error{ErrorCode::EnvFileError, "Encrypted file too small"});
    }

    file.seekg(0, std::ios::beg);
    std::vector<unsigned char> salt(crypto_pwhash_SALTBYTES);
    std::vector<unsigned char> nonce(crypto_secretbox_NONCEBYTES);
    std::vector<unsigned char> ciphertext(size - salt.size() - nonce.size());

    file.read(reinterpret_cast<char*>(salt.data()), static_cast<std::streamsize>(salt.size()));
    file.read(reinterpret_cast<char*>(nonce.data()), static_cast<std::streamsize>(nonce.size()));
    file.read(reinterpret_cast<char*>(ciphertext.data()), static_cast<std::streamsize>(ciphertext.size()));

    std::vector<unsigned char> key(crypto_secretbox_KEYBYTES);
    if (crypto_pwhash(key.data(), key.size(), password.c_str(), password.size(), salt.data(),
                      crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
                      crypto_pwhash_ALG_DEFAULT) != 0) {
        return std::unexpected(Error{ErrorCode::InternalError, "Key derivation failed"});
    }

    std::string decrypted;
    decrypted.resize(ciphertext.size() - crypto_secretbox_MACBYTES);

    if (crypto_secretbox_open_easy(reinterpret_cast<unsigned char*>(decrypted.data()), 
                                   ciphertext.data(), ciphertext.size(), nonce.data(), key.data()) != 0) {
        return std::unexpected(Error{ErrorCode::EnvFileError, "Decryption failed (wrong password?)"});
    }

    return decrypted;
}

/**
 * @brief Simple parser for .env string (mimics dotenv-cpp basic logic)
 */
void parse_env_string(const std::string& content) {
    std::stringstream ss(content);
    std::string line;
    while (std::getline(ss, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 1);

        auto trim = [](std::string& s) {
            s.erase(0, s.find_first_not_of(" \t\r\n"));
            auto last = s.find_last_not_of(" \t\r\n");
            if (last != std::string::npos) s.erase(last + 1);
            else s.clear();
        };
        trim(key);
        trim(val);

        if (!key.empty()) {
            if (std::getenv(key.c_str()) == nullptr) {
#ifdef _WIN32
                _putenv_s(key.c_str(), val.c_str());
#else
                setenv(key.c_str(), val.c_str(), 1);
#endif
            }
        }
    }
}
} // namespace

Expected<void> load_env_file(const Config& config) {
    const auto& env_path = config.env_path;
    if (!std::filesystem::exists(env_path)) {
        return std::unexpected(Error{ErrorCode::EnvFileError, "ENV file does not exist: " + env_path.string()});
    }

    if (config.env_encrypted) {
        const char* pass = std::getenv(config.env_pass_var.c_str());
        if (!pass) {
            return std::unexpected(Error{ErrorCode::EnvFileError, "Encryption password not found in environment variable: " + config.env_pass_var});
        }

        auto decrypted = decrypt_env(env_path, pass);
        if (!decrypted) return std::unexpected(decrypted.error());

        parse_env_string(*decrypted);
    } else {
        try {
            dotenv::init(dotenv::Preserve, env_path.string().c_str());
        } catch (const std::exception& e) {
            return std::unexpected(Error{ErrorCode::EnvFileError, std::string("Could not load ENV file: ") + e.what()});
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

std::map<std::string, std::string> get_meta_env() {
    std::map<std::string, std::string> meta;
    for (char** env = environ; *env != nullptr; ++env) {
        std::string entry(*env);
        if (entry.starts_with("META_")) {
            auto pos = entry.find('=');
            if (pos != std::string::npos) {
                std::string key = entry.substr(5, pos - 5);
                std::string val = entry.substr(pos + 1);
                if (!key.empty()) {
                    meta[key] = val;
                }
            }
        }
    }
    return meta;
}

} // namespace ju
