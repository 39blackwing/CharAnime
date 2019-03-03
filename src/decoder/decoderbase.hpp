#ifndef SRC_DECODER_DECODERBASE_HPP
#define SRC_DECODER_DECODERBASE_HPP

#include "packagedecoder.hpp"
extern "C" {
#include <libavcodec/avcodec.h>
}

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
 * class DecoderBase
 * The base class of Audio and Video Decoder Class
 */
class DecoderBase {
 protected:
  inline int8_t Open(const PackageDecoder &package_decoder,
                     const AVMediaType &&media_type);
  inline int8_t DecodePacket(const threadshared::PacketPtr &packet_ptr);

  inline virtual void Close();

 public:
  inline const int32_t &GetStreamIndex() const { return stream_index_; }
  inline int8_t IsOpen() const { return flag_.open; }

 protected:
  inline DecoderBase() = default;
  inline virtual ~DecoderBase() { Close(); }

 protected:
  AVCodec *codec_ = nullptr;
  AVCodecContext *codec_context_ = nullptr;
  AVFrame *frame_decoded_ = nullptr;
  AVRational time_base_ = {0};

  int32_t stream_index_ = -1;

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
 * Open Decoder
 *
 * @param package_decoder: PackageDecoder has called PackageDecoder::open().
 * @param media_type: AVMEDIA_TYPE_VIDEO or AVMEDIA_TYPE_AUDIO.
 *
 * @return: 0: normal, >0: warning, <0: error.
 */
inline int8_t DecoderBase::Open(const PackageDecoder &package_decoder,
                                const AVMediaType &&media_type) {
  if (flag_.open == 1) {
    return 1;  // decoder is open.
  }
  if (package_decoder.IsOpen() == 0) {
    Close();
    return -1;  // package_decoder is closed.
  }

  stream_index_ = av_find_best_stream(package_decoder.avformat_context_,
                                      media_type, -1, -1, &codec_, 0);
  if (stream_index_ < 0 || codec_ == nullptr) {
    Close();
    return 2;  // stream or codec not found.
  }

  codec_context_ = avcodec_alloc_context3(codec_);
  int32_t &&errnum = avcodec_parameters_to_context(
      codec_context_,
      package_decoder.avformat_context_->streams[stream_index_]->codecpar);
  if (util::PrintAvError(errnum)) {
    Close();
    return -1;
  }

  errnum = avcodec_open2(codec_context_, codec_, nullptr);
  if (util::PrintAvError(errnum)) {
    Close();
    return -1;
  }

  frame_decoded_ = av_frame_alloc();
  time_base_ =
      package_decoder.avformat_context_->streams[stream_index_]->time_base;

  flag_.open = 1;
  return 0;
}

/**
 * Stop, close Decoder and release resources.
 *
 * Warning: The child base should override and call this
 *          at the end of child Close() function.
 */
inline void DecoderBase::Close() {
  stream_index_ = -1;
  flag_.open = 0;

  if (frame_decoded_ != nullptr) {
    av_frame_free(&frame_decoded_);
    frame_decoded_ = nullptr;
  }
  if (codec_context_ != nullptr) {
    avcodec_free_context(&codec_context_);
    codec_ = nullptr;
    codec_context_ = nullptr;
  }
}

/**
 * Decode a packet.
 *
 * @param packet_ptr: The PacketPtr to decode.
 *
 * @return: 0: normal, >0: warning, <0: error.
 */
inline int8_t DecoderBase::DecodePacket(
    const threadshared::PacketPtr &packet_ptr) {
  if (flag_.open == 0) {
    return 1;  // decoder is closed.
  }
  if (packet_ptr == nullptr || packet_ptr->data == nullptr ||
      packet_ptr->data->stream_index != stream_index_) {
    return 2;  // unknown packet.
  }

  if (frame_decoded_->pkt_size > 0) {
    av_frame_unref(frame_decoded_);
  }

  int32_t &&errnum = avcodec_send_packet(codec_context_, packet_ptr->data);
  if (errnum < 0 && errnum != AVERROR(EAGAIN) && errnum != AVERROR_EOF) {
    return -1;
  }

  errnum = avcodec_receive_frame(codec_context_, frame_decoded_);
  if (errnum < 0 && errnum != AVERROR_EOF) {
    return -1;
  }

  return 0;
}

}  // namespace decoder

#endif  // SRC_DECODER_DECODERBASE_HPP
