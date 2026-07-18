#pragma once

#include <opencv2/core.hpp>

namespace customcrop {

struct QuadCorners {
  cv::Point2d topLeft;
  cv::Point2d topRight;
  cv::Point2d bottomLeft;
  cv::Point2d bottomRight;
};

// Perspective-crops `image` (CV_8UC4) to the quadrilateral described by `corners`,
// warping it into an upright rectangle sized to the corners' max width/height.
// Throws std::invalid_argument if corners are degenerate (zero-size output),
// or cv::Exception on other invalid input (e.g. empty image).
cv::Mat cropPerspective(const cv::Mat& image, const QuadCorners& corners);

} // namespace customcrop
