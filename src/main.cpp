#include "pte_utils.h"

using namespace pte;

int main(int argc, char** argv)
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

    return 0;
}
