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

template<typename type>
inline bool is_zero_or_empty(const type& number)
{
    return number==0;
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

    string getValue() const {
        stringstream ss;
        ss << (*(type*)value);
        return ss.str();
    }

    void setValue(const char* str){
        if(str == NULL)
            return;
        stringstream ss(str);
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

    string getValue() const {
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
