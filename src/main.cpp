#include <thread>
#include <iostream>
#include <cerrno>
#include <cstdlib>
#include <chrono>
#include <vector>
#include <opencv2/opencv.hpp>
#include <librealsense2/rs.hpp>
#include <nadjieb/mjpeg_streamer.hpp>
#include <array>
#include <algorithm>
#include <ranges>
#include <unistd.h> // Header for POSIX API

int main()
{
    rs2::config cfg;
    rs2::context ctx;
    std::cout << "Querying devices" << std::endl;
    auto devices = ctx.query_devices();

    const auto start_time = std::chrono::steady_clock::now();
    constexpr auto timeout = std::chrono::seconds(10);
    while (devices.size() == 0)
    {
        if (std::chrono::steady_clock::now() >= start_time + timeout)
        {
            std::cout << "No device detected after waiting for " << timeout.count() << " seconds. Exiting.\n";
            return EXIT_FAILURE;
        }
        std::cout << "No device detected. Is it plugged in?\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        devices = ctx.query_devices();
    }
    std::cout << "Found " << devices.size() << " device(s)" << std::endl;
    for (const auto& dev : devices)
    {
        if (dev.supports(RS2_CAMERA_INFO_SERIAL_NUMBER))
        {
            const std::string serial = dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
            std::cout << "Device SN: " << serial << std::endl;
            cfg.enable_device(serial);
        }
        else
        {
            std::cout << "Device does not support serial number info\n";
        }

        if (dev.supports(RS2_CAMERA_INFO_NAME))
        {
            std::cout << "Device: " << dev.get_info(RS2_CAMERA_INFO_NAME) << "\n";
        }
        else
        {
            std::cout << "Device: Unknown\n";
        }
    }

    constexpr int frequency = 30;
    constexpr std::array exit_keys = {27, static_cast<int>('q')};
    constexpr auto enableGui = false;
    constexpr std::string_view endpoint = "/camera";
    std::cout << "Enabling stream" << std::endl;

    cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, frequency);
    rs2::pipeline pipe;
    pipe.start(cfg);
    constexpr size_t PORT = 8081;
    nadjieb::MJPEGStreamer streamer;

    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    streamer.start(PORT);
    std::cout << "started stream at: http://" << hostname << ":" << PORT << endpoint
    << " or if mDNS active http://" << hostname << ".local" << ":" << PORT << endpoint << std::endl;
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
        std::vector<uchar> buff_bgr(color.total() * color.elemSize());
        cv::imencode(".jpg", color, buff_bgr,{cv::IMWRITE_JPEG_QUALITY, 95});
        streamer.publish(endpoint.data(), std::string(buff_bgr.begin(), buff_bgr.end()));
    }
}
