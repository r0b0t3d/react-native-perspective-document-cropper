import { NitroModules } from 'react-native-nitro-modules'
import type {
  ImagePerspectiveCropper,
  Rectangle,
} from './specs/ImagePerspectiveCropper.nitro'

export const HybridImagePerspectiveCropper =
  NitroModules.createHybridObject<ImagePerspectiveCropper>(
    'ImagePerspectiveCropper'
  )

export function detectRectangle(
  image: string
): Promise<Rectangle> {
  return HybridImagePerspectiveCropper.detectRectangleForImage(
    image
  )
}

export function cropImage(
  image: string,
  rectangle: Rectangle
): Promise<string> {
  return HybridImagePerspectiveCropper.cropImage(image, rectangle)
}
