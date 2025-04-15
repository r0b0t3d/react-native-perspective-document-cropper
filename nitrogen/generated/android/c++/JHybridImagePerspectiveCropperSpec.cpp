///
/// JHybridImagePerspectiveCropperSpec.cpp
/// This file was generated by nitrogen. DO NOT MODIFY THIS FILE.
/// https://github.com/mrousavy/nitro
/// Copyright © 2025 Marc Rousavy @ Margelo
///

#include "JHybridImagePerspectiveCropperSpec.hpp"

// Forward declaration of `Rectangle` to properly resolve imports.
namespace margelo::nitro::customcrop { struct Rectangle; }
// Forward declaration of `Point` to properly resolve imports.
namespace margelo::nitro::customcrop { struct Point; }

#include <NitroModules/Promise.hpp>
#include "Rectangle.hpp"
#include <NitroModules/JPromise.hpp>
#include "JRectangle.hpp"
#include "Point.hpp"
#include "JPoint.hpp"
#include <string>

namespace margelo::nitro::customcrop {

  jni::local_ref<JHybridImagePerspectiveCropperSpec::jhybriddata> JHybridImagePerspectiveCropperSpec::initHybrid(jni::alias_ref<jhybridobject> jThis) {
    return makeCxxInstance(jThis);
  }

  void JHybridImagePerspectiveCropperSpec::registerNatives() {
    registerHybrid({
      makeNativeMethod("initHybrid", JHybridImagePerspectiveCropperSpec::initHybrid),
    });
  }

  size_t JHybridImagePerspectiveCropperSpec::getExternalMemorySize() noexcept {
    static const auto method = javaClassStatic()->getMethod<jlong()>("getMemorySize");
    return method(_javaPart);
  }

  // Properties
  

  // Methods
  std::shared_ptr<Promise<Rectangle>> JHybridImagePerspectiveCropperSpec::detectRectangleForImage(const std::string& image) {
    static const auto method = javaClassStatic()->getMethod<jni::local_ref<JPromise::javaobject>(jni::alias_ref<jni::JString> /* image */)>("detectRectangleForImage");
    auto __result = method(_javaPart, jni::make_jstring(image));
    return [&]() {
      auto __promise = Promise<Rectangle>::create();
      __result->cthis()->addOnResolvedListener([=](const jni::alias_ref<jni::JObject>& __boxedResult) {
        auto __result = jni::static_ref_cast<JRectangle>(__boxedResult);
        __promise->resolve(__result->toCpp());
      });
      __result->cthis()->addOnRejectedListener([=](const jni::alias_ref<jni::JThrowable>& __throwable) {
        jni::JniException __jniError(__throwable);
        __promise->reject(std::make_exception_ptr(__jniError));
      });
      return __promise;
    }();
  }
  std::shared_ptr<Promise<std::string>> JHybridImagePerspectiveCropperSpec::cropImage(const std::string& image, const Rectangle& rectangle) {
    static const auto method = javaClassStatic()->getMethod<jni::local_ref<JPromise::javaobject>(jni::alias_ref<jni::JString> /* image */, jni::alias_ref<JRectangle> /* rectangle */)>("cropImage");
    auto __result = method(_javaPart, jni::make_jstring(image), JRectangle::fromCpp(rectangle));
    return [&]() {
      auto __promise = Promise<std::string>::create();
      __result->cthis()->addOnResolvedListener([=](const jni::alias_ref<jni::JObject>& __boxedResult) {
        auto __result = jni::static_ref_cast<jni::JString>(__boxedResult);
        __promise->resolve(__result->toStdString());
      });
      __result->cthis()->addOnRejectedListener([=](const jni::alias_ref<jni::JThrowable>& __throwable) {
        jni::JniException __jniError(__throwable);
        __promise->reject(std::make_exception_ptr(__jniError));
      });
      return __promise;
    }();
  }

} // namespace margelo::nitro::customcrop
