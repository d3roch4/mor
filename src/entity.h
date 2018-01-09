#ifndef ENTITY_H
#define ENTITY_H

#include "field.h"
#include <typeindex>

using namespace std;

string tolower_str(string&& str);

const char* remove_prefix_name(const char* str);


struct ForeignKey{
    iField* field;
    string reference;
};

template<class type>
struct Entity
{
    static string _entity_name;
    vector<unique_ptr<iField>> _fields;  //  coluna: name, type
    vector<ForeignKey> _vecFK;

    Entity(const string& table = tolower_str(string( remove_prefix_name(typeid(type).name()) )) ){
        _entity_name = table;
    }

    Entity(Entity&& hrs){}
    Entity(const Entity& hrs){
        type* objThis = (type*)this;
        type * objHrs = (type*)&hrs;

        for(auto&& col: hrs._fields){
            uintptr_t pcol = reinterpret_cast<uintptr_t>(col->value);
            uintptr_t phrs = reinterpret_cast<uintptr_t>(objHrs);
            uintptr_t pthis = reinterpret_cast<uintptr_t>(objThis);
            void* pvalue = reinterpret_cast<void*>((pcol-phrs)+pthis);

            this->_fields.emplace_back(col->copy());
            auto&& copy = _fields.back();
            copy->value = pvalue;
        }

    }
    void operator =(const Entity& hrs){}

    template<class tVar>
    iField* field(tVar& var, string name, unordered_map<string, string> options = {} ){
        _fields.emplace_back(new Field<tVar>(var));
        auto&& col = _fields.back();
        col->typeinfo.reset(new type_index(typeid(var)));
        col->setProperties(name, options);
        return col.get();
    }

    void foreignKey(iField* field, string reference){
        _vecFK.emplace_back(ForeignKey{field, reference});
    }

};

template<class type>
string Entity<type>::_entity_name;

#endif // ORM_H
