package com.margelo.nitro.customcrop;

import android.util.Log;

import androidx.annotation.Nullable;

import com.facebook.react.bridge.NativeModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.module.model.ReactModuleInfoProvider;
import com.facebook.react.TurboReactPackage;

import org.opencv.android.OpenCVLoader;

import java.util.HashMap;

public class ImagePerspectiveCropperPackage extends TurboReactPackage {
  @Nullable
  @Override
  public NativeModule getModule(String name, ReactApplicationContext reactContext) {
    return null;
  }

  @Override
  public ReactModuleInfoProvider getReactModuleInfoProvider() {
    return () -> {
        return new HashMap<>();
    };
  }

  static {
    System.loadLibrary("ImagePerspectiveCropper");
    if (!OpenCVLoader.initLocal()) {
      Log.e("RNCustomCropModule", "Could not init OpenCV");
    } else {
      Log.d("RNCustomCropModule", "OpenCV initialized!!!");
    }
  }
}
