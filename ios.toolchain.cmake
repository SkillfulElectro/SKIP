# iOS Toolchain
#
# This file is used to configure CMake for cross-compiling for iOS.
#
# Usage:
#   cmake .. -DCMAKE_TOOLCHAIN_FILE=ios.toolchain.cmake -DPLATFORM=OS64
#
# Supported Platforms:
#   OS64: iOS 64-bit
#   SIMULATOR64: iOS 64-bit Simulator
#

set(CMAKE_SYSTEM_NAME iOS)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
  set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum iOS deployment version")
else()
  message(FATAL_ERROR "Building for iOS is only supported on macOS")
endif()

if(PLATFORM STREQUAL "OS64")
  set(CMAKE_OSX_ARCHITECTURES "arm64")
  set(CMAKE_OSX_SYSROOT "iphoneos")
elseif(PLATFORM STREQUAL "SIMULATOR64")
  set(CMAKE_OSX_ARCHITECTURES "x86_64")
  set(CMAKE_OSX_SYSROOT "iphonesimulator")
else()
  message(FATAL_ERROR "Unsupported platform: ${PLATFORM}")
endif()
