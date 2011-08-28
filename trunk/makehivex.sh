#!/bin/bash

cd hivex
./configure --disable-ocaml --disable-perl --disable-python --disable-ruby
make
cd ..

