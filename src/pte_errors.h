#pragma once

#include <vector>
#include <string>
#include <stdexcept>

#define PTE_ERR_SUCCESS 0
#define PTE_ERR_PIVOT_NOT_FOUND 1
#define PTE_ERR_ENCODED_FOLDER_NOT_FOUND 2
#define PTE_ERR_NO_VIDEO_IN_FOLDER 3
#define PTE_ERR_LIBAVFORMAT_ERROR 4

namespace pte
{
    static std::vector<std::string> error_names =
    {
        "Success",
        "Pivot not found",
        "Encoded folder not found",
        "No video file found in the encoded folder",
        "An error occured with function of libavformat"    };

    void print_error(const int error_id);
}
