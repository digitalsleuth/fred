#!/bin/bash

# When compiling under mingw, make sure PKG_CONFIG_PATH is set
uname | grep MINGW &>/dev/null
if [ $? -eq 0 ]; then
  export PKG_CONFIG_PATH="/local/lib/pkgconfig"
fi

cd hivex
autoreconf
./configure --disable-ocaml --disable-perl --disable-python --disable-ruby && make
cd ..

