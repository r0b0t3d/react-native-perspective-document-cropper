package com.margelo.nitro.customcrop

import android.media.ExifInterface
import android.util.Log
import com.margelo.nitro.NitroModules
import org.opencv.core.Core
import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.core.MatOfPoint
import org.opencv.core.MatOfPoint2f
import org.opencv.core.Size
import org.opencv.imgcodecs.Imgcodecs
import org.opencv.imgproc.Imgproc
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.util.Arrays
import java.util.Collections
import java.util.concurrent.Executors
import kotlin.math.max
import kotlin.math.pow
import kotlin.math.sqrt


class HybridImagePerspectiveCropper():
    HybridImagePerspectiveCropperSpec() {

    private val applicationContext = NitroModules.applicationContext

    override fun detectRectangleForImage(
        image: String,
        onSuccess: (rectangle: Rectangle) -> Unit,
        onError: (message: String) -> Unit
    ) {
        Executors.newSingleThreadExecutor().execute {
            val rectangle = detectRectangleInImage(image.replace("file://", ""))
            if (rectangle != null) {
                onSuccess(rectangle)
            } else {
                onError("Not found")
            }
        }
    }

    override fun cropImage(
        image: String,
        rectangle: Rectangle,
        onSuccess: (image: String) -> Unit,
        onError: (message: String) -> Unit
    ) {
        val tl = Point(rectangle.topLeft.x, rectangle.topLeft.y)
        val tr = Point(rectangle.topRight.x, rectangle.topRight.y)
        val bl = Point(rectangle.bottomLeft.x, rectangle.bottomLeft.y)
        val br = Point(rectangle.bottomRight.x, rectangle.bottomRight.y)

        val imageFile = image.replace("file://", "")
        val src = Imgcodecs.imread(imageFile, Imgproc.COLOR_BGR2RGB)
        Imgproc.cvtColor(src, src, Imgproc.COLOR_BGR2RGB)

        // FIXME: This ratio introduce bug on my end
        // When the crop rectangle is small,
        // ratioAlreadyApplied will be true and ratio is smaller than 1. So the crop area is not correct
        /* boolean ratioAlreadyApplied = tr.x * (src.size().width / 500) < src.size().width;
    double ratio = ratioAlreadyApplied ? src.size().width / 500 : 1; */
        val ratio = 1.0

        val widthA = sqrt((br.x - bl.x).pow(2.0) + (br.y - bl.y).pow(2.0))
        val widthB = sqrt((tr.x - tl.x).pow(2.0) + (tr.y - tl.y).pow(2.0))

        val dw = max(widthA, widthB) * ratio
        val maxWidth = dw.toInt()

        val heightA = sqrt((tr.x - br.x).pow(2.0) + (tr.y - br.y).pow(2.0))
        val heightB = sqrt((tl.x - bl.x).pow(2.0) + (tl.y - bl.y).pow(2.0))

        val dh = max(heightA, heightB) * ratio
        val maxHeight = dh.toInt()

        val doc = Mat(maxHeight, maxWidth, CvType.CV_8UC4)

        val src_mat = Mat(4, 1, CvType.CV_32FC2)
        val dst_mat = Mat(4, 1, CvType.CV_32FC2)

        src_mat.put(
            0,
            0,
            tl.x * ratio,
            tl.y * ratio,
            tr.x * ratio,
            tr.y * ratio,
            br.x * ratio,
            br.y * ratio,
            bl.x * ratio,
            bl.y * ratio
        )
        dst_mat.put(0, 0, 0.0, 0.0, dw, 0.0, dw, dh, 0.0, dh)

        val m = Imgproc.getPerspectiveTransform(src_mat, dst_mat)

        Imgproc.warpPerspective(src, doc, m, doc.size())

        val orientation = getImageOrientation(imageFile)
        //
//    Bitmap bitmap = Bitmap.createBitmap(doc.cols(), doc.rows(), Bitmap.Config.ARGB_8888);
//    Utils.matToBitmap(doc, bitmap);
//
//    Matrix matrix = new Matrix();
//    matrix.postRotate(90);
//    Bitmap rotatedBitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, false);
//
//    ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
//    rotatedBitmap.compress(Bitmap.CompressFormat.JPEG, 70, byteArrayOutputStream);
        val file = File(applicationContext!!.cacheDir, System.currentTimeMillis().toString() + ".jpg")
        Imgproc.cvtColor(doc, doc, Imgproc.COLOR_RGB2BGR)
        Imgcodecs.imwrite(file.path, doc)
        //    saveToFile(file, byteArrayOutputStream);
        onSuccess(file.absolutePath)

        src.release()
        m.release()
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

    private fun getImageOrientation(imgPath: String): Int {
        var rotate = 0
        try {
            val exif = ExifInterface(File(imgPath).absolutePath)
            val orientation = exif.getAttributeInt(
                ExifInterface.TAG_ORIENTATION,
                ExifInterface.ORIENTATION_NORMAL
            )
            when (orientation) {
                ExifInterface.ORIENTATION_ROTATE_270 -> rotate = 270
                ExifInterface.ORIENTATION_ROTATE_180 -> rotate = 180
                ExifInterface.ORIENTATION_ROTATE_90 -> rotate = 90
            }
            Log.i("RotateImage", "Exif orientation: $orientation")
            Log.i("RotateImage", "Rotate value: $rotate")
        } catch (e: IOException) {
            e.printStackTrace()
        }
        return rotate
    }

    fun detectRectangleInImage(filePath: String): Rectangle? {
        Log.d(TAG, "detectRectangleInImage $filePath")
        val inputRgba = Imgcodecs.imread(filePath)
        val contours = findContours(inputRgba)
        val srcSize = inputRgba.size()
        val result = getBiggestRectangle(contours, srcSize)
        inputRgba.release()
        return result
    }

    private fun findContours(src: Mat): ArrayList<MatOfPoint> {
        val grayImage: Mat
        val cannedImage: Mat
        val resizedImage: Mat

        val height = src.size().height.toInt()
        val width = src.size().width.toInt()
        val size = Size(width.toDouble(), height.toDouble())

        resizedImage = Mat(size, CvType.CV_8UC4)
        grayImage = Mat(size, CvType.CV_8UC4)
        cannedImage = Mat(size, CvType.CV_8UC1)

        Imgproc.resize(src, resizedImage, size)
        Imgproc.cvtColor(resizedImage, grayImage, Imgproc.COLOR_RGBA2GRAY, 4)
        Imgproc.GaussianBlur(grayImage, grayImage, Size(5.0, 5.0), 0.0)
        Imgproc.Canny(grayImage, cannedImage, 80.0, 100.0, 3, false)

        val contours = ArrayList<MatOfPoint>()
        val hierarchy = Mat()

        Imgproc.findContours(
            cannedImage,
            contours,
            hierarchy,
            Imgproc.RETR_TREE,
            Imgproc.CHAIN_APPROX_SIMPLE
        )

        hierarchy.release()

        Collections.sort(contours, object : Comparator<MatOfPoint?> {
            override fun compare(lhs: MatOfPoint?, rhs: MatOfPoint?): Int {
                return java.lang.Double.compare(Imgproc.contourArea(rhs), Imgproc.contourArea(lhs))
            }
        })

        resizedImage.release()
        grayImage.release()
        cannedImage.release()

        return contours
    }

    private fun getBiggestRectangle(contours: ArrayList<MatOfPoint>, srcSize: Size): Rectangle? {
        val height = srcSize.height.toInt()
        val width = srcSize.width.toInt()
        val size = Size(width.toDouble(), height.toDouble())

        for (c in contours) {
            val c2f = MatOfPoint2f(*c.toArray())
            val peri = Imgproc.arcLength(c2f, true)
            val approx = MatOfPoint2f()
            Imgproc.approxPolyDP(c2f, approx, 0.02 * peri, true)

            val points = approx.toArray()

            // select biggest 4 angles polygon
            // if (points.length == 4) {
            val foundPoints = sortPoints(points)
            if (insideArea(foundPoints, size)) {
                return Rectangle(
                    Point(foundPoints[0]!!.x, foundPoints[0]!!.y),
                    Point(foundPoints[1]!!.x, foundPoints[1]!!.y),
                    Point(foundPoints[3]!!.x, foundPoints[3]!!.y),
                    Point(foundPoints[2]!!.x, foundPoints[2]!!.y),
                    srcSize.width,
                    srcSize.height)
            }
            // }
        }

        return null
    }

    private fun sortPoints(src: Array<org.opencv.core.Point>): Array<org.opencv.core.Point?> {
        val srcPoints = ArrayList(Arrays.asList(*src))

        val result = arrayOf<org.opencv.core.Point?>(null, null, null, null)

        val sumComparator: Comparator<org.opencv.core.Point?> = object : Comparator<org.opencv.core.Point?> {
            override fun compare(lhs: org.opencv.core.Point?, rhs: org.opencv.core.Point?): Int {
                if (lhs != null && rhs != null) {
                    return java.lang.Double.compare(lhs.y + lhs.x, rhs.y + rhs.x)
                }
                return 0
            }
        }

        val diffComparator: Comparator<org.opencv.core.Point?> = object : Comparator<org.opencv.core.Point?> {
            override fun compare(lhs: org.opencv.core.Point?, rhs: org.opencv.core.Point?): Int {
                if (lhs != null && rhs != null) {
                    return java.lang.Double.compare(lhs.y - lhs.x, rhs.y - rhs.x)
                }
                return 0
            }
        }

        // top-left corner = minimal sum
        result[0] = Collections.min(srcPoints, sumComparator)

        // bottom-right corner = maximal sum
        result[2] = Collections.max(srcPoints, sumComparator)

        // top-right corner = minimal difference
        result[1] = Collections.min(srcPoints, diffComparator)

        // bottom-left corner = maximal difference
        result[3] = Collections.max(srcPoints, diffComparator)

        return result
    }

    private fun insideArea(rp: Array<org.opencv.core.Point?>, size: Size): Boolean {
        val width = size.width.toInt()
        val height = size.height.toInt()

        val minimumSize = width / 10

        val isANormalShape =
            rp[0]!!.x != rp[1]!!.x && rp[1]!!.y != rp[0]!!.y && rp[2]!!.y != rp[3]!!.y && rp[3]!!.x != rp[2]!!.x
        val isBigEnough =
            ((rp[1]!!.x - rp[0]!!.x >= minimumSize) && (rp[2]!!.x - rp[3]!!.x >= minimumSize)
                    && (rp[3]!!.y - rp[0]!!.y >= minimumSize) && (rp[2]!!.y - rp[1]!!.y >= minimumSize))

        val leftOffset = rp[0]!!.x - rp[3]!!.x
        val rightOffset = rp[1]!!.x - rp[2]!!.x
        val bottomOffset = rp[0]!!.y - rp[1]!!.y
        val topOffset = rp[2]!!.y - rp[3]!!.y

        val isAnActualRectangle = ((leftOffset <= minimumSize && leftOffset >= -minimumSize)
                && (rightOffset <= minimumSize && rightOffset >= -minimumSize)
                && (bottomOffset <= minimumSize && bottomOffset >= -minimumSize)
                && (topOffset <= minimumSize && topOffset >= -minimumSize))

        return isANormalShape && isAnActualRectangle && isBigEnough
    }

    private fun rotateImagePortrait(image: Mat) {
        Core.flip(image.t(), image, 1)
    }

    companion object {
        const val TAG = "HybridImagePerspectiveCropper"
    }
}
