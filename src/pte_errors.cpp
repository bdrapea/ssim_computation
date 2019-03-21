#include "pte_errors.h"

namespace pte
{
    void print_error(const int error_id)
    {
        if(error_id < error_names.size())
            printf("err: %s\n",error_names[error_id].c_str());
        else
            printf("Erreur inconnu");
    }}
