#include "pte_utils.h"

namespace pte
{
    int compute_ssims_from_different_encodings(std::vector<double>* ssims,
                                               const char* pivot_path,
                                               const char* encoded_folder_path)
    {
        /** Check if the pivot exist **/
        if(!check_if_file_exist(pivot_path))
            return PTE_ERR_PIVOT_NOT_FOUND;

        /** Check if the encoder folder path is correct **/
        if(!check_if_folder_exist(encoded_folder_path))
            return PTE_ERR_ENCODED_FOLDER_NOT_FOUND;

        /** Select video file in the folder **/
        std::vector<std::string> video_names;
        int err = 0;
        err = get_video_file_names_from_folder(&video_names,encoded_folder_path);
        if(err) return err;

        /** Compute ssim for each video **/
        for(const std::string& video_name : video_names)
        {
            std::string diff_path = add_file_name_to_dir_path(encoded_folder_path,video_name);

            /** Check if the video is the same lenght as the pivot **/
            double duration = 0;
            err = get_video_duration(diff_path.c_str(),&duration);
            if(err) return err;
            std::cout << duration << std::endl;

        }
        return PTE_ERR_SUCCESS;
    }

    int get_video_file_names_from_folder(std::vector<std::string>* file_names,
                                         const char* folder_path)
    {
        DIR* folder = opendir(folder_path);
        struct dirent* entry;

        while((entry = readdir(folder)) != nullptr)
        {
            if(check_if_file_is_video(entry->d_name))
                file_names->push_back(entry->d_name);        }

        if(!file_names->size())
            return PTE_ERR_NO_VIDEO_IN_FOLDER;

        closedir(folder);
        return PTE_ERR_SUCCESS;    }

    int compute_ssim(const char* ref_path, const char* dif_path)
    {
        return PTE_ERR_SUCCESS;    }

    int get_video_duration(const char* video_path, double* duration)
    {
        int av_err = 0;
        AVformatContext* format_context = nullptr;
        avformat_alloc_context(format_context);

        if(format_context == nullptr)
            return PTE_ERR_LIBAVFORMAT_ERROR;

        if((av_err = avformat_open_input(&format_context,video_path,nullptr,nullptr)))
        {
        }

        avformat_free_context(format_context);
        return PTE_ERR_SUCCESS;
    }

    std::string add_file_name_to_dir_path(std::string dir_path, const std::string& file_name)
    {
        std::string absolute_path;

        #if defined WIN32 || defined WIN64
        if(dir_path[dir_path.size()-1] == '\\')
        dir_path.pop_back();

        absolute_path += dir_path + '\\' + file_name;

        #elif defined __linux__
        if(dir_path[dir_path.size()-1] == '/')
        dir_path.pop_back();

        absolute_path += dir_path + '/' + file_name;

        #else
            #error "OS not supported"
        #endif

        return absolute_path;    }

    bool check_if_file_exist(const char* path)
    {
        std::ifstream file(path);
        if(!file.is_open()) return false;
        return true;    }

    bool check_if_folder_exist(const char* path)
    {
        DIR* directory = opendir(path);
        if(directory != nullptr)
        {
            closedir(directory);
            return true;
        }

        return false;
    }

    bool check_if_file_is_video(const std::string& file_name)
    {
        std::vector<std::string> video_extensions =
        {
            ".mov",
            ".avi",
            ".wmv",
            ".mkv",
            ".mxf",
            ".mp4",
            ".mpg",
            ".ts"        };

        for(const std::string& extension : video_extensions)
        {
            std::string::size_type file_name_size = file_name.size();
            std::string::size_type extension_size = extension.size();

            if(file_name_size <= extension_size)
                return false;

            if(file_name.substr(file_name_size-extension_size, extension_size) == extension)
                return true;
        }

        return false;
    }
}
