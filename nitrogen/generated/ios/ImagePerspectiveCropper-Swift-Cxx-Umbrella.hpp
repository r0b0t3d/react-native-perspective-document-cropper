///
/// ImagePerspectiveCropper-Swift-Cxx-Umbrella.hpp
/// This file was generated by nitrogen. DO NOT MODIFY THIS FILE.
/// https://github.com/mrousavy/nitro
/// Copyright © 2025 Marc Rousavy @ Margelo
///

#pragma once

// Forward declarations of C++ defined types
// Forward declaration of `HybridImagePerspectiveCropperSpec` to properly resolve imports.
namespace margelo::nitro::customcrop { class HybridImagePerspectiveCropperSpec; }
// Forward declaration of `Point` to properly resolve imports.
namespace margelo::nitro::customcrop { struct Point; }
// Forward declaration of `Rectangle` to properly resolve imports.
namespace margelo::nitro::customcrop { struct Rectangle; }

// Include C++ defined types
#include "HybridImagePerspectiveCropperSpec.hpp"
#include "Point.hpp"
#include "Rectangle.hpp"
#include <NitroModules/Promise.hpp>
#include <NitroModules/Result.hpp>
#include <exception>
#include <memory>
#include <string>

// C++ helpers for Swift
#include "ImagePerspectiveCropper-Swift-Cxx-Bridge.hpp"

// Common C++ types used in Swift
#include <NitroModules/ArrayBufferHolder.hpp>
#include <NitroModules/AnyMapHolder.hpp>
#include <NitroModules/RuntimeError.hpp>

// Forward declarations of Swift defined types
// Forward declaration of `HybridImagePerspectiveCropperSpec_cxx` to properly resolve imports.
namespace ImagePerspectiveCropper { class HybridImagePerspectiveCropperSpec_cxx; }

// Include Swift defined types
#if __has_include("ImagePerspectiveCropper-Swift.h")
// This header is generated by Xcode/Swift on every app build.
// If it cannot be found, make sure the Swift module's name (= podspec name) is actually "ImagePerspectiveCropper".
#include "ImagePerspectiveCropper-Swift.h"
// Same as above, but used when building with frameworks (`use_frameworks`)
#elif __has_include(<ImagePerspectiveCropper/ImagePerspectiveCropper-Swift.h>)
#include <ImagePerspectiveCropper/ImagePerspectiveCropper-Swift.h>
#else
#error ImagePerspectiveCropper's autogenerated Swift header cannot be found! Make sure the Swift module's name (= podspec name) is actually "ImagePerspectiveCropper", and try building the app first.
#endif
