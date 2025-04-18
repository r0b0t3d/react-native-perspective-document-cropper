///
/// HybridImagePerspectiveCropperSpecSwift.hpp
/// This file was generated by nitrogen. DO NOT MODIFY THIS FILE.
/// https://github.com/mrousavy/nitro
/// Copyright © 2025 Marc Rousavy @ Margelo
///

#pragma once

#include "HybridImagePerspectiveCropperSpec.hpp"

// Forward declaration of `HybridImagePerspectiveCropperSpec_cxx` to properly resolve imports.
namespace ImagePerspectiveCropper { class HybridImagePerspectiveCropperSpec_cxx; }

// Forward declaration of `Rectangle` to properly resolve imports.
namespace margelo::nitro::customcrop { struct Rectangle; }
// Forward declaration of `Point` to properly resolve imports.
namespace margelo::nitro::customcrop { struct Point; }

#include <NitroModules/Promise.hpp>
#include "Rectangle.hpp"
#include "Point.hpp"
#include <string>

#include "ImagePerspectiveCropper-Swift-Cxx-Umbrella.hpp"

namespace margelo::nitro::customcrop {

  /**
   * The C++ part of HybridImagePerspectiveCropperSpec_cxx.swift.
   *
   * HybridImagePerspectiveCropperSpecSwift (C++) accesses HybridImagePerspectiveCropperSpec_cxx (Swift), and might
   * contain some additional bridging code for C++ <> Swift interop.
   *
   * Since this obviously introduces an overhead, I hope at some point in
   * the future, HybridImagePerspectiveCropperSpec_cxx can directly inherit from the C++ class HybridImagePerspectiveCropperSpec
   * to simplify the whole structure and memory management.
   */
  class HybridImagePerspectiveCropperSpecSwift: public virtual HybridImagePerspectiveCropperSpec {
  public:
    // Constructor from a Swift instance
    explicit HybridImagePerspectiveCropperSpecSwift(const ImagePerspectiveCropper::HybridImagePerspectiveCropperSpec_cxx& swiftPart):
      HybridObject(HybridImagePerspectiveCropperSpec::TAG),
      _swiftPart(swiftPart) { }

  public:
    // Get the Swift part
    inline ImagePerspectiveCropper::HybridImagePerspectiveCropperSpec_cxx& getSwiftPart() noexcept {
      return _swiftPart;
    }

  public:
    // Get memory pressure
    inline size_t getExternalMemorySize() noexcept override {
      return _swiftPart.getMemorySize();
    }

  public:
    // Properties
    

  public:
    // Methods
    inline std::shared_ptr<Promise<Rectangle>> detectRectangleForImage(const std::string& image) override {
      auto __result = _swiftPart.detectRectangleForImage(image);
      if (__result.hasError()) [[unlikely]] {
        std::rethrow_exception(__result.error());
      }
      auto __value = std::move(__result.value());
      return __value;
    }
    inline std::shared_ptr<Promise<std::string>> cropImage(const std::string& image, const Rectangle& rectangle) override {
      auto __result = _swiftPart.cropImage(image, rectangle);
      if (__result.hasError()) [[unlikely]] {
        std::rethrow_exception(__result.error());
      }
      auto __value = std::move(__result.value());
      return __value;
    }

  private:
    ImagePerspectiveCropper::HybridImagePerspectiveCropperSpec_cxx _swiftPart;
  };

} // namespace margelo::nitro::customcrop
