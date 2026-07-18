project(ImagePerspectiveCropper)
cmake_minimum_required(VERSION 3.9.0)

set (PACKAGE_NAME ImagePerspectiveCropper)
set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 20)

# opencv-mobile: vendored prebuilt SDK (core + imgproc only, no Java bindings).
# find_package resolves per-ABI via ${ANDROID_NDK_ABI_NAME}, see
# android/opencv-mobile/sdk/native/jni/OpenCVConfig.cmake.
set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/opencv-mobile/sdk/native/jni")
find_package(OpenCV REQUIRED core imgproc)

# Define C++ library and add all sources
add_library(${PACKAGE_NAME} SHARED
        src/main/cpp/cpp-adapter.cpp
        src/main/cpp/DocumentDetector.cpp
        src/main/cpp/DocumentCropper.cpp
        src/main/cpp/DocumentCropperJNI.cpp
)

# Add Nitrogen specs :)
include(${CMAKE_SOURCE_DIR}/../nitrogen/generated/android/ImagePerspectiveCropper+autolinking.cmake)

# Set up local includes
include_directories(
        "src/main/cpp"
        "../cpp"
        ${OpenCV_INCLUDE_DIRS}
)

find_library(LOG_LIB log)
find_library(JNIGRAPHICS_LIB jnigraphics)

# Link all libraries together
target_link_libraries(
        ${PACKAGE_NAME}
        ${LOG_LIB}
        ${JNIGRAPHICS_LIB}                        # <-- AndroidBitmap_lockPixels/unlockPixels
        android                                   # <-- Android core
        ${OpenCV_LIBS}                            # <-- opencv_core, opencv_imgproc (+ transitive kleidicv/openmp on arm64-v8a)
)
