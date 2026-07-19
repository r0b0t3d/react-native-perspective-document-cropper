#include "DocumentCropperJNI.hpp"

#include <android/bitmap.h>
#include <opencv2/core.hpp>
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "DocumentCropper.hpp"
#include "DocumentDetector.hpp"

namespace {

// RAII wrapper around AndroidBitmap_lockPixels/unlockPixels so a thrown
// exception can never leave a bitmap locked.
class BitmapPixelLock {
 public:
  BitmapPixelLock(JNIEnv* env, jobject bitmap) : env_(env), bitmap_(bitmap), pixels_(nullptr) {
    if (AndroidBitmap_getInfo(env_, bitmap_, &info_) != ANDROID_BITMAP_RESULT_SUCCESS) {
      throw std::runtime_error("AndroidBitmap_getInfo failed");
    }
    if (info_.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
      throw std::runtime_error("Unsupported bitmap format, expected ARGB_8888");
    }
    if (AndroidBitmap_lockPixels(env_, bitmap_, &pixels_) != ANDROID_BITMAP_RESULT_SUCCESS) {
      throw std::runtime_error("AndroidBitmap_lockPixels failed");
    }
  }

  ~BitmapPixelLock() {
    if (pixels_ != nullptr) {
      AndroidBitmap_unlockPixels(env_, bitmap_);
    }
  }

  BitmapPixelLock(const BitmapPixelLock&) = delete;
  BitmapPixelLock& operator=(const BitmapPixelLock&) = delete;

  const AndroidBitmapInfo& info() const { return info_; }
  void* pixels() const { return pixels_; }

 private:
  JNIEnv* env_;
  jobject bitmap_;
  AndroidBitmapInfo info_;
  void* pixels_;
};

// Wraps (without copying) a locked RGBA_8888 bitmap's pixels as a CV_8UC4 Mat,
// honoring the bitmap's row stride (which may exceed width * 4 due to padding).
cv::Mat wrapBitmapAsMat(const BitmapPixelLock& lock) {
  const AndroidBitmapInfo& info = lock.info();
  return cv::Mat(static_cast<int>(info.height), static_cast<int>(info.width), CV_8UC4,
                  lock.pixels(), info.stride);
}

// Bitmap.createBitmap's class/method/config-field lookups never change across
// calls, so they're resolved once (thread-safe magic-statics init) and cached
// as global refs instead of re-resolved via JNI reflection on every crop.
struct CreateBitmapRefs {
  jclass bitmapClass;
  jmethodID createBitmapMethod;
  jobject argb8888Config;
};

const CreateBitmapRefs& getCreateBitmapRefs(JNIEnv* env) {
  static CreateBitmapRefs refs = [env]() {
    jclass bitmapClass = static_cast<jclass>(
        env->NewGlobalRef(env->FindClass("android/graphics/Bitmap")));
    jclass configClass = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID createBitmapMethod = env->GetStaticMethodID(
        bitmapClass, "createBitmap",
        "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jfieldID argb8888Field =
        env->GetStaticFieldID(configClass, "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
    jobject argb8888Config =
        env->NewGlobalRef(env->GetStaticObjectField(configClass, argb8888Field));
    return CreateBitmapRefs{bitmapClass, createBitmapMethod, argb8888Config};
  }();
  return refs;
}

jobject createArgbBitmap(JNIEnv* env, int width, int height) {
  const CreateBitmapRefs& refs = getCreateBitmapRefs(env);

  jobject bitmap = env->CallStaticObjectMethod(refs.bitmapClass, refs.createBitmapMethod, width,
                                                height, refs.argb8888Config);
  if (env->ExceptionCheck()) {
    // A pending Java exception (e.g. IllegalArgumentException from
    // Bitmap.createBitmap) must be cleared before any further JNI calls are
    // made — including the throwRuntimeException() callers use to report
    // native errors back to Kotlin.
    env->ExceptionClear();
    throw std::runtime_error("Bitmap.createBitmap threw a Java exception");
  }
  if (bitmap == nullptr) {
    throw std::runtime_error("Bitmap.createBitmap returned null");
  }
  return bitmap;
}

// Copies `src` (CV_8UC4, tightly packed) into a locked destination bitmap,
// row by row, to respect the destination's own stride.
void copyMatIntoBitmap(const cv::Mat& src, const BitmapPixelLock& dstLock) {
  const AndroidBitmapInfo& info = dstLock.info();
  auto* dst = static_cast<uint8_t*>(dstLock.pixels());
  size_t rowBytes = static_cast<size_t>(src.cols) * 4;
  if (src.isContinuous() && info.stride == rowBytes) {
    memcpy(dst, src.ptr(0), rowBytes * static_cast<size_t>(src.rows));
    return;
  }
  for (int row = 0; row < src.rows; row++) {
    memcpy(dst + static_cast<size_t>(row) * info.stride, src.ptr(row), rowBytes);
  }
}

void throwRuntimeException(JNIEnv* env, const char* message) {
  jclass exClass = env->FindClass("java/lang/RuntimeException");
  if (exClass != nullptr) {
    env->ThrowNew(exClass, message);
  }
}

} // namespace

extern "C" {

JNIEXPORT jdoubleArray JNICALL
Java_com_margelo_nitro_customcrop_HybridImagePerspectiveCropper_nativeDetectRectangle(
    JNIEnv* env, jobject /*thiz*/, jobject bitmap) {
  try {
    BitmapPixelLock lock(env, bitmap);
    cv::Mat image = wrapBitmapAsMat(lock);

    auto result = customcrop::detectDocumentRectangle(image);
    if (!result.has_value()) {
      return nullptr;
    }

    double values[10] = {
        result->topLeft.x,     result->topLeft.y,     result->topRight.x,
        result->topRight.y,    result->bottomLeft.x,  result->bottomLeft.y,
        result->bottomRight.x, result->bottomRight.y, result->sourceWidth,
        result->sourceHeight,
    };
    jdoubleArray out = env->NewDoubleArray(10);
    env->SetDoubleArrayRegion(out, 0, 10, values);
    return out;
  } catch (const std::exception& e) {
    throwRuntimeException(env, e.what());
    return nullptr;
  } catch (...) {
    throwRuntimeException(env, "Unknown native error in nativeDetectRectangle");
    return nullptr;
  }
}

JNIEXPORT jobject JNICALL
Java_com_margelo_nitro_customcrop_HybridImagePerspectiveCropper_nativeCropImage(
    JNIEnv* env, jobject /*thiz*/, jobject bitmap, jdouble tlX, jdouble tlY, jdouble trX,
    jdouble trY, jdouble blX, jdouble blY, jdouble brX, jdouble brY) {
  try {
    customcrop::QuadCorners corners{
        cv::Point2d(tlX, tlY),
        cv::Point2d(trX, trY),
        cv::Point2d(blX, blY),
        cv::Point2d(brX, brY),
    };

    cv::Mat cropped;
    {
      BitmapPixelLock srcLock(env, bitmap);
      cv::Mat image = wrapBitmapAsMat(srcLock);
      cropped = customcrop::cropPerspective(image, corners);
    }

    jobject outputBitmap = createArgbBitmap(env, cropped.cols, cropped.rows);
    BitmapPixelLock dstLock(env, outputBitmap);
    copyMatIntoBitmap(cropped, dstLock);
    return outputBitmap;
  } catch (const std::exception& e) {
    throwRuntimeException(env, e.what());
    return nullptr;
  } catch (...) {
    throwRuntimeException(env, "Unknown native error in nativeCropImage");
    return nullptr;
  }
}

} // extern "C"
