#ifndef FIELD_H
#define FIELD_H

#include <iostream>
#include <cstring>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <memory>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <unordered_map>

using namespace std;

struct iField
{
    string name;
    unordered_map<string, string> options;
    shared_ptr<type_index> typeinfo;
    void* value;

    void setProperties(const string& name, unordered_map<string, string>& options);

    template<typename type>
    void getValue(type&& value)
    {
        value = *(type*)this->value;
    }

    virtual string getValue() const = 0;
    virtual void setValue(const char* value) = 0;
    virtual bool isNull() const = 0;
    virtual unique_ptr<iField> copy() = 0;
};

template<class type>
struct Field : iField
{

    Field(type& data) {
        value = &data;
    }

    string getValue() const {
        return to_string(*(type*)value);
    }

    void setValue(const char* str){
        if(str == NULL)
            return;
        stringstream ss(str);
        ss >> *(type*)value;
    }

    bool isNull() const{
        return *(type*)value==0;
    }

    unique_ptr<iField> copy(){
        Field<type>* p = new Field<type>(*(type*)value);
        *p = *this;
        return unique_ptr<iField>(p);
    }
};

template<>
struct Field<string> : iField
{

    Field(string& str){
        value = &str;
    }

    string getValue() const {
        return *(string*)value;
    }
    void setValue(const char* str){
        if(str != NULL)
            *(string*)value = str;
    }
    bool isNull() const{
        return ((string*)value)->empty();
    }
    unique_ptr<iField> copy(){
        Field<string>* p = new Field<string>(*(string*)value);
        *p = *this;
        return unique_ptr<iField>(p);
    }
};

template<>
struct Field<vector<int>> : iField
{
    Field(vector<int>& vector){
        value = &vector;
    }

    string getValue() const {
        string str;
        for(int i: *((vector<int>*)value)){
            if(str.size())
                str+=',';
            str+= to_string(i);
        }
        return str;
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
        return ((vector<int>*)value)->empty();
    }
    unique_ptr<iField> copy(){
        Field<vector<int>>* p = new Field<vector<int>>(*(vector<int>*)value);
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

    string getValue() const {
        std::time_t tp= chrono::system_clock::to_time_t( *(date_time*)value );
        return std::asctime(std::gmtime(&tp));
    }
    void setValue(const char* str){
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


#endif // FIELD_H
