///
/// ImagePerspectiveCropper-Swift-Cxx-Bridge.cpp
/// This file was generated by nitrogen. DO NOT MODIFY THIS FILE.
/// https://github.com/mrousavy/nitro
/// Copyright © 2025 Marc Rousavy @ Margelo
///

#include "ImagePerspectiveCropper-Swift-Cxx-Bridge.hpp"

// Include C++ implementation defined types
#include "HybridImagePerspectiveCropperSpecSwift.hpp"
#include "ImagePerspectiveCropper-Swift-Cxx-Umbrella.hpp"

namespace margelo::nitro::customcrop::bridge::swift {

  // pragma MARK: std::function<void(const Rectangle& /* rectangle */)>
  Func_void_Rectangle create_Func_void_Rectangle(void* _Nonnull swiftClosureWrapper) {
    auto swiftClosure = ImagePerspectiveCropper::Func_void_Rectangle::fromUnsafe(swiftClosureWrapper);
    return [swiftClosure = std::move(swiftClosure)](const Rectangle& rectangle) mutable -> void {
      swiftClosure.call(rectangle);
    };
  }
  
  // pragma MARK: std::function<void(const std::string& /* message */)>
  Func_void_std__string create_Func_void_std__string(void* _Nonnull swiftClosureWrapper) {
    auto swiftClosure = ImagePerspectiveCropper::Func_void_std__string::fromUnsafe(swiftClosureWrapper);
    return [swiftClosure = std::move(swiftClosure)](const std::string& message) mutable -> void {
      swiftClosure.call(message);
    };
  }
  
  // pragma MARK: std::shared_ptr<margelo::nitro::customcrop::HybridImagePerspectiveCropperSpec>
  std::shared_ptr<margelo::nitro::customcrop::HybridImagePerspectiveCropperSpec> create_std__shared_ptr_margelo__nitro__customcrop__HybridImagePerspectiveCropperSpec_(void* _Nonnull swiftUnsafePointer) {
    ImagePerspectiveCropper::HybridImagePerspectiveCropperSpec_cxx swiftPart = ImagePerspectiveCropper::HybridImagePerspectiveCropperSpec_cxx::fromUnsafe(swiftUnsafePointer);
    return std::make_shared<margelo::nitro::customcrop::HybridImagePerspectiveCropperSpecSwift>(swiftPart);
  }
  void* _Nonnull get_std__shared_ptr_margelo__nitro__customcrop__HybridImagePerspectiveCropperSpec_(std__shared_ptr_margelo__nitro__customcrop__HybridImagePerspectiveCropperSpec_ cppType) {
    std::shared_ptr<margelo::nitro::customcrop::HybridImagePerspectiveCropperSpecSwift> swiftWrapper = std::dynamic_pointer_cast<margelo::nitro::customcrop::HybridImagePerspectiveCropperSpecSwift>(cppType);
  #ifdef NITRO_DEBUG
    if (swiftWrapper == nullptr) [[unlikely]] {
      throw std::runtime_error("Class \"HybridImagePerspectiveCropperSpec\" is not implemented in Swift!");
    }
  #endif
    ImagePerspectiveCropper::HybridImagePerspectiveCropperSpec_cxx& swiftPart = swiftWrapper->getSwiftPart();
    return swiftPart.toUnsafe();
  }

} // namespace margelo::nitro::customcrop::bridge::swift
