#include "pte_errors.h"

namespace pte
{
    void print_error(const int error_id)
    {
        if(error_id < static_cast<int>(error_names.size()))
            printf("err: %s\n",error_names[error_id].c_str());
        else
            printf("Erreur inconnu\n");
    }

    std::string get_libav_error_message(const int libav_error_id)
    {
        std::string buffer(265,0);
        int av_err = 0;
        av_err = av_strerror(libav_error_id,const_cast<char*>(buffer.data()),buffer.size());
        if(av_err) return nullptr;

        return buffer;
    }}
