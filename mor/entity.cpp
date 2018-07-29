#include "entity.h"


namespace mor {

string tolower_str(string&& str){
    for(int i=0; i<str.size(); i++)
        str[i] = tolower(str[i]);

    return str;
}

const char* remove_prefix_name(const char *str)
{
    while(str[0] < 'A' || str[0] > 'z')
        str++;
    return str;
}

}
