#ifndef PLAYER_SYSPLAYER_WINPLAYER_HPP
#define PLAYER_SYSPLAYER_WINPLAYER_HPP

#ifdef _OS_WIN

#include <windows.h>
#include "../../util.hpp"
#include "../videoplayer.hpp"

namespace player {

class VideoPlayer final : public VideoPlayerBase {
 public:
  inline int8_t Init(const int32_t &width, const int32_t &height) override {
    VideoPlayerBase::Init(width, height);

    auto com = std::string("mode ") + std::to_string(width_) + "," +
               std::to_string(height_);
    if (system(com.c_str()) != 0) {
      util::PrintError("Init player\n");
      return -1;
    }

    SetConsoleTitleA("CharAnime Player");

    cmd_main_handle_ = CreateConsoleScreenBuffer(GENERIC_WRITE | GENERIC_READ,
                                                 FILE_SHARE_WRITE, NULL,
                                                 CONSOLE_TEXTMODE_BUFFER, NULL);
    cmd_buf_handle_ = CreateConsoleScreenBuffer(GENERIC_WRITE | GENERIC_READ,
                                                FILE_SHARE_WRITE, NULL,
                                                CONSOLE_TEXTMODE_BUFFER, NULL);

    CONSOLE_CURSOR_INFO cci;
    cci.bVisible = 0, cci.dwSize = 1;
    if (cmd_main_handle_ == INVALID_HANDLE_VALUE ||
        cmd_buf_handle_ == INVALID_HANDLE_VALUE ||
        SetConsoleCursorInfo(cmd_main_handle_, &cci) == 0 ||
        SetConsoleCursorInfo(cmd_buf_handle_, &cci) == 0) {
      util::PrintError("Init player\n");
      return -1;
    }

    return 0;
  }

  ~VideoPlayer() {
    CloseHandle(cmd_main_handle_);
    CloseHandle(cmd_buf_handle_);
  }

  inline void Play(const char text[]) override {
    WriteConsoleOutputCharacterA(cmd_main_handle_, text, width_ * height_,
                                 coord, &written_bytes);
    SetConsoleActiveScreenBuffer(cmd_main_handle_);
    HANDLE x = cmd_main_handle_;
    cmd_main_handle_ = cmd_buf_handle_;
    cmd_buf_handle_ = x;
  }

 private:
  HANDLE cmd_main_handle_ = INVALID_HANDLE_VALUE;
  HANDLE cmd_buf_handle_ = INVALID_HANDLE_VALUE;
  COORD coord = {0};
  DWORD written_bytes = 0;
};

}  // namespace player

#endif  // _OS_WIN

#endif  // PLAYER_SYSPLAYER_WINPLAYER_HPP
