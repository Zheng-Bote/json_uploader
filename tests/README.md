# Test Server

<!-- DOCTOC SKIP -->

Test-Server (API-Server) for testing json_uploader

## Build

```bash
mkdir build && cd build
cmake ..
cmake --build .

# user and password for clinet authentication
# User name and password can be changed in the code
./crow_test_server --user myuser --password secret --port 8080
```

## Usage

### Login

```bash
curl -X POST http://localhost:8080/login \
  -H "Content-Type: application/json" \
  -d '{"user":"myuser","password":"secret"}'
```

Antwort enthält {"token":"..."}.

### Upload

```bash
curl -X POST http://localhost:8080/upload \
  -H "Authorization: Bearer <token>" \
  -H "Content-Type: application/json" \
  -d '{"key":"value"}'
```

```bash
curl -X POST http://localhost:8080/upload \
  -H "Authorization: Bearer <token>" \
  -H "Content-Type: application/json" \
  -d @data.json
```
