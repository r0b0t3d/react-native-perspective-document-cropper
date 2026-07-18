#pragma once

#include <jni.h>

extern "C" {

// Returns a 10-element array
// [tlX, tlY, trX, trY, blX, blY, brX, brY, srcWidth, srcHeight],
// or null if no document rectangle was found. Throws RuntimeException on
// native error (e.g. unsupported bitmap format).
JNIEXPORT jdoubleArray JNICALL
Java_com_margelo_nitro_customcrop_HybridImagePerspectiveCropper_nativeDetectRectangle(
    JNIEnv* env, jobject thiz, jobject bitmap);

// Perspective-crops `bitmap` to the quad described by the 8 corner doubles and
// returns a new ARGB_8888 Bitmap. Throws RuntimeException on native error.
JNIEXPORT jobject JNICALL
Java_com_margelo_nitro_customcrop_HybridImagePerspectiveCropper_nativeCropImage(
    JNIEnv* env, jobject thiz, jobject bitmap, jdouble tlX, jdouble tlY, jdouble trX,
    jdouble trY, jdouble blX, jdouble blY, jdouble brX, jdouble brY);

} // extern "C"
