#include <jni.h>
#include "ImagePerspectiveCropperOnLoad.hpp"

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
  return margelo::nitro::customcrop::initialize(vm);
}
