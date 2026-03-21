#!/bin/bash
export EM_CACHE=~/.emscripten_cache
rm -rf ../dist
mkdir ../dist
time emcc -o ../dist/index.html main.cpp -Wall -std=c++14 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os -I. -I ./physfs/src -I ./raylib/src -I ./raylib/src/external -L. -L ./raylib/src -s USE_GLFW=3 -s ASYNCIFY -s INITIAL_HEAP=16777216 -s STACK_SIZE=4194304 -s FORCE_FILESYSTEM=1 --preload-file FS --shell-file ./shell.html ./raylib/src/web/libraylib.web.a ./physfs/build/libphysfs.a -DPLATFORM_WEB -s 'EXPORTED_FUNCTIONS=["_free","_malloc","_main"]' -s EXPORTED_RUNTIME_METHODS=ccall
mv ../dist/* ../public/
cp -r ../public/* ../dist/