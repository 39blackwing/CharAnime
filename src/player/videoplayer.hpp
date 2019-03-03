#ifndef SRC_PLAYER_VIDEOPLAYER_HPP
#define SRC_PLAYER_VIDEOPLAYER_HPP

#include <stdint.h>

namespace player {

class VideoPlayerBase {
 protected:
  virtual inline int8_t Init(const int32_t &width, const int32_t &height) {
    width_ = width;
    height_ = height;
    return 0;
  }

  virtual inline void Play(const char text[]) = 0;

  int32_t width_ = 0, height_ = 0;
};

}  // namespace player

#if _OS_WIN
#include "videosys/winplayer.hpp"
#else

#ifdef _OS_UNIX
#include "videosys/unixplayer.hpp"
#else
#error "Unknown OS"

#endif  // _OS_UNIX
#endif  // _OS_WIn

#endif  // SRC_PLAYER_VIDEOPLAYER_HPP
