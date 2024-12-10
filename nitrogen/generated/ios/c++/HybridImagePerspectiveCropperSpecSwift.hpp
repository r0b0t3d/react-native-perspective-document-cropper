///
/// HybridImagePerspectiveCropperSpecSwift.hpp
/// This file was generated by nitrogen. DO NOT MODIFY THIS FILE.
/// https://github.com/mrousavy/nitro
/// Copyright © 2024 Marc Rousavy @ Margelo
///

#pragma once

#include "HybridImagePerspectiveCropperSpec.hpp"

// Forward declaration of `HybridImagePerspectiveCropperSpecCxx` to properly resolve imports.
namespace ImagePerspectiveCropper { class HybridImagePerspectiveCropperSpecCxx; }

// Forward declaration of `Rectangle` to properly resolve imports.
namespace margelo::nitro::customcrop { struct Rectangle; }
// Forward declaration of `Point` to properly resolve imports.
namespace margelo::nitro::customcrop { struct Point; }

#include <string>
#include <functional>
#include "Rectangle.hpp"
#include "Point.hpp"

#if __has_include(<NitroModules/HybridContext.hpp>)
#include <NitroModules/HybridContext.hpp>
#else
#error NitroModules cannot be found! Are you sure you installed NitroModules properly?
#endif

#include "ImagePerspectiveCropper-Swift-Cxx-Umbrella.hpp"

namespace margelo::nitro::customcrop {

  /**
   * The C++ part of HybridImagePerspectiveCropperSpecCxx.swift.
   *
   * HybridImagePerspectiveCropperSpecSwift (C++) accesses HybridImagePerspectiveCropperSpecCxx (Swift), and might
   * contain some additional bridging code for C++ <> Swift interop.
   *
   * Since this obviously introduces an overhead, I hope at some point in
   * the future, HybridImagePerspectiveCropperSpecCxx can directly inherit from the C++ class HybridImagePerspectiveCropperSpec
   * to simplify the whole structure and memory management.
   */
  class HybridImagePerspectiveCropperSpecSwift: public virtual HybridImagePerspectiveCropperSpec {
  public:
    // Constructor from a Swift instance
    explicit HybridImagePerspectiveCropperSpecSwift(const ImagePerspectiveCropper::HybridImagePerspectiveCropperSpecCxx& swiftPart):
      HybridObject(HybridImagePerspectiveCropperSpec::TAG),
      _swiftPart(swiftPart) { }

  public:
    // Get the Swift part
    inline ImagePerspectiveCropper::HybridImagePerspectiveCropperSpecCxx getSwiftPart() noexcept { return _swiftPart; }

  public:
    // Get memory pressure
    inline size_t getExternalMemorySize() noexcept override {
      return _swiftPart.getMemorySize();
    }

  public:
    // Properties
    

  public:
    // Methods
    inline void detectRectangleForImage(const std::string& image, const std::function<void(const Rectangle& /* rectangle */)>& onSuccess, const std::function<void(const std::string& /* message */)>& onError) override {
      _swiftPart.detectRectangleForImage(image, onSuccess, onError);
    }
    inline void cropImage(const std::string& image, const Rectangle& rectangle, const std::function<void(const std::string& /* image */)>& onSuccess, const std::function<void(const std::string& /* message */)>& onError) override {
      _swiftPart.cropImage(image, rectangle, onSuccess, onError);
    }

  private:
    ImagePerspectiveCropper::HybridImagePerspectiveCropperSpecCxx _swiftPart;
  };

} // namespace margelo::nitro::customcrop