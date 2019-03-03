#ifndef SRC_READCONFIG_HPP
#define SRC_READCONFIG_HPP

#include <fstream>
#include "../util.hpp"

namespace player {

class VideoConfig final {
  int32_t dst_width_ = -1, dst_height_ = -1;
  char gray_map_[256] = {0};

 public:
  inline const char &operator[](const uint8_t &x) const { return gray_map_[x]; }
  inline const int32_t &GetDstWidth() const { return dst_width_; }
  inline const int32_t &GetDstHeight() const { return dst_height_; }

 public:
  /**
   * Init VideoConfig
   *
   * @return: 0 on successed, -1 on error.
   */
  inline int8_t Init() {
    std::ifstream in_file("charanime_config.txt");
    if (!in_file) {
      util::PrintError("Missing \"charanime_config.txt\".\n");
      return -1;
    }
    std::string line;

    in_file >> dst_width_ >> dst_height_;
    if (dst_width_ <= 0 || dst_height_ <= 0) {
      util::PrintError("Config file error.\n");
      return -1;
    }
    for (int32_t l, r; in_file >> l >> r;) {
      if (l < 0 || r > 255 || l > r || !std::getline(in_file, line)) {
        util::PrintError("Config file error.\n");
        return -1;
      }
      size_t pos = line.find_first_of('\'') + 1;
      if (pos == 0) {
        util::PrintError("Config file error.\n");
        return -1;
      }
      for (char c = *(line.cbegin() + pos); l <= r; ++l) {
        gray_map_[l] = c;
      }
    }
    return 0;
  }
};

}  // namespace player

#endif  // SRC_READCONFIG_HPP
