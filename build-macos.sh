#!/bin/sh

clang instancing.c  -Wall -o build/instancing.exe \
  -I /opt/local/include -L /opt/local/lib -l glew \
  -framework SDL2 -framework SDL2_image -framework OpenGL \
  -F $HOME/Library/Frameworks \
  -I $HOME/Library/Frameworks/SDL2.framework/Headers \
  -I $HOME/Library/Frameworks/SDL2_image.framework/Headers
