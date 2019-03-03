#ifndef SRC_UTIL_HPP
#define SRC_UTIL_HPP

extern "C" {
#include <libavutil/avutil.h>
}
#include <iostream>
#include <mutex>

namespace util {

/**
 *
 *
 *
 *
 *
 * namespace operator
 *
 *
 *
 *
 *
 */
namespace operate {

inline int64_t operator*(const int64_t &x, const AVRational &y) {
  return (y.den == 0 ? 0 : x * y.num * 1000 / y.den);
}

}  // namespace operate

/**
 *
 *
 *
 *
 *
 * namespace pointer
 *
 *
 *
 *
 *
 */
namespace pointer {

template <typename T>
inline void DeletePtr(T *&ptr) {
  delete ptr, ptr = nullptr;
}

template <typename T>
inline void DeleteArray(T *&array) {
  delete[] array, array = nullptr;
}

}  // namespace pointer

/**
 *
 *
 *
 *
 *
 * namespace print
 *
 *
 *
 *
 *
 */
namespace print {

/**
 * Base class of all the print functor class in namespace UtilPrin.
 */
class BaseFunctorPrint {
 protected:
  static std::mutex output_mutex_;
  BaseFunctorPrint() = default;
};
std::mutex BaseFunctorPrint::output_mutex_;

/**
 * Print to screen
 *
 * @param data: Template type value.
 * @param opinion: 0: nothing, 1: std::flush, 2: std::endl
 */
class FunctorPrint final : public BaseFunctorPrint {
 public:
  template <typename T>
  inline void operator()(const T &data, const uint8_t &&opinion = 1) {
    output_mutex_.lock();
    std::cout << data;

    switch (opinion) {
      case 1:
        std::cout << std::flush;
        break;

      case 2:
        std::cout << std::endl;
        break;

      default:
        break;
    }

    output_mutex_.unlock();
  }
} Print;

/**
 * Print error to screen
 *
 * @param text: Error text.
 */
class FunctorPrintError final : public BaseFunctorPrint {
 public:
  inline void operator()(const char text[]) {
    output_mutex_.lock();
    std::cerr << "Error: " << text;
    output_mutex_.unlock();
  }
} PrintError;

/**
 * Print AvError if in error state,
 *
 * @param errnum: The return value of ffmpeg function.
 *
 * @return: 0: normal, -1: error.
 */
class FunctorPrintAvError final : public BaseFunctorPrint {
 public:
  char buf[AV_ERROR_MAX_STRING_SIZE] = {0};

  inline int8_t operator()(const int32_t &errnum) {
    if (errnum >= 0) {
      return 0;
    }
    av_make_error_string(buf, sizeof(buf), errnum);

    output_mutex_.lock();
    std::cerr << "AvError " << errnum << ": " << buf << "\n";
    output_mutex_.unlock();
    return -1;
  }
} PrintAvError;

}  // namespace print

/**
 *
 *
 *
 *
 *
 * USING
 *
 *
 *
 *
 *
 */
using namespace pointer;
using print::Print;
using print::PrintError;
using print::PrintAvError;

}  // namespace util

using namespace util::operate;

#endif  // SRC_UTIL_HPP
