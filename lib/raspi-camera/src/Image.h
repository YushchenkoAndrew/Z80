#include "Camera.h"


namespace Raspi {

namespace Utils { 

class Image {

public:
  static void RGBA2Luminance(uint8_t* src, uint8_t* dst, uint32_t size) {
    for (uint32_t i = 0; i < size; i += 4) {
      dst[i >> 2] = (uint8_t)(0.2126f * src[i] + 0.7152f * src[i + 1] + 0.0722f * src[i + 2]);
    }
  }

  static void Luminance2RGBA(uint8_t* src, uint8_t* dst, uint32_t size) {
    for (uint32_t i = 0; i < size; i += 4) {
      dst[i + 0] = src[i >> 2];
      dst[i + 1] = src[i >> 2];
      dst[i + 2] = src[i >> 2];
      dst[i + 3] = 255;
    }
  }

  static void GaussianBlur(uint8_t* src, uint32_t widht, uint32_t height) {
    for (uint32_t i = 1; i < height - 1; i++) {
      for (uint32_t j = 1; j < widht - 1; j++) {
        // auto sum = src[i]
        // TODO: impl this

        auto sum =
        src[(i - 1) * widht + j - 1] +
        src[(i - 1) * widht + j - 0] +
        src[(i - 1) * widht + j + 1] +
        src[(i - 0) * widht + j - 1] +
        src[(i - 0) * widht + j - 0] +
        src[(i - 0) * widht + j + 1] +
        src[(i + 1) * widht + j - 1] +
        src[(i + 1) * widht + j - 0] +
        src[(i + 1) * widht + j + 1];

        src[i * widht + j] = (uint8_t)(sum / 9);
      }
    }

    // const auto& size = config->at(0).size;
    // for (uint32_t i = 0; i < height; i++) {}

  }

};
};
}