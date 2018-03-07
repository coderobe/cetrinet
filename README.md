# cetrinet (name subject to change) by coderobe

## Dependencies
- meson
- clang
- LCUI https://github.com/lc-soft/LCUI
- lcui.css https://github.com/lc-ui/lcui.css
- nlohmann/json https://github.com/nlohmann/json
- Simple WebSocket Server https://github.com/eidheim/Simple-WebSocket-Server
- minify https://github.com/tdewolff/minify

### Optional
- make

## Build

### With meson
- `CXX=clang++ meson . build`
- `cd build`
- `ninja`

### With make (invokes meson)
- `make`

## Run
- `build/cetrinet`
