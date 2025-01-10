#include <chrono>
#include <thread>
#include <memory>
#include <iostream>

#include <png.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <libcamera/libcamera.h>
// #include <png>
// #include <libcamera/fi.h>

static void requestComplete(libcamera::Request *request) {
  if (request->status() == libcamera::Request::RequestCancelled) return;

  const std::map<const libcamera::Stream*, libcamera::FrameBuffer*> &buffers = request->buffers();
  printf("YESSSS\n");


  for (auto &[stream, buffer] : buffers) {
    auto& planes = buffer->planes();

    for (const auto& plane : planes) {
      const int fd = plane.fd.get();

      printf("%d\n", plane.offset);

      png_bytep address = (png_bytep)mmap(nullptr, plane.offset + plane.length, PROT_READ, MAP_SHARED, fd, 0);


      FILE *fp = fopen("test.png", "wb");

      png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
      if (!png) abort();

      png_infop info = png_create_info_struct(png);
      if (!info) abort();

      if (setjmp(png_jmpbuf(png))) abort();

      png_init_io(png, fp);

      const auto& config = stream->configuration();

      // Output is 8bit depth, RGBA format.
      png_set_IHDR(
        png,
        info,
        config.size.width, config.size.height,
        // width, height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
      );
      png_write_info(png, info);

      // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
      // Use png_set_filler().
      //png_set_filler(png, 0, PNG_FILLER_AFTER);


      // png_bytep *row_pointers = NULL;

      // if (!row_pointers) abort();

      printf("(%dx%d)\n", config.size.width, config.size.height);


      // png_set_filler(png, 0, PNG_FILLER_AFTER);

      // png_bytep row_pointers = (png_bytep)malloc(sizeof(png_byte) * config.size.width * 4);
      // for (int i = 0; i < config.size.width; i++) row_pointers[i] = ((i % 3) == 1) * 255;

      // for (int y = 0; y < config.size.height; y++) {
      // //   // const auto size = png_get_rowbytes(png, info);
      // //   // row_pointers[y] = (png_byte*)malloc(size);

      //   // png_write_row(png, address + sizeof(uint8_t) * config.size.width * y);
      //   png_write_row(png, row_pointers);


      // png_bytepp row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * config.size.height);
      // for(int y = 0; y < config.size.height; y++) {
        // auto size = png_get_rowbytes(png,info);
        // row_pointers[y] = (png_byte*)malloc(size);

        // for (int x = 0; x < size; x++) {
        //   // if ((x % 4) == 3) row_pointers[y][x] = 255;
        //   row_pointers[y][x] = address[x];
        // }



      // }

      for (unsigned int y = 0; y < config.size.height; y++, address += config.stride) {
		    // output.write(row, rowLength);
      png_write_row(png, address);
	    }

      // png_write_image(png, row_pointers);


      // //   // for (int x = 0; x < size; x += sizeof)
      // }


      // png_write_image(png, row_pointers);
      png_write_end(png, NULL);

      // for (int y = 0; y < config.size.height; y++) free(row_pointers[y]);
      // free(row_pointers);

      fclose(fp);

      png_destroy_write_struct(&png, &info);

    }


    // const libcamera::FrameMetadata &metadata = buffer->metadata();
      // const libcamera::FrameBuffer::Plane &plane = buffer->planes()[0];
      // stream->configuration
      // buffer->metadata
      // maped
      // libcamera::Ima
      // libcamera::Span<uint8_t> t;
      // t.data()

      // plane.
  }
}

int main() {
  // std::shared_ptr<libcamera::Camera> camera;
  std::unique_ptr<libcamera::CameraManager> cm = std::make_unique<libcamera::CameraManager>();
  cm->start();

  auto cameras = cm->cameras();
  if (cameras.empty()) {
      std::cout << "No cameras were identified on the system."
                << std::endl;
      cm->stop();
      return EXIT_FAILURE;
  }

  for (auto const &camera : cm->cameras())
    std::cout << camera->id() << std::endl;



  std::string cameraId = cameras[0]->id();
  auto camera = cm->get(cameraId);
  camera->acquire();

  std::unique_ptr<libcamera::CameraConfiguration> config = camera->generateConfiguration( { libcamera::StreamRole::Viewfinder } );

  libcamera::StreamConfiguration &streamConfig = config->at(0);
  std::cout << "Default viewfinder configuration is: " << streamConfig.toString() << std::endl;

  streamConfig.size.width = 1296;
  streamConfig.size.height = 972;

  // streamConfig.size.width = 640;
  // streamConfig.size.height = 480;
  streamConfig.bufferCount = 1;

  config->validate();
  std::cout << "Validated viewfinder configuration is: " << streamConfig.toString() << std::endl;

  camera->configure(config.get());

  libcamera::FrameBufferAllocator *allocator = new libcamera::FrameBufferAllocator(camera);

  for (libcamera::StreamConfiguration &cfg : *config) {
      int ret = allocator->allocate(cfg.stream());
      if (ret < 0) {
          std::cerr << "Can't allocate buffers" << std::endl;
          return -ENOMEM;
      }
  
      size_t allocated = allocator->buffers(cfg.stream()).size();
      std::cout << "Allocated " << allocated << " buffers for stream" << std::endl;
  }

  libcamera::Stream *stream = streamConfig.stream();
  const std::vector<std::unique_ptr<libcamera::FrameBuffer>> &buffers = allocator->buffers(stream);
  std::vector<std::unique_ptr<libcamera::Request>> requests;

  for (unsigned int i = 0; i < buffers.size(); ++i) {
    std::unique_ptr<libcamera::Request> request = camera->createRequest();
    if (!request)
    {
        std::cerr << "Can't create request" << std::endl;
        return -ENOMEM;
    }

    const std::unique_ptr<libcamera::FrameBuffer> &buffer = buffers[i];
    int ret = request->addBuffer(stream, buffer.get());
    if (ret < 0)
    {
        std::cerr << "Can't set buffer for request"
              << std::endl;
        return ret;
    }

    requests.push_back(std::move(request));
  }

  camera->requestCompleted.connect(requestComplete);

  camera->start();
  for (std::unique_ptr<libcamera::Request> &request : requests)
   camera->queueRequest(request.get());



  printf("HERE\n");

  std::this_thread::sleep_for(std::chrono::milliseconds(5000));

  camera->stop();
  allocator->free(stream);
  delete allocator;
  camera->release();
  camera.reset();
  cm->stop();

  return 0;
}