#include <stdint.h>

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
};
};
};