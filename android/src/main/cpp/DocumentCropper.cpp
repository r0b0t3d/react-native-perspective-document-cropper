#include "DocumentCropper.hpp"

#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace customcrop {

// Ported from HybridImagePerspectiveCropper.kt::_cropImage.
//
// Color note: the original JVM implementation read via Imgcodecs.imread (BGR-ish)
// and wrote via Imgcodecs.imwrite, so it needed cvtColor(BGR2RGB)/cvtColor(RGB2BGR)
// around the warp. This implementation is fed RGBA pixels directly from an Android
// Bitmap (see DocumentCropperJNI.cpp) and returns RGBA pixels the same way, so no
// color-order conversion is needed here — warpPerspective is channel-order agnostic.
cv::Mat cropPerspective(const cv::Mat& image, const QuadCorners& corners) {
  const cv::Point2d& tl = corners.topLeft;
  const cv::Point2d& tr = corners.topRight;
  const cv::Point2d& bl = corners.bottomLeft;
  const cv::Point2d& br = corners.bottomRight;

  double widthA = std::sqrt(std::pow(br.x - bl.x, 2.0) + std::pow(br.y - bl.y, 2.0));
  double widthB = std::sqrt(std::pow(tr.x - tl.x, 2.0) + std::pow(tr.y - tl.y, 2.0));
  double maxWidthD = std::max(widthA, widthB);
  int maxWidth = static_cast<int>(maxWidthD);

  double heightA = std::sqrt(std::pow(tr.x - br.x, 2.0) + std::pow(tr.y - br.y, 2.0));
  double heightB = std::sqrt(std::pow(tl.x - bl.x, 2.0) + std::pow(tl.y - bl.y, 2.0));
  double maxHeightD = std::max(heightA, heightB);
  int maxHeight = static_cast<int>(maxHeightD);

  // Degenerate/near-coincident corners truncate to 0, which would otherwise
  // reach Bitmap.createBitmap(0, ...) in the JNI layer and throw a Java
  // exception there instead of failing cleanly here.
  if (maxWidth <= 0 || maxHeight <= 0) {
    throw std::invalid_argument("cropPerspective: degenerate quad produces zero-size output");
  }

  cv::Mat doc(maxHeight, maxWidth, CV_8UC4);

  cv::Mat srcMat(4, 1, CV_32FC2);
  cv::Mat dstMat(4, 1, CV_32FC2);
  srcMat.at<cv::Point2f>(0) = cv::Point2f(static_cast<float>(tl.x), static_cast<float>(tl.y));
  srcMat.at<cv::Point2f>(1) = cv::Point2f(static_cast<float>(tr.x), static_cast<float>(tr.y));
  srcMat.at<cv::Point2f>(2) = cv::Point2f(static_cast<float>(br.x), static_cast<float>(br.y));
  srcMat.at<cv::Point2f>(3) = cv::Point2f(static_cast<float>(bl.x), static_cast<float>(bl.y));

  dstMat.at<cv::Point2f>(0) = cv::Point2f(0.f, 0.f);
  dstMat.at<cv::Point2f>(1) = cv::Point2f(static_cast<float>(maxWidthD), 0.f);
  dstMat.at<cv::Point2f>(2) = cv::Point2f(static_cast<float>(maxWidthD), static_cast<float>(maxHeightD));
  dstMat.at<cv::Point2f>(3) = cv::Point2f(0.f, static_cast<float>(maxHeightD));

  cv::Mat m = cv::getPerspectiveTransform(srcMat, dstMat);
  cv::warpPerspective(image, doc, m, doc.size());

  return doc;
}

} // namespace customcrop
