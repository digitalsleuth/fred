#!/bin/bash

cd hivex
./configure --without-ocaml --without-perl --without-python --without-ruby
make
cd ..

