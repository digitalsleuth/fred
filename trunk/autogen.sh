#!/bin/bash

# -----------------------------------------------------------------------------
# Default command line options.
# -----------------------------------------------------------------------------

OPT_JOBS=1
OPT_ONLY_BOOTSTRAP=0
OPT_PLATFORM="linux"
OPT_STATIC_HIVEX=0

OPT_LINUX_QMAKE=`which qmake`

OPT_WIN32_COMPILER_SUFFIX="i686-w64-mingw32"
OPT_WIN32_QMAKE="/opt/qt-4.8.4-mingw/bin/qmake"

# -----------------------------------------------------------------------------
# ------------------ DO NOT CHANGE ANYTHING BELOW THIS LINE -------------------
# -----------------------------------------------------------------------------

# Try to make somehow sure we are running in bash and not some other shell
if [ -z "$BASH_VERSION" ]; then
  echo "ERROR: This script must be run in a bash shell! Try using \"bash $0\""
  exit 1
fi

# -----------------------------------------------------------------------------
# Function declarations
# -----------------------------------------------------------------------------

# Print usage and exit
PrintUsage() {
  echo
  echo "Usage:"
  echo "  $0 [options]"
  echo
  echo "Options:"
  echo "  --help: Print this help message."
  echo "  --jobs=<number> (Def.: 1) : Specify how many make jobs should be run simultaneously."
  echo "  --linux-qmake=<qmakebin> (Def.: qmake) : Specify the linux qmake binary to use."
  echo "  --only-bootstrap[=0..1] (Def.: 0) : Only bootstrap, but do not compile fred."
  echo "  --platform=<platform> (Def.: linux) : Specify the platform fred should be build for. Available platforms are 'linux' and 'win32'."
  echo "  --static-hivex[=0..1] (Def.: 0): Build and link in-tree hivex statically."
  echo "  --win32-compiler-suffix=<suffix> (Def.: i686-w64-mingw32) : Specify the win32 crosscompiler suffix to use."
  echo "  --win32-qmake=<qmakebin> (Def.: /opt/qt-4.8.4-mingw/bin/qmake) : Specify the win32 qmake binary to use."
  echo
  exit 1
}

# Extract argument value
get_arg_val() {
  local TMP=`echo "$1" | cut -d= -f2`
  if [ "$1" = "$TMP" ]; then
    # No arg specified for option, assume 1
    echo 1
  else
    if [[ -z "$TMP" || $(echo -n "$TMP" | sed 's/[0-9]//g' | wc -c) -ne 0 ]]; then
      echo "ERROR: Non-integer arg for option '$1' specified!" 1>&2
      exit 1
    fi
    echo $TMP
  fi
}

# -----------------------------------------------------------------------------
# Parse command line args
# -----------------------------------------------------------------------------

shopt extglob &>/dev/null
EXTGLOB=$?
shopt -s extglob &>/dev/null
while :; do
  case "$1" in
    --help)
      PrintUsage
      ;;
    --jobs=*)
      OPT_JOBS=$(get_arg_val "$1") || PrintUsage
      shift
      ;;
    --linux-qmake=*)
      TMP=`echo "$1" | cut -d= -f2`
      if [[ -z "$TMP" || "$1" = "$TMP" ]]; then
        echo "ERROR: No option arg for '$1' specified!"
        PrintUsage
      fi
      if [ ! -x "$TMP" ]; then
        echo "ERROR: The specified linux qmake binary '$TMP' does not exist or is not executable!"
        exit 1
      fi
      OPT_LINUX_QMAKE="$TMP"
      shift
      ;;
    --only-bootstrap?(=[01]))
      OPT_ONLY_BOOTSTRAP=$(get_arg_val "$1") || PrintUsage
      shift
      ;;
    --platform=*)
      TMP=`echo "$1" | cut -d= -f2`
      if [[ -z "$TMP" || "$1" = "$TMP" ]]; then
        echo "ERROR: No option arg for '$1' specified!"
        PrintUsage
      fi
      TMP=`echo "$TMP" | tr "[A-Z]" "[a-z]"`
      if [[ "$TMP" != "linux" && "$TMP" != "win32" ]]; then
        echo "ERROR: Unsupported platform '$TMP' specified!"
        PrintUsage
      fi
      OPT_PLATFORM="$TMP"
      shift
      ;;
    --static-hivex?(=[01]))
      OPT_STATIC_HIVEX=$(get_arg_val "$1") || PrintUsage
      shift
      ;;
    --win32-compiler-suffix=*)
      TMP=`echo "$1" | cut -d= -f2`
      if [[ -z "$TMP" || "$1" = "$TMP" ]]; then
        echo "ERROR: No option arg for '$1' specified!"
        PrintUsage
      fi
      if [[ ! -x "$(which \"${TMP}-gcc\")" || ! -x "$(which \"${TMP}-g++\")" ]]; then
        echo "ERROR: Couldn't find '${TMP}-gcc' or '${TMP}-g++'!"
        echo "ERROR: The specified win32 compiler suffix does not seem to be correct!"
        exit 1
      fi
      OPT_WIN32_COMPILER_SUFFIX="$TMP"
      shift
      ;;
    --win32-qmake=*)
      TMP=`echo "$1" | cut -d= -f2`
      if [[ -z "$TMP" || "$1" = "$TMP" ]]; then
        echo "ERROR: No option arg for '$1' specified!"
        PrintUsage
      fi
      if [ ! -x "$TMP" ]; then
        echo "ERROR: The specified win32 qmake binary '$TMP' does not exist or is not executable!"
        exit 1
      fi
      OPT_WIN32_QMAKE="$TMP"
      shift
      ;;
    --*)
      echo "ERROR: Unknown option / Wrong option arg '$1' specified!" 1>&2
      PrintUsage
      ;;
    *)
      break
      ;;
  esac
