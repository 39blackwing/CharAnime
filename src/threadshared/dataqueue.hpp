#ifndef SRC_THREADSHARED_DATAQUEUE
#define SRC_THREADSHARED_DATAQUEUE

#include <condition_variable>
#include <memory>
#include "../util.hpp"

namespace threadshared {

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
 * class DataQueue
 *
 * A queue which is used to save thread data
 */
template <typename T, const uint32_t kQueueMaxLen>
class DataQueue final {
 public:
  inline void PushBack(const std::shared_ptr<T> &ptr);
  inline std::shared_ptr<T> GetPopFront();
  inline void Clear();
  inline void Quit();

  inline uint32_t GetNowLen() const { return now_len_; };

 public:
  DataQueue() : queue_(new std::shared_ptr<T>[kQueueMaxLen]) {}
  ~DataQueue() { util::DeleteArray(queue_); }

 private:
  std::shared_ptr<T> *queue_ = nullptr;
  uint32_t pos_head_ = 0, pos_tail_ = 0, now_len_ = 0;

  std::condition_variable cond_push_, cond_pop_;
  std::mutex mutex_;

  struct {
    uint8_t is_quit : 1;
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
 * Push to back.
 *
 * @param ptr: shared_ptr of data.
 *
 * If the queue is full, this function will sleep until
 *  GetPopFront() or Quit() or Clear() is called and return.
 */
template <typename T, const uint32_t kQueueMaxLen>
inline void DataQueue<T, kQueueMaxLen>::PushBack(
    const std::shared_ptr<T> &ptr) {
  std::unique_lock<std::mutex> lock(mutex_);

  while (now_len_ == kQueueMaxLen) {
    if (flag_.is_quit == 1) {
      return;
    }
    cond_pop_.wait(lock);
    if (flag_.is_quit == 1) {
      return;
    }
  }

  ++now_len_;
  queue_[pos_tail_] = ptr;
  if (++pos_tail_ == kQueueMaxLen) {
    pos_tail_ = 0;
  }

  cond_push_.notify_one();
}

/**
 * Pop front.
 *
 * @return: shared_ptr of data in normal state, nullptr in Quit() state.
 *
 * If the queue is empty, this function will sleep until
 *  Pushback() or Quit() is called and return.
 */
template <typename T, const uint32_t kQueueMaxLen>
inline std::shared_ptr<T> DataQueue<T, kQueueMaxLen>::GetPopFront() {
  std::unique_lock<std::mutex> lock(mutex_);

  while (now_len_ == 0) {
    if (flag_.is_quit == 1) {
      return nullptr;
    }
    cond_push_.wait(lock);
    if (flag_.is_quit == 1) {
      return nullptr;
    }
  }

  --now_len_;
  auto &ptr = queue_[pos_head_];
  if (++pos_head_ == kQueueMaxLen) {
    pos_head_ = 0;
  }

  cond_pop_.notify_one();
  return ptr;
}

/**
 * Clear the queue.
 */
template <typename T, const uint32_t kQueueMaxLen>
inline void DataQueue<T, kQueueMaxLen>::Clear() {
  std::unique_lock<std::mutex> lock(mutex_);

  util::DeleteArray(queue_);
  queue_ = new std::shared_ptr<T>[kQueueMaxLen];
  pos_head_ = pos_tail_ = now_len_ = 0;

  cond_pop_.notify_one();
}

/**
 * Quit all waiting function.
 */
template <typename T, const uint32_t kQueueMaxLen>
inline void DataQueue<T, kQueueMaxLen>::Quit() {
  std::unique_lock<std::mutex> lock(mutex_);

  flag_.is_quit = 1;
  cond_pop_.notify_one();
  cond_push_.notify_one();
}

};  // namespace threadshared

#endif  // SRC_THREADSHARED_DATAQUEUE
