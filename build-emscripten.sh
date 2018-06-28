#!/bin/sh

emcc instancing.c -Wall -s WASM=1 \
  -o build/instancing.html \
  -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' \
  --preload-file asset_dir
