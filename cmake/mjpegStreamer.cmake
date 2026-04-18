include(FetchContent)
FetchContent_Declare(
    nadjieb_mjpeg_streamer
    GIT_REPOSITORY git@github.com:nadjieb/cpp-mjpeg-streamer.git
    GIT_TAG 43692a0f917bea8abe1fb2b3104137ab7a45f895
)

FetchContent_MakeAvailable(nadjieb_mjpeg_streamer)