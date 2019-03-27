#include "pte_utils.h"

using namespace pte;

int main(int argc, char** argv)
{

    auto compute_ssim_from_canal_encoded_videos = [argc,argv]()
    {
        std::vector<double> encoded_ssims;
        std::vector<std::string> video_names;
        int pte_err = 0;

        pte_err = compute_ssims_from_different_encodings(argv[1],argv[2],&encoded_ssims,&video_names);
        print_error(pte_err);
        if(pte_err) return pte_err;

        if(argc >= 4)
        {
            pte_err = generate_log_from_ssims(encoded_ssims,video_names,argv[3]);
            print_error(pte_err);
        }
    };

    fs::path kjob_path;
    int pte_err = generate_canal_kjob(argv[1], &kjob_path);
    print_error(pte_err);
    if(pte_err) return pte_err;

    std::cout << "KJOB PATH " << kjob_path << std::endl;
    return 0;
}
