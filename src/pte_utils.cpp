#include "pte_utils.h"

namespace pte
{
    int compute_ssims_from_different_encodings(const char* pivot_path,
                                               const char* encoded_folder_path,
                                               std::vector<double>* ssims,
                                               std::vector<std::string>* video_names)
    {
        std::cout << "====================ANALYZE PIVOT===================\n" << std::endl;
        std::cout << "Check if pivot exist: " << std::flush;
        if(!check_if_file_exist(pivot_path))
            return PTE_ERR_PIVOT_NOT_FOUND;
        std::cout << "OK\n" << std::endl;

        std::cout << "Check if the encoder folder path is correct: " << std::flush;
        if(!check_if_folder_exist(encoded_folder_path))
            return PTE_ERR_ENCODED_FOLDER_NOT_FOUND;
        std::cout << "OK\n" << std::endl;

        std::cout << "Select video file in the folder: " << std::flush;
        int err = 0;
        err = get_video_file_names_from_folder(video_names,encoded_folder_path);
        if(err) return err;
        std::cout << "OK\n" << std::endl;

        std::cout << "Delete the pivot name if it appears in the video names vector: " << std::flush;
        std::string pivot_name;
        err = get_file_name_from_path(pivot_path,&pivot_name);
        if(err) return err;
        std::cout << "OK\n" << std::endl;

        err = delete_pivot_from_video_names(pivot_name,video_names);
        if(err) return err;

        std::cout << "Seeking informations from pivot's stream for the further computations... " << std::endl;
        int64_t pivot_duration = 0;
        err = get_video_duration(pivot_path,&pivot_duration);
        if(err) return err;
        std::cout << "pivot's duration = " << pivot_duration << "s OK\n" << std::endl;

        int64_t pivot_height = 0, pivot_width = 0;
        err = get_video_width_height(pivot_path,&pivot_width, &pivot_height);
        if(err) return err;
        std::cout << "pivot's dimensions = " << pivot_width << 'x' << pivot_height << " OK\n" << std::endl;


        /** For each video **/
        for(const std::string& video_name : *video_names)
        {
            std::cout << "====================COMPUTING: " << video_name << " ===================\n" << std::endl;
            std::string diff_path = add_file_name_to_dir_path(encoded_folder_path,video_name);

            std::cout << "Check if " << video_name << " is the same duration as pivot: " << std::flush;
            int64_t duration = 0;
            err = get_video_duration(diff_path.c_str(),&duration);
            if(err) return err;

            if(duration > pivot_duration-2 && duration < pivot_duration+2)
            {
                std::cout << "OK\n" << std::endl;

                bool is_rescaled = false;
                int64_t width = 0, height = 0;
                err = get_video_width_height(diff_path.c_str(),&width, &height);
                if(err) return err;

                std::string path_to_scaled_video = diff_path;
                if((width != pivot_width) || (height != pivot_height))
                {
                    std::cout << "Rescaling " << video_name << " from " << width << 'x' << height << " -> " << pivot_width << 'x' << pivot_height << ':' << std::flush;
                    err = scale_video(diff_path,encoded_folder_path,pivot_width,pivot_height,&path_to_scaled_video);
                    if(err) return err;
                    is_rescaled = true;
                    std::cout << " OK\n" << std::endl;
                }

                std::cout << "Compute the ssim for pivot and encoded video... " << std::endl;
                double ssim = 0;
                err = compute_ssim(pivot_path,path_to_scaled_video.c_str(),&ssim);
                if(err) return err;
                ssims->push_back(ssim);
                std::cout << "ssim = " << ssim << '\n'<< std::endl;


                if(is_rescaled)
                {
                    std::cout << "Deleting: " << path_to_scaled_video << '\n' << std::endl;                    remove(path_to_scaled_video.c_str());
                }

            }
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
            if(check_if_file_is_video(entry->d_name) && check_if_video_is_scaled(entry->d_name))
                file_names->push_back(entry->d_name);        }

        if(!file_names->size())
            return PTE_ERR_NO_VIDEO_IN_FOLDER;

        closedir(folder);
        return PTE_ERR_SUCCESS;    }

    int delete_pivot_from_video_names(const std::string& pivot_name,
                                      std::vector<std::string>* video_names)
    {
        std::vector<std::string>::iterator it;
        for(it = video_names->begin(); it != video_names->end(); ++it)
            if(pivot_name == *it)
            {
                video_names->erase(it);            }

        return PTE_ERR_SUCCESS;
    }

    int compute_ssim(const char* ref_path,
                     const char* dif_path,
                     double* ssim)
    {
        std::stringstream ffmpeg_cmd;

        ffmpeg_cmd  << "ffmpeg -i ";
        ffmpeg_cmd << dif_path;
        ffmpeg_cmd << " -i ";
        ffmpeg_cmd << ref_path;
        ffmpeg_cmd << " -lavfi ssim -f null -";
        ffmpeg_cmd << " 2>&1 >/dev/null";

        std::string logs;
        int err = execute_and_get_log(ffmpeg_cmd.str(), &logs);
        if(err) return err;

        err = get_average_ssim_from_logs(logs,ssim);
        if(err) return err;

        return PTE_ERR_SUCCESS;    }

    std::string add_file_name_to_dir_path(std::string dir_path,
                                          const std::string& file_name)
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

