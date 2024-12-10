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
  image: string,
  onSuccess: (rectangle: Rectangle) => void,
  onError: (error: string) => void
) {
  HybridImagePerspectiveCropper.detectRectangleForImage(
    image,
    onSuccess,
    onError
  )
}

export function cropImage(
  image: string,
  rectangle: Rectangle,
  onSuccess: (image: string) => void,
  onError: (error: string) => void
) {
  HybridImagePerspectiveCropper.cropImage(image, rectangle, onSuccess, onError)
}
