#include <chrono>
#include <crow.h>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>

#include <nlohmann/json.hpp>
using nlohmann::json;

namespace fs = std::filesystem;

struct ServerConfig {
  std::string apiUser;
  std::string apiPassword;
  uint16_t port = 18080;
};

ServerConfig parse_args(int argc, char **argv) {
  ServerConfig cfg;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--user" && i + 1 < argc) {
      cfg.apiUser = argv[++i];
    } else if (arg == "--password" && i + 1 < argc) {
      cfg.apiPassword = argv[++i];
    } else if (arg == "--port" && i + 1 < argc) {
      cfg.port = static_cast<uint16_t>(std::stoi(argv[++i]));
    } else {
      std::cerr << "Unbekannter Parameter: " << arg << "\n";
    }
  }

  if (cfg.apiUser.empty() || cfg.apiPassword.empty()) {
    throw std::runtime_error("Bitte --user und --password angeben.");
  }

  return cfg;
}

std::string generate_token() {
  static thread_local std::mt19937_64 rng{std::random_device{}()};
  static const char chars[] =
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

  std::uniform_int_distribution<size_t> dist(0, sizeof(chars) - 2);

  std::string token;
  token.reserve(64);
  for (int i = 0; i < 64; ++i) {
    token.push_back(chars[dist(rng)]);
  }
  return token;
}

std::string current_timestamp_string() {
  using namespace std::chrono;
  auto now = system_clock::now();
  auto t = system_clock::to_time_t(now);
  auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

  std::tm tm{};
#ifdef _WIN32
  localtime_s(&tm, &t);
#else
  localtime_r(&t, &tm);
#endif

  char buf[64];
  std::snprintf(buf, sizeof(buf), "%04d%02d%02d_%02d%02d%02d_%03d",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
                tm.tm_min, tm.tm_sec, static_cast<int>(ms.count()));
  return std::string{buf};
}

int main(int argc, char **argv) {
  try {
    auto cfg = parse_args(argc, argv);

    // Token-Store (in-memory, sehr simpel)
    std::unordered_set<std::string> validTokens;

    // Zielverzeichnis
    fs::path baseDir = fs::path("testserver") / "data";
    fs::create_directories(baseDir);

    crow::SimpleApp app;

    CROW_ROUTE(app, "/login")
        .methods(crow::HTTPMethod::Post)(
            [cfg, &validTokens](const crow::request &req) {
              // Erwartet JSON: { "user": "...", "password": "..." }
              auto body = req.body;
              crow::json::rvalue json;
              try {
                json = crow::json::load(body);
              } catch (...) {
                return crow::response(400, "Invalid JSON");
              }

              if (!json || !json.has("user") || !json.has("password")) {
                return crow::response(400, "Missing user or password");
              }

              std::string user = json["user"].s();
              std::string password = json["password"].s();

              if (user == cfg.apiUser && password == cfg.apiPassword) {
                std::string token = generate_token();
                validTokens.insert(token);

                crow::json::wvalue resp;
                resp["token"] = token;
                return crow::response{resp};
              } else {
                return crow::response(401, "Unauthorized");
              }
            });

    CROW_ROUTE(app, "/upload")
        .methods(crow::HTTPMethod::Post)([&validTokens,
                                          baseDir](const crow::request &req) {
          // std::cout << "--- START RAW BODY ---\n"
          //           << req.body << "\n--- END RAW BODY ---\n";

          // Authorization: Bearer <token>
          auto authIt = req.headers.find("Authorization");
          if (authIt == req.headers.end()) {
            return crow::response(401, "Missing Authorization header");
          }

          std::string auth = authIt->second;
          const std::string prefix = "Bearer ";
          if (auth.rfind(prefix, 0) != 0) {
            return crow::response(401, "Invalid Authorization header");
          }

          std::string token = auth.substr(prefix.size());
          if (validTokens.find(token) == validTokens.end()) {
            return crow::response(401, "Invalid token");
          }

          // Body prüfen
          if (req.body.empty()) {
            return crow::response(400, "Empty body");
          }

          // JSON-Validierung mit nlohmann::json
          try {
            json parsed = json::parse(req.body);

            // Optional: minimale Strukturprüfung
            if (!parsed.is_object() && !parsed.is_array()) {
              return crow::response(400, "JSON must be object or array");
            }

            // Check for META_DEBUG=yes (stored as metadata.DEBUG or top-level
            // DEBUG)
            auto check_debug = [](const json &obj) {
              bool debug_found = false;
              json meta_data = json::object();

              if (obj.contains("metadata") && obj["metadata"].is_object()) {
                const auto &meta = obj["metadata"];
                if (meta.contains("DEBUG") && meta["DEBUG"] == "yes") {
                  debug_found = true;
                  meta_data = meta;
                }
              } else if (obj.contains("DEBUG") && obj["DEBUG"] == "yes") {
                debug_found = true;
                meta_data = obj; // Use whole object as context for single mode
              }

              if (debug_found) {
                std::cout << "--- METADATA DEBUG ---\n";
                std::cout << meta_data.dump(2) << "\n";
                std::cout << "----------------------\n";
              }
            };

            if (parsed.is_array()) {
              if (!parsed.empty())
                check_debug(parsed[0]);
            } else {
              check_debug(parsed);
            }

          } catch (const std::exception &e) {
            return crow::response(400,
                                  std::string("Invalid JSON: ") + e.what());
          }

          // Datei speichern
          std::string filename =
              "upload_" + current_timestamp_string() + ".json";
          fs::path outPath = baseDir / filename;

          try {
            std::ofstream ofs(outPath, std::ios::binary);
            if (!ofs) {
              return crow::response(500, "Failed to open output file");
            }
            ofs << req.body;
          } catch (const std::exception &e) {
            return crow::response(500,
                                  std::string("File write error: ") + e.what());
          }

          crow::json::wvalue resp;
          resp["status"] = "ok";
          resp["file"] = outPath.string();
          return crow::response{resp};
        });

    std::cout << "Starte Crow-Server auf Port " << cfg.port << "\n";
    app.port(cfg.port).multithreaded().run();
  } catch (const std::exception &e) {
    std::cerr << "Fatal: " << e.what() << "\n";
    return 1;
  }
}
