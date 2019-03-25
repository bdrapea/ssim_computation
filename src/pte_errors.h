#pragma once

#include <vector>
#include <string>
#include <stdexcept>

extern "C"
{
    #include <libavutil/error.h>
}

#define PTE_ERR_SUCCESS 0
#define PTE_ERR_PIVOT_NOT_FOUND 1
#define PTE_ERR_ENCODED_FOLDER_NOT_FOUND 2
#define PTE_ERR_NO_VIDEO_IN_FOLDER 3
#define PTE_ERR_LIBAVFORMAT_ERROR 4
#define PTE_ERR_PATH_NOT_VALID 5
#define PTE_ERR_LIBAV_FATAL_ERROR 6
#define PTE_ERR_POPEN_FAIL 7
#define PTE_ERR_CANT_FIND_SSIM_SCORE_IN_LOGS 8
#define PTE_ERR_LIBAVCODEC_ERROR 9

namespace pte
{
    static std::vector<std::string> error_names =
    {
        "Success",
        "Pivot not found",
        "Encoded folder not found",
        "No video file found in the encoded folder",
        "An error occured with function of libavformat",
        "The path submitted is not valid",
        "Cannot print libav error... Where is your god now ?"
        "Can't find ssim score in logs",
        "An error occured with function of libcodec"    };

    void print_error(const int error_id);

    std::string get_libav_error_message(const int libav_error_id);
}
