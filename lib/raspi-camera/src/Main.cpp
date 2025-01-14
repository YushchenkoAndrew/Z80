#include <src/Camera.h>
#include <lib/Image.h>
#include <lib/Gaussian.h>

int main() {
  auto camera = Raspi::Camera();

  const auto size = camera.Size();
  const uint32_t len = camera.FrameSize();

  uint8_t* image = (uint8_t*)malloc(sizeof(uint8_t) * len);
  uint8_t* data = (uint8_t*)malloc(sizeof(uint8_t) * size.height * size.width);

  camera.TakeFrame(image);
  Raspi::Utils::Image::RGBA2Luminance(image, data, len);

  Raspi::Utils::Gaussian::Blur(data, size.width, size.height, 3);
  
  // TODO: Impl code here

  Raspi::Utils::Image::Luminance2RGBA(data, image, len);
  camera.Save(image);

  std::free(data);
  std::free(image);


  return 0;
}