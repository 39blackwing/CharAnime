#ifndef SRC_THREADSHARED_DATASTRUCT_HPP
#define SRC_THREADSHARED_DATASTRUCT_HPP

extern "C" {
#include <libavformat/avformat.h>
}
#include <memory>
#include "../util.hpp"

namespace threadshared {

struct Packet {
  AVPacket *data = nullptr;

  ~Packet() { av_packet_free(&data); }

 protected:
  Packet() : data(av_packet_alloc()) {}

 public:
  static std::shared_ptr<Packet> MakeShared() {
    return std::shared_ptr<Packet>(new Packet());
  }
};

struct Image {
  uint8_t *data = nullptr;
  int64_t pts = -1;
  static uint32_t image_size;

  virtual ~Image() { util::DeleteArray(data); }

 protected:
  Image() {
    if (image_size <= 0) {
      throw image_size;
    }
    data = new uint8_t[image_size];
  }

 public:
  static std::shared_ptr<Image> MakeShared() {
    return std::shared_ptr<Image>(new Image());
  }
};

uint32_t Image::image_size = 0;

typedef std::shared_ptr<Packet> PacketPtr;
typedef std::shared_ptr<Image> ImagePtr;

}  // namespace threadshared

#endif  // SRC_THREADSHARED_DATASTRUCT_HPP
