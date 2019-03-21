#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <cstring>

#include "pte_errors.h"

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/error.h>
}

namespace pte
{
    int compute_ssims_from_different_encodings(std::vector<double>* ssims,
                                               const char* pivot_path,
                                               const char* encoding_folder_path);
    int get_video_file_names_from_folder(std::vector<std::string>* file_names,
                                         const char* folder_path);
    int compute_ssim(const char* ref_path, const char* diff_path);
    std::string add_file_name_to_dir_path(std::string dir_path, const std::string& file_name);
    bool check_if_file_exist(const char* path);
    bool check_if_folder_exist(const char* path);
    bool check_if_file_is_video(const std::string& file_name);
    int get_video_duration(const char* video_path, double* duration);
}
