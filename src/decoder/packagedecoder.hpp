#ifndef SRC_DECODER_PACKAGEDECODER_HPP
#define SRC_DECODER_PACKAGEDECODER_HPP

#include <string>
#include "../threadshared/datastruct.hpp"
#include "../util.hpp"

namespace decoder {

/**
 *
 *
 *
 *
 *
 * CLASS DEFINE
 *
 *
 *
 *
 *
 */

/**
 * class PackageDecoder
 * A class about ffmpeg file package decode.
 */
class PackageDecoder final {
  friend class DecoderBase;

 public:
  inline int8_t Open(const std::string &filename);
  inline void Close();
  inline threadshared::PacketPtr ReadPacket();

 public:
  inline int8_t IsOpen() const { return flag_.open; }

 public:
  inline ~PackageDecoder() { Close(); }

 private:
  AVFormatContext *avformat_context_ = nullptr;

  struct {
    uint8_t open : 1;
  } flag_ = {0};
};

/**
 *
 *
 *
 *
 *
 * CLASS MEMBER FUNCTION DEFINE
 *
 *
 *
 *
 *
 */

/**
 * Open PackageDecoder.
 *
 * @param filename: the name of media file to open.
 *
 * @return: 0: normal, >0: warning, <0: error.
 */
inline int8_t PackageDecoder::Open(const std::string &filename) {
  if (flag_.open == 1) {
    return 1;
  }
  util::Print("FFMPEG Info: ", 0);
  util::Print(av_version_info(), 2);

  // Open file.
  int32_t &&errnum = avformat_open_input(&avformat_context_, filename.c_str(),
                                     nullptr, nullptr);
  if (util::PrintAvError(errnum)) {
    Close();
    return -1;
  }

  // Find and print stream info.
  errnum = avformat_find_stream_info(avformat_context_, nullptr);
  if (util::PrintAvError(errnum)) {
    Close();
    return -1;
  }
  av_dump_format(avformat_context_, 0, filename.c_str(), 0);

  flag_.open = 1;
  return 0;
}

/**
 * Stop, close PackageDecoder and release resources.
 */
inline void PackageDecoder::Close() {
  flag_.open = 0;
  if (avformat_context_ != nullptr) {
    avformat_close_input(&avformat_context_);
    avformat_context_ = nullptr;
  }
}

/**
 * Read an AVPacket for decoding.
 *
 * @return: PacketPtr if success, nullptr if failed.
 */
inline threadshared::PacketPtr PackageDecoder::ReadPacket() {
  if (flag_.open == 0) {
    return nullptr;
  }
  auto packet_ptr = threadshared::Packet::MakeShared();
  if (av_read_frame(avformat_context_, packet_ptr->data) < 0) {
    return nullptr;
  }
  return packet_ptr;
}

}  // namespace decoder

#endif  // SRC_DECODER_MAINDECODER_HPP
