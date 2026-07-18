#include "DocumentDetector.hpp"

#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <vector>

namespace customcrop {

namespace {

// Ported from HybridImagePerspectiveCropper.kt::findContours.
std::vector<std::vector<cv::Point>> findDocumentContours(const cv::Mat& src) {
  cv::Mat grayImage;
  cv::Mat cannedImage;

  // Original Kotlin resized to the same size it read (a no-op 1:1 scale) before
  // converting to grayscale. That resize allocated and copied the full-resolution
  // image for no geometric effect, so it's dropped here — cvtColor reads `src` directly.
  cv::cvtColor(src, grayImage, cv::COLOR_RGBA2GRAY, 4);
  cv::GaussianBlur(grayImage, grayImage, cv::Size(5, 5), 0);
  cv::Canny(grayImage, cannedImage, 80, 100, 3, false);

  std::vector<std::vector<cv::Point>> contours;
  cv::Mat hierarchy;
  cv::findContours(cannedImage, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

  std::sort(contours.begin(), contours.end(),
            [](const std::vector<cv::Point>& lhs, const std::vector<cv::Point>& rhs) {
              return cv::contourArea(lhs) > cv::contourArea(rhs);
            });

  return contours;
}

// Ported from HybridImagePerspectiveCropper.kt::sortPoints.
// [0]=top-left (min x+y), [1]=top-right (min y-x), [2]=bottom-right (max x+y), [3]=bottom-left (max y-x)
std::array<cv::Point2f, 4> sortPoints(const std::vector<cv::Point2f>& points) {
  auto bySum = [](const cv::Point2f& a, const cv::Point2f& b) {
    return (a.x + a.y) < (b.x + b.y);
  };
  auto byDiff = [](const cv::Point2f& a, const cv::Point2f& b) {
    return (a.y - a.x) < (b.y - b.x);
  };

  std::array<cv::Point2f, 4> result{};
  result[0] = *std::min_element(points.begin(), points.end(), bySum);
  result[2] = *std::max_element(points.begin(), points.end(), bySum);
  result[1] = *std::min_element(points.begin(), points.end(), byDiff);
  result[3] = *std::max_element(points.begin(), points.end(), byDiff);
  return result;
}

// Ported from HybridImagePerspectiveCropper.kt::insideArea.
bool insideArea(const std::array<cv::Point2f, 4>& rp, const cv::Size& size) {
  // Integer division, matching the original Kotlin `width / 10` (Int / Int) exactly.
  double minimumSize = size.width / 10;

  bool isANormalShape =
      rp[0].x != rp[1].x && rp[1].y != rp[0].y && rp[2].y != rp[3].y && rp[3].x != rp[2].x;
  bool isBigEnough =
      (rp[1].x - rp[0].x >= minimumSize) && (rp[2].x - rp[3].x >= minimumSize) &&
      (rp[3].y - rp[0].y >= minimumSize) && (rp[2].y - rp[1].y >= minimumSize);

  double leftOffset = rp[0].x - rp[3].x;
  double rightOffset = rp[1].x - rp[2].x;
  double bottomOffset = rp[0].y - rp[1].y;
  double topOffset = rp[2].y - rp[3].y;

  bool isAnActualRectangle =
      (leftOffset <= minimumSize && leftOffset >= -minimumSize) &&
      (rightOffset <= minimumSize && rightOffset >= -minimumSize) &&
      (bottomOffset <= minimumSize && bottomOffset >= -minimumSize) &&
      (topOffset <= minimumSize && topOffset >= -minimumSize);

  return isANormalShape && isAnActualRectangle && isBigEnough;
}

} // namespace

std::optional<DetectedRectangle> detectDocumentRectangle(const cv::Mat& image) {
  cv::Size srcSize(image.cols, image.rows);
  auto contours = findDocumentContours(image);

  for (const auto& contour : contours) {
    std::vector<cv::Point2f> contourF(contour.begin(), contour.end());
    double peri = cv::arcLength(contourF, true);

    std::vector<cv::Point2f> approx;
    cv::approxPolyDP(contourF, approx, 0.02 * peri, true);
    if (approx.empty()) {
      continue;
    }

    // NOTE: original Kotlin has the `points.length == 4` vertex-count filter
    // commented out (dead code) — any polygon reaches sortPoints/insideArea
    // below. Preserved as-is for behavior parity.
    auto foundPoints = sortPoints(approx);
    if (insideArea(foundPoints, srcSize)) {
      DetectedRectangle rect;
      rect.topLeft = foundPoints[0];
      rect.topRight = foundPoints[1];
      rect.bottomLeft = foundPoints[3];
      rect.bottomRight = foundPoints[2];
      rect.sourceWidth = srcSize.width;
      rect.sourceHeight = srcSize.height;
      return rect;
    }
  }

  return std::nullopt;
}

} // namespace customcrop
