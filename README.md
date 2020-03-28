# cetrinet (name subject to change) by coderobe

## Dependencies
- meson
- clang++ (with c++17 support)
- TGUI https://github.com/texus/TGUI **(v 0.8.7)**
- sfml-graphics (required by TGUI)
- nlohmann/json https://github.com/nlohmann/json
- Simple WebSocket Server https://gitlab.com/eidheim/Simple-WebSocket-Server

### Optional
- make

## Build

### On macOS
- Use something like `export PKG_CONFIG_PATH=/usr/local/Cellar/openssl/1.0.2n/lib/pkgconfig:vendor/pkgconfig`
- Run `git clone https://github.com/eidheim/simple-websocket-server vendor/include/simple-websocket-server`
- Extract `https://github.com/nlohmann/json/releases/download/v3.7.3/include.zip` into `vendor/include`
- Build with meson

### With meson
- `CXX=clang++ meson . build`
- `cd build`
- `ninja`

### With make (invokes meson)
- `make`

## Run
- `build/cetrinet`
