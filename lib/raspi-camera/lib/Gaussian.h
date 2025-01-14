#include <algorithm>
#include <math.h>

namespace Raspi {

namespace Utils { 

  /**
   * Optimizes Gaussian Blur 
   * For more info check out this
   * https://blog.ivank.net/fastest-gaussian-blur.html
  */
class Gaussian {
public:
  static void Blur(uint8_t* src, uint32_t width, uint32_t height, uint32_t r) {
    uint8_t dst[width * height], box[3];

    GaussBox(box, 3, r);
    BoxBlur(src, dst, width, height, (box[0] - 1) / 2);
    BoxBlur(dst, src, width, height, (box[1] - 1) / 2);
    BoxBlur(src, dst, width, height, (box[2] - 1) / 2);
  }

private:
  static void GaussBox(uint8_t* ptr, int32_t n, int32_t sigma) {
    int32_t wl = (int32_t)sqrtf((12 * sigma * sigma / n) + 1);  // Ideal averaging filter width 
    if (wl % 2 == 0) wl--;

    int32_t wu = wl + 2;
    int32_t m = (int32_t)roundf((12 * sigma * sigma - n * wl * wl - 4 * n * wl - 3 * n) / (-4 * wl - 4));

    for (int32_t i = 0; i < n; i++) ptr[i] = i < m ? wl : wu;
  }

  static void BoxBlur(uint8_t* src, uint8_t* dst, uint32_t width, uint32_t height, uint32_t r) {
    std::copy(src, src + width * height, dst);

    BoxBlurH(dst, src, width, height, r);
    BoxBlurT(src, dst, width, height, r);
  }

  static void BoxBlurH(uint8_t* src, uint8_t* dst, uint32_t width, uint32_t height, uint32_t r) {
    for (int32_t i = 0; i < height; i++) {
      for (int32_t j = 0; j < width; j++) {
        int32_t val = 0;
        for (int32_t ix = j - r; ix < j + r + 1; ix++) {
          int32_t x = std::min((int32_t)width - 1, std::max(0, ix));
          val += src[i * width + x];
        }

        dst[i * width + j] = (uint8_t)(val / (r + r + 1));
      }
    }
  }

  static void BoxBlurT(uint8_t* src, uint8_t* dst, uint32_t width, uint32_t height, uint32_t r) {
    for (int32_t i = 0; i < height; i++) {
      for (int32_t j = 0; j < width; j++) {
        int32_t val = 0;
        for (int32_t iy = i - r; iy < i + r + 1; iy++) {
          int32_t y = std::min((int32_t)height - 1, std::max(0, iy));
          val += src[y * width + j];
        }

        dst[i * width + j] = (uint8_t)(val / (r + r + 1));
      }
    }
  }

};
};
};