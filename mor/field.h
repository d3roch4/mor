#ifndef FIELD_H
#define FIELD_H

#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <d3util/stacktrace.h>
#include "descfield.h"
#include "ientity.h"
#include "ifield.h"

namespace mor {

using namespace std;

struct colon_is_space : std::ctype<char> {
  colon_is_space() : std::ctype<char>(get_table()) {}
  static mask const* get_table()
  {
    static mask rc[table_size];
    rc['\0'] = std::ctype_base::space;
    return &rc[0];
  }
};
static colon_is_space* is_space = new colon_is_space;
static locale delimit{std::cin.getloc(), is_space};

template<typename type>
inline bool is_zero_or_empty(const type& number)
{
    return number==0;
}

template<typename type>
inline bool is_zero_or_empty(const type* ref)
{
    return ref==0;
}

template<>
inline bool is_zero_or_empty(const string& str)
{
    return str.empty();
}

template<class type>
struct Field : iField
{

    Field(type& data) {
        value = &data;
    }

    string getValue(const DescField& desc) const {
        stringstream ss;
        ss << (*(type*)value);
        return ss.str();
    }

    void setValue(const char* str, const DescField& desc){
        if(str == NULL)
            return;
        stringstream ss(str);
        ss.imbue(delimit);
        ss >> *(type*)value;
    }

    bool isNull() const{
       return is_zero_or_empty(*(type*)value);
    }

    unique_ptr<iField> copy(){
        Field<type>* p = new Field<type>(*(type*)value);
        *p = *this;
        return unique_ptr<iField>(p);
    }
};

template<typename T>
struct Field<vector<T>> : iField
{
    Field(vector<T>& vector){
        value = &vector;
    }

    string getValue(const DescField& desc) const {
        stringstream ss;
        for(T i: *((vector<T>*)value)){
            if(ss.str().size())
                ss << ',';
            ss << i;
        }
        return ss.str();
    }
    void setValue(const char* str){
        if(str == NULL)
            return;

        char *token = std::strtok((char*)str, ",");
        while (token != NULL) {
            ((vector<int>*)value)->emplace_back(stoi(token));
            token = std::strtok(NULL, ",");
        }
    }
    bool isNull() const{
        return ((vector<T>*)value)->empty();
    }
    unique_ptr<iField> copy(){
        Field<vector<T>>* p = new Field<vector<T>>(*(vector<T>*)value);
        *p = *this;
        return unique_ptr<iField>(p);
    }
};

typedef chrono::time_point<std::chrono::system_clock> date_time;
template<>
struct Field<date_time> : iField
{

    Field(date_time& date) {
        value = &date;
    }

    string getValue(const DescField& desc) const {
        std::time_t tp= chrono::system_clock::to_time_t( *(date_time*)value );
        return std::asctime(std::gmtime(&tp));
    }
    void setValue(const char* str, const DescField& desc){
        if(str == NULL)
            return;

        struct std::tm tm;
        std::istringstream ss(str);
        ss >> std::get_time(&tm, "%Y-%m-%d %H-%M-%S"); // or just %T in this case
        std::time_t time = mktime(&tm);
        *(date_time*)value = chrono::system_clock::from_time_t(time);
    }
    bool isNull() const {
        return *(date_time*)value==chrono::time_point<std::chrono::system_clock>{};
    }
    unique_ptr<iField> copy(){
        Field<date_time>* p = new Field<date_time>(*(date_time*)value);
        *p = *this;
        return unique_ptr<iField>(p);
    }
};


template<typename T>
class Entity;
template<typename Object>
struct Field<Object*> : iField
{
    Field(Entity<Object>* entity) {
        value = entity;
    }

    string getValue(const DescField& desc) const {
        iEntity* ptr = (iEntity*) value;
        const vector<DescField>& descsChild = ptr->_get_desc_fields();
        const vector<shared_ptr<iField>>& fieldsChild = ptr->_get_fields();
        const string& refName = desc.options.find("field")->second;
        for(int i=0; i<descsChild.size(); i++)
            if(descsChild[i].name == refName)
                return fieldsChild[i]->getValue(descsChild[i]);

        return "Entity: "+Entity<Object>::_entity_name;
    }
    void setValue(const char* str, const DescField& desc){
        iEntity* ptr = (iEntity*) value;
        const vector<DescField>& descsChild = ptr->_get_desc_fields();
        const vector<shared_ptr<iField>>& fieldsChild = ptr->_get_fields();
        const string& refName = desc.options.find("field")->second;
        for(int i=0; i<descsChild.size(); i++)
            if(descsChild[i].name == refName)
                return fieldsChild[i]->setValue(str, descsChild[i]);

        throw_with_trace(runtime_error("Type "+Entity<Object>::_entity_name+", not set with string"));
    }
    bool isNull() const {
        return false;
    }
    unique_ptr<iField> copy(){
        Field<Object*>* p = new Field<Object*>((Object*)value);
//        *p = *this;
        return unique_ptr<iField>(p);
    }
};


//template<class T>
//void operator << (T& value, iField* field)
//{
//    switch (field->typeinfo) {
//    case typeid(int):
//        value = *((int*)field->value);
//        break;
//    case typeid(double):
//        value = *((double*)field->value);
//        break;
//    case typeid(string):
//        value = *((string*)field->value);
//        break;
//    default:
//        break;
//    }
//}

}
#endif // FIELD_H
