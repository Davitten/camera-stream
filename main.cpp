#include <thread>
#include <opencv2/opencv.hpp>
#include <librealsense2/rs.hpp>
#include <nadjieb/mjpeg_streamer.hpp>
#include <array>
#include <algorithm>
#include <ranges>

[[noreturn]] int main()
{
    rs2::pipeline pipe;
    rs2::config cfg;
    constexpr int frequency = 30;
    constexpr std::array exit_keys = {27, static_cast<int>('q')};
    constexpr auto enableGui = false;

    cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, frequency);
    constexpr size_t PORT = 8080;
    nadjieb::MJPEGStreamer streamer;

    streamer.start(PORT);
    pipe.start(cfg);
    while (true)
    {
        const rs2::frameset data = pipe.wait_for_frames();
        const rs2::video_frame frame = data.get_color_frame();

        const int width = frame.get_width();
        const int height = frame.get_height();

        const cv::Mat color(cv::Size(width, height), CV_8UC3, const_cast<void*>(frame.get_data()), cv::Mat::AUTO_STEP);
        if (enableGui)
        {
            // Display in a GUI
            cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
            cv::imshow("Display Image", color);
            if (const int key = cv::waitKey(1); std::ranges::find(exit_keys, key) != exit_keys.end()) break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / frequency));
        std::vector<uchar> buff_bgr;
        cv::imencode(".jpg", color, buff_bgr,{cv::IMWRITE_JPEG_QUALITY, 95});
        streamer.publish("/camera", std::string(buff_bgr.begin(), buff_bgr.end()));
    }
}
