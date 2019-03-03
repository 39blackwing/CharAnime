#ifndef SRC_PLAYER_VIDEOSYS_UNIXPLAYER
#define SRC_PLAYER_VIDEOSYS_UNIXPLAYER

#ifdef _OS_UNIX

#include <iostream>

namespace player {

class VideoPlayer final : public VideoPlayerBase {
 public:
  inline int8_t Init(const int32_t &width, const int32_t &height) override {
    VideoPlayerBase::Init(width, height);
    if (system("reset") != 0) {
      util::PrintError("Init player\n");
      return -1;
    }

    std::cout << "\033[?25l" << std::flush;  // hide cursor;
    com_move_cursor_ = std::string("\033[") + std::to_string(height_) +
                       "A\033[" + std::to_string(width_) + "D";
    return 0;
  }

  ~VideoPlayer() {
    std::cout << "\033[?25h" << std::flush;  // show cursor;
  }

  inline void Play(const char text[]) override {
    std::cout << com_move_cursor_ << "\033[K";
    std::cout.write(text, width_);
    for (register int32_t i = 1; i < height_; ++i) {
      text += width_;
      std::cout << std::endl;
      std::cout.write(text, width_);
    }
  }

 private:
  std::string com_move_cursor_;
};

}  // namespace player

#endif  // _OS_UNIX

#endif  // SRC_PLAYER_VIDEOSYS_UNIXPLAYER
