#include <mutex>
#include <memory>
#include <algorithm>
#include <condition_variable>

#include <png.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <libcamera/libcamera.h>

namespace Raspi {

class Camera {
public:
  Camera() {
    auto cameras = manager()->cameras();
    if (!cameras.size()) printf("No cameras were identified on the system\n"), abort();

    camera = manager()->get(cameras[0]->id());
    camera->acquire();

    config = camera->generateConfiguration({ libcamera::StreamRole::Viewfinder });
    auto& stream = config->at(0);

    stream.size.width  = 1296;
    stream.size.height = 972;
    stream.bufferCount = 1;

    config->validate();
    camera->configure(config.get());

    allocator = std::make_unique<libcamera::FrameBufferAllocator>(camera);

    int32_t ret = allocator->allocate(stream.stream());
    if (ret < 0) printf("Can't allocate buffers\n"), abort();

    camera->requestCompleted.connect(this, &Camera::Request);
    camera->start();
  }

  ~Camera() {
    if (camera != nullptr) camera->stop(), camera->release(), allocator->free(config->at(0).stream());

    allocator.reset();
    camera.reset();
    config.reset();
    manager()->stop();
  }

  void TakeFrame(uint8_t* dst) {
    auto stream = config->at(0).stream();

    const auto& buffers = allocator->buffers(stream);
    const auto request = camera->createRequest();
    if (!request) printf("Can't create request\n"), abort();

    int32_t ret = request->addBuffer(stream, buffers[0].get());
    if (ret < 0) printf("Can't set buffer for request\n"), abort();

    camera->queueRequest(request.get());

    std::unique_lock<std::mutex> lock(frame.first);
    frame.second.wait(lock);

    const auto& planes = buffers[0]->planes(); 
    if (planes.size() != 1) printf("Unexpected amount of planes\n"), abort();

    const auto& plane = planes.at(0);
    const uint32_t fd = plane.fd.get();

    uint8_t* address = (uint8_t*)mmap(nullptr, plane.offset + plane.length, PROT_READ, MAP_SHARED, fd, 0);

    for (uint32_t i = 0, size = FrameSize(); i < size; i++) {
      dst[i] = address[i ^ ((i % 2 == 0) << 1)]; // NOTE: Swap R & B, for some reason those data from mmap swapped
    }
  }

  void Request(libcamera::Request* request) { frame.second.notify_all(); }

  void Save(uint8_t* ptr) {
    FILE* fp = fopen("test.png", "wb");
    const auto& size = config->at(0).size;

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) printf("Failed to create PNG structure\n"), fclose(fp), abort();

    png_infop info = png_create_info_struct(png);
    if (!info)  printf("Failed to create PNG info structure\n"), fclose(fp), abort();
    if (setjmp(png_jmpbuf(png))) printf("Error during PNG creation\n"), png_destroy_write_struct(&png, &info), fclose(fp), abort();

    png_init_io(png, fp);
    png_set_IHDR(
      png,
      info,
      size.width, size.height,
      8,
      PNG_COLOR_TYPE_RGBA,
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT
    );

    png_write_info(png, info);

    for (uint32_t i = 0, stride = config->at(0).stride; i < size.height; i++) {
      png_write_row(png, ptr + stride * i);
	  }

    png_write_end(png, NULL);

    fclose(fp);
    png_destroy_write_struct(&png, &info);
  }

private:
  static inline std::unique_ptr<libcamera::CameraManager>& manager() {
    static std::unique_ptr<libcamera::CameraManager> value;
    if (value == nullptr) value = std::make_unique<libcamera::CameraManager>(), value->start();

    return value;
  }

public:
  inline uint32_t FrameSize() { return config != nullptr ? config->at(0).stride * config->at(0).size.height : 0; }
  inline libcamera::Size Size() { return config != nullptr ? config->at(0).size : libcamera::Size(); }

private: 
  std::shared_ptr<libcamera::Camera> camera;
  std::unique_ptr<libcamera::CameraConfiguration> config;
  std::unique_ptr<libcamera::FrameBufferAllocator> allocator;

  std::pair<std::mutex, std::condition_variable> frame;
  uint8_t* address;
};
};