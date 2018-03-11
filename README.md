# cetrinet (name subject to change) by coderobe

## Dependencies
- meson
- clang++ (with c++17 support)
- TGUI https://github.com/texus/TGUI **(v 0.7)**
- nlohmann/json https://github.com/nlohmann/json
- Simple WebSocket Server https://github.com/eidheim/Simple-WebSocket-Server

### Optional
- make
- minify https://github.com/tdewolff/minify
- upx https://github.com/upx/upx

## Build

### On macOS
- Use something like `export PKG_CONFIG_PATH=/usr/local/Cellar/openssl/1.0.2n/lib/pkgconfig:vendor/pkgconfig`
- Run `git clone https://github.com/eidheim/simple-websocket-server`
- Extract `https://github.com/nlohmann/json/releases/download/v3.1.1/include.zip` into `vendor/include`
- Build with meson

### With meson
- `CXX=clang++ meson . build`
- `cd build`
- `ninja`

### With make (invokes meson)
- `make`

## Run
- `build/cetrinet`
