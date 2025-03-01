///
/// HybridImagePerspectiveCropperSpec.hpp
/// This file was generated by nitrogen. DO NOT MODIFY THIS FILE.
/// https://github.com/mrousavy/nitro
/// Copyright © 2025 Marc Rousavy @ Margelo
///

#pragma once

#include <NitroModules/JHybridObject.hpp>
#include <fbjni/fbjni.h>
#include "HybridImagePerspectiveCropperSpec.hpp"




namespace margelo::nitro::customcrop {

  using namespace facebook;

  class JHybridImagePerspectiveCropperSpec: public jni::HybridClass<JHybridImagePerspectiveCropperSpec, JHybridObject>,
                                            public virtual HybridImagePerspectiveCropperSpec {
  public:
    static auto constexpr kJavaDescriptor = "Lcom/margelo/nitro/customcrop/HybridImagePerspectiveCropperSpec;";
    static jni::local_ref<jhybriddata> initHybrid(jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

  protected:
    // C++ constructor (called from Java via `initHybrid()`)
    explicit JHybridImagePerspectiveCropperSpec(jni::alias_ref<jhybridobject> jThis) :
      HybridObject(HybridImagePerspectiveCropperSpec::TAG),
      _javaPart(jni::make_global(jThis)) {}

  public:
    ~JHybridImagePerspectiveCropperSpec() override {
      // Hermes GC can destroy JS objects on a non-JNI Thread.
      jni::ThreadScope::WithClassLoader([&] { _javaPart.reset(); });
    }

  public:
    size_t getExternalMemorySize() noexcept override;

  public:
    inline const jni::global_ref<JHybridImagePerspectiveCropperSpec::javaobject>& getJavaPart() const noexcept {
      return _javaPart;
    }

  public:
    // Properties
    

  public:
    // Methods
    void detectRectangleForImage(const std::string& image, const std::function<void(const Rectangle& /* rectangle */)>& onSuccess, const std::function<void(const std::string& /* message */)>& onError) override;
    void cropImage(const std::string& image, const Rectangle& rectangle, const std::function<void(const std::string& /* image */)>& onSuccess, const std::function<void(const std::string& /* message */)>& onError) override;

  private:
    friend HybridBase;
    using HybridBase::HybridBase;
    jni::global_ref<JHybridImagePerspectiveCropperSpec::javaobject> _javaPart;
  };

} // namespace margelo::nitro::customcrop
