#!/bin/bash

LINUX_QMAKE="qmake"

WIN32_HOST="i686-w64-mingw32"
WIN32_QMAKE="/opt/qt-4.8.4-mingw/bin/qmake"

HIVEX_CONFIGURE_OPTIONS="--disable-ocaml --disable-perl --disable-python --disable-ruby --disable-shared"

PrintUsage() {
  echo "Usage: $0 <platform>"
  echo
  echo "Options:"
  echo "  platform: Specify the platform fred should be build for (linux or win32)."
  echo
  exit 1
}

PLATFORM=""
[ -z "$1" ] && PrintUsage
[ "$1" = "linux" ] && PLATFORM="linux"
[ "$1" = "win32" ] && PLATFORM="win32"
[ -z "$PLATFORM" ] && PrintUsage

echo "Bootstrapping fred..."
git submodule init
git submodule update

echo "Bootstrapping hivex..."
(
  cd hivex
  if [ "$PLATFORM" = "linux" ]; then
    ./autogen.sh $HIVEX_CONFIGURE_OPTIONS
  fi
  if [ "$PLATFORM" = "win32" ]; then
    ./autogen.sh --host=$WIN32_HOST $HIVEX_CONFIGURE_OPTIONS
  fi
)

echo "Building hivex..."
(
  cd hivex
  make
)

echo "Building fred..."
if [ "$PLATFORM" = "linux" ]; then
  $LINUX_QMAKE
  make
fi
if [ "$PLATFORM" = "win32" ]; then
  $WIN32_QMAKE
  make
fi

echo "All done."