    int get_file_name_from_path(const std::string& file_path,
                                std::string* file_name)
    {
        std::string::size_type file_path_size = file_path.size();
        std::string::size_type ind = file_path_size-1;
        const char* file_path_data = file_path.data();

        #if defined WIN32 || defined WIN64
        while(file_path_data[ind] != '\\')
        {
            if(ind == 0)
                return PTE_ERR_PATH_NOT_VALID;
            ind --;
        }

        #elif defined __linux__
        while(file_path_data[ind] != '/')
        {
            if(ind == 0)
                return PTE_ERR_PATH_NOT_VALID;
            ind --;
        }
        #else
            #error "OS not supported"
        #endif

        *file_name = file_path.substr(ind+1, file_path_size-ind);

        return PTE_ERR_SUCCESS;
    }

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
        return false;    }

    bool check_if_video_is_scaled(const std::string& video_path)
    {
        if(video_path.find("Scaled") != std::string::npos)
            return false;

        return true;    }

    int get_video_duration(const char* video_path,
                           int64_t* duration)
    {
        int av_err = 0;
        AVFormatContext* format_context = avformat_alloc_context();

        if(format_context == nullptr)
            return PTE_ERR_LIBAVFORMAT_ERROR;

        if(!(av_err = avformat_open_input(&format_context,video_path,nullptr,nullptr)))
        {
            if( (av_err = avformat_find_stream_info(format_context, nullptr)) )
                return PTE_ERR_LIBAVFORMAT_ERROR;

            *duration = format_context->duration/AV_TIME_BASE;

            avformat_close_input(&format_context);
            avformat_free_context(format_context);

            return PTE_ERR_SUCCESS;
        }
        else
        {
            std::cerr << get_libav_error_message(av_err) << std::endl;
            avformat_free_context(format_context);
            return PTE_ERR_LIBAVFORMAT_ERROR;
        }
    }

    int get_video_width_height(const char* video_path,
                               int64_t* width,
                               int64_t* height)
    {
        int av_err = 0;
        AVFormatContext* format_context = avformat_alloc_context();
        if(format_context == nullptr)
            return PTE_ERR_LIBAV_FATAL_ERROR;

        if( (av_err = avformat_open_input(&format_context,video_path,nullptr,nullptr)) )
        {
            std::cerr << get_libav_error_message(av_err) << std::endl;
            return PTE_ERR_LIBAVFORMAT_ERROR;        }

        if( (av_err = avformat_find_stream_info(format_context,nullptr) < 0) )
        {
            std::cerr << get_libav_error_message(av_err) << std::endl;
            return PTE_ERR_LIBAVFORMAT_ERROR;        }

        int video_stream = 0;
        for(size_t i=0; i<format_context->nb_streams; i++)
            if(format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                video_stream = i;

        AVCodecParameters* codec_parameters = format_context->streams[video_stream]->codecpar;

        *width = codec_parameters->width;
        *height = codec_parameters->height;

        avformat_close_input(&format_context);
        avformat_free_context(format_context);

        return PTE_ERR_SUCCESS;
    }

    int scale_video(const std::string& video_path,
                    const std::string&  dir_path,
                    const int64_t width,
                    const int64_t height,
                    std::string* path_to_scaled_video)
    {
        std::string video_name = "";
        int err = get_file_name_from_path(video_path.c_str(),&video_name);
        if(err) return PTE_ERR_PATH_NOT_VALID;

        std::string scaled_video_name = "Scaled_" + std::to_string(width) + 'x' + std::to_string(height) + video_name;
        std::string scaled_video_path = add_file_name_to_dir_path(dir_path,scaled_video_name);

        std::stringstream ffmpeg_cmd;
        ffmpeg_cmd << "ffmpeg -y -i " << video_path;
        ffmpeg_cmd << " -vf scale=" << width << ':' << height;

        if(width == 3840 && height == 2160)
            ffmpeg_cmd  << " -c:v libx265";
        else
            ffmpeg_cmd << " -c:v libx264";
            ffmpeg_cmd << " -crf 0";
            ffmpeg_cmd << " -preset ultrafast";
            ffmpeg_cmd << " " << scaled_video_path;
            ffmpeg_cmd << " 2>&1 >/dev/null";

        *path_to_scaled_video = scaled_video_path;

        std::string log;
        err = execute_and_get_log(ffmpeg_cmd.str(),&log);
        return err;

        return PTE_ERR_SUCCESS;
    }

    int execute_and_get_log(const std::string& cmd,
                            std::string* logs)
    {
        std::string buffer(256,0);

        FILE* fp = popen(cmd.c_str(),"r");

        if(fp == nullptr)
            return PTE_ERR_POPEN_FAIL;

        while(fgets(buffer.data(), buffer.size(), fp) != nullptr)
        {
            logs->append(buffer.data());

            std::string::size_type ind = 0;
            if( (ind = buffer.find("time=")) != std::string::npos )
            {
                for(size_t i=0; i<16; i++) std::cout << '\b';

                std::cout << buffer.substr(ind, 16)  << std::endl;            }
        }

        pclose(fp);

        return PTE_ERR_SUCCESS;
    }

    int get_average_ssim_from_logs(const std::string& logs,
                                   double* ssim)
    {
        std::string keyword = " All:";
        std::string::size_type ind = 0;
        if((ind = logs.find(keyword)) != std::string::npos)
            *ssim = std::stod(logs.substr(ind+keyword.size(),8));
        else
            return PTE_ERR_CANT_FIND_SSIM_SCORE_IN_LOGS;

        return PTE_ERR_SUCCESS;    }

    int generate_log_from_ssims(const std::vector<double>& ssims,
                                const std::vector<std::string>& video_names,
                                const std::string& path_to_log)
    {
        std::ofstream log_file(path_to_log.c_str());
        if(!log_file.is_open())
            return PTE_ERR_PATH_NOT_VALID;

        std::vector<double>::size_type ssims_count = ssims.size();

        for(std::vector<double>::size_type i = 0; i < ssims_count; i++)
            log_file << video_names[i] << ' ' << ssims[i] << '\n';

        log_file.close();
        return PTE_ERR_SUCCESS;
    }

}
