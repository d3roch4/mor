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

iField *field_i(iEntity *entity, const shared_ptr<iField> &field, const string &name, shared_ptr<type_index> typeinfo, unordered_map<string, string>&& options){

    vector<DescField>& _desc_fields = entity->_get_desc_fields();
    vector<shared_ptr<iField>>& _fields = entity->_get_fields();

    _fields.emplace_back(field);
    auto&& fld = _fields.back();

    if(_fields.size() > _desc_fields.size()){
        DescField desc{name, options, typeinfo};
        _desc_fields.emplace_back(desc);
    }

    return fld.get();
}



}
