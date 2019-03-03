#ifndef SRC_TIMER_HPP
#define SRC_TIMER_HPP

#include <chrono>
#include <thread>

namespace timer {

class Timer final {
 public:
  inline Timer() { Update(); }

  inline void Update() { clock_ = std::chrono::high_resolution_clock::now(); }

  inline void WaitSinceLastUpdate(const int64_t &ms) {
    std::this_thread::sleep_until(clock_ + std::chrono::milliseconds(ms));
  }

 private:
  std::chrono::high_resolution_clock::time_point clock_;
};

}  // namespace timer

#endif  // SRC_TIMER_HPP
