#ifndef CONVERT_MAP_H
#define CONVERT_MAP_H
#include <mor/mor.h>
#include <map>
#include <sstream>
#include <boost/algorithm/string.hpp>

struct IgnoreData {};

struct SpaceReplace{
    std::string space;
    SpaceReplace(std::string s) : space{s} {}
};

struct obj_to_map
{
    std::map<std::string, std::string> data;
    template<class FieldData, class Annotations>
    void operator()(FieldData f, Annotations e, int cont)
    {
        IgnoreData* a = typename Annotations::get_field(f.name());
        if(a == nullptr){
            std::stringstream ss;
            ss.imbue(delimit_endl);
            ss << f.get();
            std::string value = ss.str();
            SpaceReplace* s = typename Annotations::get_field(f.name());
            if(s)
                boost::replace_all(value, " ", s->space);
            data[f.name()] = value;
        }
    }
};

template<class T>
std::map<std::string, std::string> to_map(T & x)
{
    obj_to_map om;
    reflector::visit_each(x, om);
    return om.data;
}

struct from_map
{
    const std::map<std::string, std::string>& data;
    from_map(const std::map<std::string, std::string>& data) :
        data{data}
    {
    }

    template<class FieldData, class Annotations>
    void operator()(FieldData f, Annotations e, int cont)
    {
        std::stringstream ss;
        ss.imbue(delimit_endl);
        auto it = data.find(f.name());
        if(it!=data.end()){
            ss << it->second;
            ss >> f.get();
        }
    }

    template <typename T>
    operator T () {
        T ret{};
        reflector::visit_each(ret, *this);
        return ret;
    }
};

#endif // CONVERT_MAP_H
