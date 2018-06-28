#!/bin/sh

for SPRITE_SIZE in 1024 2048 4096 8192 16384 32768
do
  clang main_instancing.c -O3 -D SPRITES_SIZE=$SPRITE_SIZE -Wall -o build/instancing_$SPRITE_SIZE.exe \
    -I /opt/local/include -L /opt/local/lib -l glew \
    -framework SDL2 -framework SDL2_image -framework OpenGL \
    -F $HOME/Library/Frameworks \
    -I $HOME/Library/Frameworks/SDL2.framework/Headers \
    -I $HOME/Library/Frameworks/SDL2_image.framework/Headers

  clang main_sdl.c -O3 -D SPRITES_SIZE=$SPRITE_SIZE  -Wall -o build/sdl_$SPRITE_SIZE.exe \
    -framework SDL2 -framework SDL2_image -framework OpenGL \
    -F $HOME/Library/Frameworks \
    -I $HOME/Library/Frameworks/SDL2.framework/Headers \
    -I $HOME/Library/Frameworks/SDL2_image.framework/Headers
done
