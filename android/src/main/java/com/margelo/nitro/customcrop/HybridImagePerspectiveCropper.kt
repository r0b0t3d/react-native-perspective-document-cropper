package com.margelo.nitro.customcrop

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.util.Log
import com.margelo.nitro.NitroModules
import com.margelo.nitro.core.Promise
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.FileOutputStream
import java.io.IOException

// JPEG quality used when writing the cropped output. 95 matches OpenCV's
// imgcodecs default JPEG quality, which the previous implementation relied on
// implicitly via Imgcodecs.imwrite (no explicit IMWRITE_JPEG_QUALITY param).
private const val OUTPUT_JPEG_QUALITY = 95

class HybridImagePerspectiveCropper():
    HybridImagePerspectiveCropperSpec() {

    private val applicationContext = NitroModules.applicationContext

    // Native methods backed by DocumentCropperJNI.cpp. Primitive/Bitmap
    // marshalling only — no Rectangle/Point objects cross the JNI boundary.
    private external fun nativeDetectRectangle(bitmap: Bitmap): DoubleArray?

    private external fun nativeCropImage(
        bitmap: Bitmap,
        tlX: Double, tlY: Double,
        trX: Double, trY: Double,
        blX: Double, blY: Double,
        brX: Double, brY: Double
    ): Bitmap

    override fun detectRectangleForImage(image: String): Promise<Rectangle> {
        return Promise.parallel {
            detectRectangleInImage(image.replace("file://", ""))
        }
    }

    override fun cropImage(image: String, rectangle: Rectangle): Promise<String> {
        return Promise.parallel {
            _cropImage(image, rectangle)
        }
    }

    private fun _cropImage(
        image: String,
        rectangle: Rectangle
    ): String {
        val imageFile = image.replace("file://", "")
        val bitmap = decodeBitmap(imageFile)

        val cropped = try {
            nativeCropImage(
                bitmap,
                rectangle.topLeft.x, rectangle.topLeft.y,
                rectangle.topRight.x, rectangle.topRight.y,
                rectangle.bottomLeft.x, rectangle.bottomLeft.y,
                rectangle.bottomRight.x, rectangle.bottomRight.y
            )
        } finally {
            bitmap.recycle()
        }

        val file = File(applicationContext!!.cacheDir, System.currentTimeMillis().toString() + ".jpg")
        try {
            FileOutputStream(file).use { fos ->
                cropped.compress(Bitmap.CompressFormat.JPEG, OUTPUT_JPEG_QUALITY, fos)
            }
        } finally {
            cropped.recycle()
        }

        return file.absolutePath
    }

    override val memorySize: Long
        get() = 0

    private fun saveToFile(file: File, baos: ByteArrayOutputStream) {
        var fos: FileOutputStream? = null
        try {
            fos = FileOutputStream(file)
            baos.writeTo(fos)
        } catch (ioe: IOException) {
            // Handle exception here
            ioe.printStackTrace()
        } finally {
            try {
                fos!!.close()
                baos.close()
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }
    }

    private fun detectRectangleInImage(filePath: String): Rectangle {
        Log.d(TAG, "detectRectangleInImage $filePath")
        val bitmap = decodeBitmap(filePath)
        val result = try {
            nativeDetectRectangle(bitmap)
        } finally {
            bitmap.recycle()
        }
        if (result != null) {
            return Rectangle(
                Point(result[0], result[1]),
                Point(result[2], result[3]),
                Point(result[4], result[5]),
                Point(result[6], result[7]),
                result[8],
                result[9]
            )
        } else {
            throw Exception("No rect")
        }
    }

    // opencv-mobile has no imgcodecs module (no cv::imread/imwrite), so image
    // decode happens here via BitmapFactory and the pixel buffer is passed to
    // native as an ARGB_8888 Bitmap (see DocumentCropperJNI.cpp).
    private fun decodeBitmap(path: String): Bitmap {
        val options = BitmapFactory.Options().apply {
            inPreferredConfig = Bitmap.Config.ARGB_8888
        }
        return BitmapFactory.decodeFile(path, options)
            ?: throw Exception("Unable to decode image at $path")
    }

    companion object {
        const val TAG = "HybridImagePerspectiveCropper"
    }
}
