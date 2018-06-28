#!/bin/sh

for i in 1024 2048 4096 8192 16384 32768
do
  emcc main_instancing.c -Wall -s WASM=1 -Oz \
    -D SPRITES_SIZE=$i \
    -o build/instancing_$i.html \
    -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' \
    --preload-file asset_dir
done

for i in 1024 2048 4096 #,8192,16384,32768
do
  emcc main_sdl.c -Wall -s WASM=1 -Oz -D SPRITES_SIZE=$i \
    -o build/sdl_$i.html \
    -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' \
    --preload-file asset_dir
done
