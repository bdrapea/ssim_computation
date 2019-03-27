#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <cstring>
#include <sstream>
#include <array>
#include <memory>
#include <experimental/filesystem>

#include "pte_errors.h"

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
}

namespace fs = std::experimental::filesystem;

namespace pte
{
    int compute_ssims_from_different_encodings(const char* pivot_path,
                                               const char* encoding_folder_path,
                                               std::vector<double>* ssims,
                                               std::vector<std::string>* video_names);

    int get_video_file_names_from_folder(std::vector<std::string>* file_names,
                                         const char* folder_path);

    int delete_pivot_from_video_names(const std::string& pivot_name,
                                      std::vector<std::string>* video_names);


    int compute_ssim(const char* ref_path,
                     const char* diff_path,
                     double* ssim);

    std::string add_file_name_to_dir_path(std::string dir_path,
                                          const std::string& file_name);

    int get_file_name_from_path(const std::string& file_path,
                                std::string* file_name);

    bool check_if_file_exist(const char* path);

    bool check_if_folder_exist(const char* path);

    bool check_if_file_is_video(const std::string& file_name);

    bool check_if_video_is_scaled(const std::string& video_name);

    int get_video_duration(const char* video_path,
                           int64_t* duration);

    int get_video_width_height(const char* video_path,
                               int64_t* width,
                               int64_t* height);

    int scale_video(const std::string& video_path,
                    const std::string& dir_path,
                    const int64_t width,
                    const int64_t height,
                    std::string* path_to_scaled_video);

    int execute_and_get_log(const std::string& cmd,
                            std::string* logs);

    int get_average_ssim_from_logs(const std::string& logs,
                                   double* ssim);

    int generate_log_from_ssims(const std::vector<double>& ssims,
                                const std::vector<std::string>& video_names,
                                const std::string& path_to_log);

    int generate_canal_kjob(const fs::path& pivot_path,
                            fs::path* kjob_path);

    int replace_input_outputs_in_canal_kjob(const fs::path& input_path,
                                            const fs::path& output_dir_path,
                                            const fs::path& kjob_path);
}
