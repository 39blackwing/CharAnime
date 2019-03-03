#ifndef SRC_DECODER_VIDEODECODER_HPP
#define SRC_DECODER_VIDEODECODER_HPP

extern "C" {
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}
#include "../threadshared/datastruct.hpp"
#include "decoderbase.hpp"

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
 * class VideoDecoder
 * The class of Video Decoder.
 */
class VideoDecoder final : public DecoderBase {
 public:
  inline int8_t Open(const PackageDecoder &package_decoder,
                     const AVPixelFormat &dst_format, const int32_t &dst_width,
                     const int32_t &dst_height);
  inline void Close() override;
  inline threadshared::ImagePtr DecodePacket(
      const threadshared::PacketPtr &packet_ptr);

 public:
  inline int32_t GetDstWidth() const { return dst_width_; }
  inline int32_t GetDstHeight() const { return dst_height_; }
  inline int32_t GetDstLineSize() const { return dst_linesize_; }
  inline int32_t GetDstImageSize() const { return dst_height_ * dst_linesize_; }

 public:
  ~VideoDecoder() { Close(); }

 private:
  SwsContext *sws_context_ = nullptr;

  int32_t dst_width_ = 0, dst_height_ = 0, dst_linesize_ = 0;
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
 * @param package_decoder: PackageDecoder in open state.
 * @param dst_formae: AV_PIX_FMT_?.
 * @param dst_width & dst_height:
 *  If both of them are >0, the output image use the param size.
 *  If only one of them are =0, zoom the image.
 *  If both of them are <0 it is considered as screen-size value, the output
 *    image size will be set to adapt to your screen.
 *  If both of them are =0 or in other cases, the output image use source size.
 *
 * @return: 0: normal, >0: warning, <0: error.
 */
inline int8_t VideoDecoder::Open(const PackageDecoder &package_decoder,
                                 const AVPixelFormat &dst_format,
                                 const int32_t &dst_width = 0,
                                 const int32_t &dst_height = 0) {
  auto &&errnum = DecoderBase::Open(package_decoder, AVMEDIA_TYPE_VIDEO);
  if (errnum != 0) {
    return errnum;
  }

  if (dst_width > 0 && dst_height > 0) {  // param size
    dst_width_ = dst_width;
    dst_height_ = dst_height;
  } else if (dst_width > 0) {  // zoom
    dst_width_ = dst_width;
    dst_height_ = dst_width_ * codec_context_->height / codec_context_->width;
  } else if (dst_height > 0) {  // zoom
    dst_height_ = dst_height;
    dst_width_ = dst_height_ * codec_context_->width / codec_context_->height;
  } else if (dst_width < 0 && dst_height < 0) {  // adapt
    if (-dst_width * codec_context_->height <
        -dst_height * codec_context_->width) {
      dst_width_ = -codec_context_->width;
      dst_height_ = dst_width_ * codec_context_->height / codec_context_->width;
    } else {
      dst_height_ = -codec_context_->height;
      dst_width_ = dst_height_ * codec_context_->width / codec_context_->height;
    }
  } else {  // source
    dst_width_ = codec_context_->width;
    dst_height_ = codec_context_->height;
  }

  dst_linesize_ = av_image_get_buffer_size(dst_format, dst_width_, 1, 1);

  sws_context_ =
      sws_getContext(codec_context_->width, codec_context_->height,
                     codec_context_->pix_fmt, dst_width_, dst_height_,
                     dst_format, SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
  if (sws_context_ == nullptr) {
    return -1;  // sws_context_ error;
  }

  threadshared::Image::image_size = GetDstImageSize();

  return 0;
}

/**
 * Stop, close Decoder and release resources.
 */
inline void VideoDecoder::Close() {
  DecoderBase::Close();
  if (sws_context_ != nullptr) {
    sws_freeContext(sws_context_);
    sws_context_ = nullptr;
  }
}

inline threadshared::ImagePtr VideoDecoder::DecodePacket(
    const threadshared::PacketPtr &packet_ptr) {
  if (DecoderBase::DecodePacket(packet_ptr) != 0) {
    return nullptr;
  }

  auto image_ptr = threadshared::Image::MakeShared();

  sws_scale(sws_context_, frame_decoded_->data, frame_decoded_->linesize, 0,
            codec_context_->height, &image_ptr->data, &dst_linesize_);

  image_ptr->pts = (frame_decoded_->pts != AV_NOPTS_VALUE
                        ? image_ptr->pts = frame_decoded_->pts * time_base_
                        : -1);
  return image_ptr;
}

}  // namespace decoder

#endif  // SRC_DECODER_VIDEODECODER_HPP
