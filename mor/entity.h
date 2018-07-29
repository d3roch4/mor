#ifndef ENTITY_H
#define ENTITY_H

#include "field.h"
#include "descfield.h"
#include <typeindex>
#include "ientity.h"

namespace mor {

using namespace std;

string tolower_str(string&& str);

const char* remove_prefix_name(const char* str);


struct ForeignKey{
    iField* field;
    string reference;
};

template<class type>
struct Entity : protected iEntity
{
    static string _entity_name;
    static vector<DescField> _desc_fields;
    vector<shared_ptr<iField>> _fields;
    vector<ForeignKey> _vecFK;

    Entity(const string& table = tolower_str(string( remove_prefix_name(typeid(type).name()) )) ){
        _entity_name = table;
    }

    Entity(Entity&& hrs){
        operator =(hrs);
    }

    Entity(const Entity& hrs){
        operator =(hrs);
    }

    inline Entity& operator =(const Entity& hrs){
        type* objThis = (type*)this;
        type* objHrs =  (type*)&hrs;

        this->_fields.clear();
        for(auto&& col: hrs._fields){
            uintptr_t pcol = reinterpret_cast<uintptr_t>(col->value);
            uintptr_t phrs = reinterpret_cast<uintptr_t>(objHrs);
            uintptr_t pthis = reinterpret_cast<uintptr_t>(objThis);
            void* pvalue = reinterpret_cast<void*>((pcol-phrs)+pthis);

            this->_fields.emplace_back(col->copy());
            auto&& copy = _fields.back();
            copy->value = pvalue;
        }
        return *objThis;
    }

protected:
    iField* field_i(const shared_ptr<iField>& field, string name, shared_ptr<type_index> typeinfo, unordered_map<string, string> options = {} ){
        _fields.emplace_back(field);
        auto&& fld = _fields.back();

        if(_fields.size() > _desc_fields.size()){
            DescField desc{name, options, typeinfo};
            _desc_fields.emplace_back(desc);
        }

        return fld.get();
    }

    template<class tVar>
    iField* field(const tVar& var, string name, unordered_map<string, string> options = {} ){
        shared_ptr<iField> f{ new Field<tVar>( const_cast<tVar&>(var) ) };
        shared_ptr<type_index> typeinfo{new type_index(typeid(var))};

        field_i(f, name, typeinfo, options);
    }

    void foreignKey(iField* field, string reference){
        _vecFK.emplace_back(ForeignKey{field, reference});
    }

    vector<DescField> _get_desc_fields(){
        return _desc_fields;
    }
    vector<shared_ptr<iField>> _get_fields(){
        return _fields;
    }
};

template<class type>
string Entity<type>::_entity_name;
template<class type>
vector<DescField> Entity<type>::_desc_fields;

}
#endif // ORM_H

//template<class tVar>
//iField* field_e(Entity<tVar>& entity, string name, unordered_map<string, string> options = {} ){
//    auto it = options.find("field");
//    if(it!=options.end())
//    {
//        for(int i=0; i<entity._desc_fields.size(); i++)
//            if(entity._desc_fields[i].name == it->second)
//                return field_i(entity._fields[i]->copy(), name, entity._desc_fields[i].typeinfo, options);

//        throw runtime_error("field: '"+it->second+"', not found in "+entity._entity_name);
//    }
//    else
//        throw runtime_error("Entity: option: 'field' in '"+this->_entity_name+"."+name+"' not found");
//}

