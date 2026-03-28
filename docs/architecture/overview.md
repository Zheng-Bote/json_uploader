# Architectural Overview - JSON Uploader

This document provides a detailed overview of the system architecture, design patterns, and data flow within the JSON Uploader project.

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Design Patterns](#design-patterns)
- [Diagrams](#diagrams)
  - [1. Bounded Context & Interfaces](#1-bounded-context--interfaces)
  - [2. Class Diagram](#2-class-diagram)
  - [3. Sequence Diagram: Upload Workflow](#3-sequence-diagram-upload-workflow)
  - [4. Component Diagram](#4-component-diagram)
- [Data Flow](#data-flow)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## Design Patterns

- **Service-Provider Pattern**: Core business logic is encapsulated in services (`AuthSrv`, `UploaderSrv`, `EmailSrv`).
- **Controller Pattern**: CLI parsing and main workflow orchestration are handled by controllers (`CliParserCtrl`).
- **Utility Pattern**: Reusable, stateless logic is implemented in utility modules (`EnvUtil`, `LoggerUtil`, `ValidatorUtil`).
- **Streaming Architecture**: Uses a callback-driven approach for data processing to handle large files without excessive memory consumption.

## Diagrams

### 1. Bounded Context & Interfaces

This diagram illustrates the external interfaces and the main functional boundaries of the application.

```mermaid
graph TD
    subgraph LocalSystem [Local System]
        User((User))
        CLI[CLI Parser Ctrl]
        ENV[ENV Util]
        Logger[Logger Util]
        FS[(File System)]
    end

    subgraph ExternalServices [External Services]
        API_Auth(API Auth Endpoint)
        API_Upload(API Upload Endpoint)
        SMTP(SMTP Server)
    end

    User -- CLI Args --> CLI
    CLI -- Read --> FS
    CLI -- Load --> ENV
    ENV -- Decrypt/Parse --> FS
    CLI -- Init --> Logger
    CLI -- POST Login --> API_Auth
    CLI -- Streaming POST --> API_Upload
    CLI -- SMTP Send --> SMTP
```

### 2. Class Diagram

```mermaid
classDiagram
    class Config {
        +path json_path
        +path schema_path
        +path env_path
        +bool env_encrypted
        +string env_pass_var
        +CompressionType api_compression
        +map metadata
    }

    class CliParserCtrl {
        +static parse(int argc, char** argv) Expected~Config~
    }

    class EnvUtil {
        +load_env_file(Config config) Expected~void~
        +get_env(string key) string
        +get_meta_env() map
    }

    class AuthSrv {
        -Config config
        +login() Expected~string~
    }

    class UploaderSrv {
        -Config config
        -string token
        +upload() Expected~void~
        -compress_helper(StreamState, data, FlushMode) bool
    }

    class EmailSrv {
        -Config config
        +send_status(bool success, string msg) Expected~void~
    }

    CliParserCtrl ..> Config : creates
    AuthSrv --> Config : uses
    UploaderSrv --> Config : uses
    EmailSrv --> Config : uses
    EnvUtil --> Config : modifies
```

### 3. Sequence Diagram: Upload Workflow

```mermaid
sequenceDiagram
    participant Main
    participant CLI as CliParserCtrl
    participant ENV as EnvUtil
    participant Auth as AuthSrv
    participant Up as UploaderSrv
    participant API

    Main->>CLI: parse(argc, argv)
    CLI-->>Main: Config
    Main->>ENV: load_env_file(Config)
    ENV->>Main: (Env Loaded)
    Main->>Auth: login()
    Auth->>API: POST /login
    API-->>Auth: Token
    Auth-->>Main: Token
    Main->>Up: upload()
    loop For each JSON object
        Up->>Up: Validate
        Up->>Up: Merge Metadata
        Up->>Up: Compress (Zstd/Gzip)
        Up->>API: POST /upload (Chunked)
    end
    API-->>Up: 200 OK
    Up-->>Main: Success
```

### 4. Component Diagram

```mermaid
graph TD
    Main[Main Orchestrator]
    CLI[CLI Controller]
    ENV[Environment Module]
    Sodium[Sodium Decryption]
    Auth[Auth Service]
    Up[Uploader Service]
    Val[Validation Engine]
    Comp[Compression Engine]

    Main --> CLI
    Main --> ENV
    ENV --> Sodium
    Main --> Auth
    Main --> Up
    Up --> Val
    Up --> Comp
```

## Data Flow

1.  **Configuration Phase**: CLI arguments are parsed into a `Config` object.
2.  **Environment Phase**: The `.env` file is loaded. If encrypted, `libsodium` is used for in-memory decryption.
3.  **Authentication Phase**: Credentials from the environment are used to obtain a Bearer Token.
4.  **Upload Phase**:
    - The JSON file is read as a stream using `simdjson`.
    - Each object is validated against the schema.
    - Environment variables prefixed with `META_` are merged into the object's `metadata` field.
    - The object is compressed using the configured algorithm (Zstd or Gzip).
    - Data is sent to the API via chunked transfer encoding.
5.  **Notification Phase**: If enabled, an email is sent with the final status.
