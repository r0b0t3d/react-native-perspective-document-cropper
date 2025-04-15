import { type HybridObject } from 'react-native-nitro-modules'

interface Point {
  x: number
  y: number
}

export interface Rectangle {
  topLeft: Point
  topRight: Point
  bottomLeft: Point
  bottomRight: Point
  width: number
  height: number
}

export interface ImagePerspectiveCropper
  extends HybridObject<{ ios: 'swift'; android: 'kotlin' }> {
  detectRectangleForImage(image: string): Promise<Rectangle>
  cropImage(
    image: string,
    rectangle: Rectangle
  ): Promise<string>
}
