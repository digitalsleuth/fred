#!/bin/bash

# When compiling under mingw, make sure PKG_CONFIG_PATH is set
uname | grep MINGW &>/dev/null
if [ $? -eq 0 ]; then
  export PKG_CONFUG_PATH="/local/lib/pkgconfig"
fi

cd hivex
make clean
./configure --disable-ocaml --disable-perl --disable-python --disable-ruby && make
cd ..

