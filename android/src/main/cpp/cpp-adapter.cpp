#include <jni.h>
#include "ImagePerspectiveCropperOnLoad.hpp"

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
  return facebook::jni::initialize(vm, []() {
    margelo::nitro::customcrop::registerAllNatives();
  });
}
