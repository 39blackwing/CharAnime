#define __STDC_CONSTANT_MACROS

#include "decoder/videodecoder.hpp"
#include "player/readconfig.hpp"
#include "player/videoplayer.hpp"
#include "threadshared/dataqueue.hpp"
#include "timer.hpp"

struct ThreadData {
  decoder::PackageDecoder package_decoder;
  decoder::VideoDecoder video_decoder;

  threadshared::DataQueue<threadshared::Packet, 64> video_queue;
  threadshared::DataQueue<threadshared::Image, 64> image_queue;

  player::VideoConfig video_config;
  player::VideoPlayer video_player;

  timer::Timer player_timer;
};

void ThreadDecodeVideo(ThreadData *data) {
  for (threadshared::PacketPtr packet_ptr = nullptr;
       (packet_ptr = data->video_queue.GetPopFront()) != nullptr;) {
    auto &&image_ptr = data->video_decoder.DecodePacket(packet_ptr);
    if (image_ptr == nullptr) {
      continue;
    }

    for (uint32_t i = 0; i < image_ptr->image_size; ++i) {
      image_ptr->data[i] = data->video_config[image_ptr->data[i]];
    }
    data->image_queue.PushBack(image_ptr);
  }
}

void ThreadPlayVideo(ThreadData *data) {
  data->player_timer.Update();
  for (threadshared::ImagePtr image_ptr = nullptr;
       (image_ptr = data->image_queue.GetPopFront()) != nullptr;) {
    data->player_timer.WaitSinceLastUpdate(image_ptr->pts);
    data->video_player.Play(reinterpret_cast<char *>(image_ptr->data));
  }
}

int main(int32_t argc, char *argv[]) {
  std::ios::sync_with_stdio(0);

  std::string filename;

  if (argc >= 2) {
    filename = std::string(argv[1]);
  } else {
    util::Print("Input file: ");
    std::cin >> filename;
  }

  auto data = new ThreadData;

  if (data->video_config.Init() != 0 ||
      data->package_decoder.Open(filename) != 0 ||
      data->video_decoder.Open(data->package_decoder, AV_PIX_FMT_GRAY8,
                               data->video_config.GetDstWidth(),
                               data->video_config.GetDstHeight()) != 0) {
    return -1;
  }

  util::Print("Start in 5 seconds...");
  data->player_timer.Update();
  data->player_timer.WaitSinceLastUpdate(5000);

  if (data->video_player.Init(data->video_config.GetDstWidth(),
                              data->video_config.GetDstHeight()) != 0) {
    return -1;
  }

  std::thread thread_decode_video(ThreadDecodeVideo, data);
  std::thread thread_play_video(ThreadPlayVideo, data);

  for (threadshared::PacketPtr packet_ptr = nullptr;
       (packet_ptr = data->package_decoder.ReadPacket()) != nullptr;) {
    if (packet_ptr->data->stream_index ==
        data->video_decoder.GetStreamIndex()) {
      data->video_queue.PushBack(packet_ptr);
    }
  }

  data->video_queue.Quit();
  if (thread_decode_video.joinable()) {
    thread_decode_video.join();
  }
  data->image_queue.Quit();
  if (thread_play_video.joinable()) {
    thread_play_video.join();
  }

  util::DeletePtr(data);

  util::Print("Done.\n");
  return 0;
}
