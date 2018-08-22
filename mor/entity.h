#ifndef ENTITY_H
#define ENTITY_H

#include "field.h"
#include "descfield.h"
#include <typeindex>
#include "ientity.h"
#include <boost/any.hpp>

namespace mor {

using namespace std;

string tolower_str(string&& str);

const char* remove_prefix_name(const char* str);

iField* field_i(iEntity* entity, const shared_ptr<iField>& field, const string& name, shared_ptr<type_index> typeinfo, unordered_map<string, string>&& options = {} );

template<class type>
struct Entity : public iEntity
{
    static std::unordered_map<string, boost::any> _atrributes;
    static string _entity_name;
    static vector<DescField> _desc_fields;
    vector<shared_ptr<iField>> _fields;

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

    template<class tVar>
    iField* field(const tVar& var, const string& name, unordered_map<string, string>&& options = {} ){
        shared_ptr<iField> f{ new Field<tVar>( const_cast<tVar&>(var) ) };
        shared_ptr<type_index> typeinfo{new type_index(typeid(var))};

        field_i(this, f, name, typeinfo, std::move(options));
    }

    vector<DescField>& _get_desc_fields(){
        return _desc_fields;
    }
    vector<shared_ptr<iField>>& _get_fields(){
        return _fields;
    }
    std::string _get_name(){
        return _entity_name;
    }
    std::unordered_map<string, boost::any>& _get_atrributes(){
        return _atrributes;
    }
};

template<class type>
string Entity<type>::_entity_name;
template<class type>
vector<DescField> Entity<type>::_desc_fields;
template<class type>
std::unordered_map<string, boost::any> Entity<type>::_atrributes;

}
#endif // ORM_H