done
if [ $EXTGLOB -ne 0 ]; then
  shopt -u extglob &>/dev/null
fi

# -----------------------------------------------------------------------------
# Check command line args
# -----------------------------------------------------------------------------

if [ "$OPT_PLATFORM" = "linux" ]; then
  if [ ! -x "$OPT_LINUX_QMAKE" ]; then
    echo "ERROR: Couldn't find qmake! Consider specifying it with --linux-qmake."
    exit 1
  fi
fi

if [ "$OPT_PLATFORM" = "win32" ]; then
  if [ ! -x "$OPT_WIN32_QMAKE" ]; then
    echo "ERROR: Couldn't find qmake! Consider specifying it with --win32-qmake."
    exit 1
  fi
fi

# -----------------------------------------------------------------------------
# Build
# -----------------------------------------------------------------------------

# Get script directory and cd to it
SCRIPT_DIR=`dirname "$0"`
(
  cd "$SCRIPT_DIR"

  # When requested to build static, init, bootstrap, configure and make hivex
  if [ $OPT_STATIC_HIVEX -eq 1 ]; then
    echo "-----------------------------------------------------------------------------"
    echo "Bootstrapping fred"
    echo "-----------------------------------------------------------------------------"
    (
      git submodule init
      git submodule update
    )

    echo "-----------------------------------------------------------------------------"
    echo "Bootstrapping hivex"
    echo "-----------------------------------------------------------------------------"
    (
      cd hivex
      if [ "$OPT_PLATFORM" = "linux" ]; then
        ./autogen.sh --disable-ocaml --disable-perl --disable-python --disable-ruby --disable-shared || exit 1
      fi
      if [ "$OPT_PLATFORM" = "win32" ]; then
        ./autogen.sh --host=$OPT_WIN32_COMPILER_SUFFIX --disable-ocaml --disable-perl --disable-python --disable-ruby --disable-shared || exit 1
      fi
    )
    [ $? -ne 0 ] && exit 1

    echo "-----------------------------------------------------------------------------"
    echo "Building hivex"
    echo "-----------------------------------------------------------------------------"
    (
      cd hivex
      make clean &>/dev/null
      make -j$OPT_JOBS || exit 1
    )
    [ $? -ne 0 ] && [ "$OPT_PLATFORM" != "win32" ] && exit 1
  fi

  if [ $OPT_ONLY_BOOTSTRAP -eq 0 ]; then
    echo "-----------------------------------------------------------------------------"
    echo "Building fred"
    echo "-----------------------------------------------------------------------------"
    make distclean &>/dev/null
    if [ "$OPT_PLATFORM" = "linux" ]; then
      if [ $OPT_STATIC_HIVEX -eq 0 ]; then
        $OPT_LINUX_QMAKE || exit 1
      else
        $OPT_LINUX_QMAKE HIVEX_STATIC=1 || exit 1
      fi
      make clean &>/dev/null
      make -j$OPT_JOBS || exit 1
    fi
    if [ "$OPT_PLATFORM" = "win32" ]; then
      if [ $OPT_STATIC_HIVEX -eq 0 ]; then
        $OPT_WIN32_QMAKE || exit 1
      else
        $OPT_WIN32_QMAKE HIVEX_STATIC=1 || exit 1
      fi
      make clean &>/dev/null
      make -j$OPT_JOBS || exit 1
    fi
  fi
)

if [ $? -eq 0 ]; then
  echo "-----------------------------------------------------------------------------"
  echo "All done."
  echo "-----------------------------------------------------------------------------"
else
  echo "-----------------------------------------------------------------------------"
  echo "An error occured while building! See output above for details."
  echo "-----------------------------------------------------------------------------"
fi

