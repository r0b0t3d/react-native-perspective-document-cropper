///
/// HybridImagePerspectiveCropperSpec.hpp
/// This file was generated by nitrogen. DO NOT MODIFY THIS FILE.
/// https://github.com/mrousavy/nitro
/// Copyright © 2025 Marc Rousavy @ Margelo
///

#pragma once

#if __has_include(<NitroModules/HybridObject.hpp>)
#include <NitroModules/HybridObject.hpp>
#else
#error NitroModules cannot be found! Are you sure you installed NitroModules properly?
#endif

// Forward declaration of `Rectangle` to properly resolve imports.
namespace margelo::nitro::customcrop { struct Rectangle; }

#include <NitroModules/Promise.hpp>
#include "Rectangle.hpp"
#include <string>

namespace margelo::nitro::customcrop {

  using namespace margelo::nitro;

  /**
   * An abstract base class for `ImagePerspectiveCropper`
   * Inherit this class to create instances of `HybridImagePerspectiveCropperSpec` in C++.
   * You must explicitly call `HybridObject`'s constructor yourself, because it is virtual.
   * @example
   * ```cpp
   * class HybridImagePerspectiveCropper: public HybridImagePerspectiveCropperSpec {
   * public:
   *   HybridImagePerspectiveCropper(...): HybridObject(TAG) { ... }
   *   // ...
   * };
   * ```
   */
  class HybridImagePerspectiveCropperSpec: public virtual HybridObject {
    public:
      // Constructor
      explicit HybridImagePerspectiveCropperSpec(): HybridObject(TAG) { }

      // Destructor
      ~HybridImagePerspectiveCropperSpec() override = default;

    public:
      // Properties
      

    public:
      // Methods
      virtual std::shared_ptr<Promise<Rectangle>> detectRectangleForImage(const std::string& image) = 0;
      virtual std::shared_ptr<Promise<std::string>> cropImage(const std::string& image, const Rectangle& rectangle) = 0;

    protected:
      // Hybrid Setup
      void loadHybridMethods() override;

    protected:
      // Tag for logging
      static constexpr auto TAG = "ImagePerspectiveCropper";
  };

} // namespace margelo::nitro::customcrop
