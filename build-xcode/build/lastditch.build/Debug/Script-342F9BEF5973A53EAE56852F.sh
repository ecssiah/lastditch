#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/ecssiah/Development/justsky/lastditch/build-xcode
  /opt/homebrew/bin/cmake -E echo Assets...
  /opt/homebrew/bin/cmake -E copy_directory /Users/ecssiah/Development/justsky/lastditch/assets /Users/ecssiah/Development/justsky/lastditch/build-xcode/Debug/assets
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/ecssiah/Development/justsky/lastditch/build-xcode
  /opt/homebrew/bin/cmake -E echo Assets...
  /opt/homebrew/bin/cmake -E copy_directory /Users/ecssiah/Development/justsky/lastditch/assets /Users/ecssiah/Development/justsky/lastditch/build-xcode/Release/assets
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/ecssiah/Development/justsky/lastditch/build-xcode
  /opt/homebrew/bin/cmake -E echo Assets...
  /opt/homebrew/bin/cmake -E copy_directory /Users/ecssiah/Development/justsky/lastditch/assets /Users/ecssiah/Development/justsky/lastditch/build-xcode/MinSizeRel/assets
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/ecssiah/Development/justsky/lastditch/build-xcode
  /opt/homebrew/bin/cmake -E echo Assets...
  /opt/homebrew/bin/cmake -E copy_directory /Users/ecssiah/Development/justsky/lastditch/assets /Users/ecssiah/Development/justsky/lastditch/build-xcode/RelWithDebInfo/assets
fi

