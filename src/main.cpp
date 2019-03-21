#include "pte_utils.h"

using namespace pte;

int main(int argc, char** argv)
{
    std::vector<double> encoded_ssims;
    int pte_err = 0;

    pte_err = compute_ssims_from_different_encodings(&encoded_ssims,argv[1],argv[2]);
    print_error(pte_err);
    if(pte_err) return pte_err;

    return 0;
}
