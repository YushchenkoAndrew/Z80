#include <src/Image.h>

int main() {
  auto camera = Raspi::Camera();

  uint32_t size = camera.FrameSize();
  uint8_t* image = (uint8_t*)malloc(sizeof(uint8_t) * size);
  uint8_t* data = (uint8_t*)malloc(sizeof(uint8_t) * (size >> 2));

  camera.TakeFrame(image);
  Raspi::Utils::Image::RGBA2Luminance(image, data, size);

  Raspi::Utils::Image::GaussianBlur(data, 1296, 972);
  
  // TODO: Impl code here

  Raspi::Utils::Image::Luminance2RGBA(data, image, size);
  camera.Save(image);

  std::free(data);
  std::free(image);


  return 0;
}