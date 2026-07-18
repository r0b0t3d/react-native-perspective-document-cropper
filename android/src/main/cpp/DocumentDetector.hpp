#pragma once

#include <opencv2/core.hpp>
#include <optional>

namespace customcrop {

// Corners of a detected document, plus the size of the source image they
// were detected in (matches the `Rectangle` shape in src/specs/ImagePerspectiveCropper.nitro.ts).
struct DetectedRectangle {
  cv::Point2d topLeft;
  cv::Point2d topRight;
  cv::Point2d bottomLeft;
  cv::Point2d bottomRight;
  double sourceWidth;
  double sourceHeight;
};

// Detects the largest document-like quadrilateral contour in `image`.
// `image` must be CV_8UC4 (RGBA). Returns std::nullopt if no contour passes
// the shape/size heuristics (mirrors the old `getBiggestRectangle` returning null).
std::optional<DetectedRectangle> detectDocumentRectangle(const cv::Mat& image);

} // namespace customcrop
