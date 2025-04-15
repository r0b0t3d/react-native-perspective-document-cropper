import Foundation
import UIKit
import CoreImage
import NitroModules

enum CropperError: Error {
  case imageNotFound
  case imageProcessedFailed
  case noRectangleFound
}

class HybridImagePerspectiveCropper: HybridImagePerspectiveCropperSpec {
  private let rectangleDetectionQueue = DispatchQueue(label: "RectangleDetectionQueue")
  
  func detectRectangleForImage(image: String) -> NitroModules.Promise<Rectangle> {
    return Promise.parallel(rectangleDetectionQueue) {
      guard let ciImage = self.prepareImage(image: image) else {
        throw CropperError.imageNotFound
      }
      
      return try self.detectRectangle(from: ciImage)
    }
  }
  
  func cropImage(image: String, rectangle: Rectangle) throws -> NitroModules.Promise<String> {
    return Promise.parallel(rectangleDetectionQueue) {
      guard var ciImage = self.prepareImage(image: image) else {
        throw CropperError.imageNotFound
      }
      // CoreImage is working with cartesian coordinates, basically y:0 is in the bottom left corner
      let newTopLeft = self.cartesianForPoint(rectangle.topLeft, height: rectangle.height)
      let newTopRight = self.cartesianForPoint(rectangle.topRight, height: rectangle.height)
      let newBottomLeft = self.cartesianForPoint(rectangle.bottomLeft, height: rectangle.height)
      let newBottomRight = self.cartesianForPoint(rectangle.bottomRight, height: rectangle.height)
      let rectangleCoordinates: [String: Any] = [
        "inputTopLeft": CIVector(cgPoint: newTopLeft),
        "inputTopRight": CIVector(cgPoint: newTopRight),
        "inputBottomLeft": CIVector(cgPoint: newBottomLeft),
        "inputBottomRight": CIVector(cgPoint: newBottomRight)
      ]
        
      ciImage = ciImage.applyingFilter("CIPerspectiveCorrection", parameters: rectangleCoordinates)
      let context = CIContext()
      guard let correctedCGImage = context.createCGImage(ciImage, from: ciImage.extent) else {
        throw CropperError.imageProcessedFailed
      }
      let correctedUIImage = UIImage(cgImage: correctedCGImage)
      
      // Encode image as JPEG
      guard let imageData = correctedUIImage.jpegData(compressionQuality: 0.8) else {
        throw CropperError.imageProcessedFailed
      }
      let uuid = UUID().uuidString
      let paths = FileManager.default.urls(for: .cachesDirectory, in: .userDomainMask)
      guard let documentsDirectory = paths.first else {
        throw CropperError.imageProcessedFailed
      }
      let dataPath = documentsDirectory.appendingPathComponent("\(uuid).jpg")
      
      try imageData.write(to: dataPath)
      return dataPath.path
    }
  }
  
  private func prepareImage(image: String) -> CIImage? {
    let parsedImageUri = image.replacingOccurrences(of: "file://", with: "")
    let fileURL = URL(fileURLWithPath: parsedImageUri)
    guard let imageData = try? Data(contentsOf: fileURL), let uiImage = UIImage(data: imageData) else {
        return nil
    }
    
    return CIImage(cgImage: self.fixOrientation(uiImage).cgImage!)
  }

  private func detectRectangle(from ciImage: CIImage) throws -> Rectangle {
    let context = CIContext(options: nil)
    let cgDetectionImage = context.createCGImage(ciImage, from: ciImage.extent)
    let detectionImage = CIImage(cgImage: cgDetectionImage!)
    
    let detector = CIDetector(ofType: CIDetectorTypeRectangle, context: nil, options: [CIDetectorAccuracy: CIDetectorAccuracyHigh])
    let features = detector?.features(in: detectionImage) as? [CIRectangleFeature]
    let rectangleFeature = self.biggestRectangle(in: features)
    
    if let rectangleFeature = rectangleFeature {
        let rectangleCoordinates = self.computeRectangle(rectangleFeature, for: detectionImage)
        return rectangleCoordinates
    } else {
      throw CropperError.noRectangleFound
    }
  }

  private func biggestRectangle(in rectangles: [CIRectangleFeature]?) -> CIRectangleFeature? {
    guard let rectangles = rectangles, !rectangles.isEmpty else { return nil }
    return rectangles.max { first, second in
        let firstPerimeter = hypot(first.topLeft.x - first.topRight.x, first.topLeft.y - first.topRight.y) +
                                hypot(first.topLeft.x - first.bottomLeft.x, first.topLeft.y - first.bottomLeft.y)
        let secondPerimeter = hypot(second.topLeft.x - second.topRight.x, second.topLeft.y - second.topRight.y) +
                                hypot(second.topLeft.x - second.bottomLeft.x, second.topLeft.y - second.bottomLeft.y)
        return firstPerimeter < secondPerimeter
    }
  }

  private func computeRectangle(_ rectangle: CIRectangleFeature, for image: CIImage) -> Rectangle {
    let imageBounds = image.extent
    return Rectangle(
      topLeft: Point(x: rectangle.topLeft.x, y: imageBounds.height - rectangle.topLeft.y),
      topRight:  Point(x: rectangle.topRight.x, y: imageBounds.height - rectangle.topRight.y),
      bottomLeft: Point(x: rectangle.bottomLeft.x, y: imageBounds.height - rectangle.bottomLeft.y),
      bottomRight:Point(x: rectangle.bottomRight.x, y: imageBounds.height - rectangle.bottomRight.y),
      width: imageBounds.width,
      height: imageBounds.height
    )
  }

  private func fixOrientation(_ image: UIImage) -> UIImage {
    if image.imageOrientation == .up {
        return image
    }
    
    var transform = CGAffineTransform.identity
    
    switch image.imageOrientation {
    case .down, .downMirrored:
        transform = transform.translatedBy(x: image.size.width, y: image.size.height).rotated(by: .pi)
    case .left, .leftMirrored:
        transform = transform.translatedBy(x: image.size.width, y: 0).rotated(by: .pi / 2)
    case .right, .rightMirrored:
        transform = transform.translatedBy(x: 0, y: image.size.height).rotated(by: -.pi / 2)
    default:
        break
    }
    
    switch image.imageOrientation {
    case .upMirrored, .downMirrored:
        transform = transform.translatedBy(x: image.size.width, y: 0).scaledBy(x: -1, y: 1)
    case .leftMirrored, .rightMirrored:
        transform = transform.translatedBy(x: image.size.height, y: 0).scaledBy(x: -1, y: 1)
    default:
        break
    }
    
    guard let cgImage = image.cgImage else { return image }
    let context = CGContext(data: nil, width: Int(image.size.width), height: Int(image.size.height),
                            bitsPerComponent: cgImage.bitsPerComponent, bytesPerRow: 0,
                            space: cgImage.colorSpace!, bitmapInfo: cgImage.bitmapInfo.rawValue)
    context?.concatenate(transform)
    
    switch image.imageOrientation {
    case .left, .leftMirrored, .right, .rightMirrored:
        context?.draw(cgImage, in: CGRect(x: 0, y: 0, width: image.size.height, height: image.size.width))
    default:
        context?.draw(cgImage, in: CGRect(x: 0, y: 0, width: image.size.width, height: image.size.height))
    }
    
    guard let newCgImage = context?.makeImage() else { return image }
    return UIImage(cgImage: newCgImage)
  }
  
  private func cartesianForPoint(_ point: Point, height: CGFloat) -> CGPoint {
    return CGPointMake(point.x, height - point.y)
  }
}
