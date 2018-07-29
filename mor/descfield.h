#ifndef DESCFIELD_H
#define DESCFIELD_H

#include <typeinfo>
#include <typeindex>
#include <memory>
#include <string>
#include <unordered_map>

namespace mor {
using namespace std;

struct DescField
{
    string name;
    unordered_map<string, string> options;
    shared_ptr<type_index> typeinfo;
};

}
#endif // DESCFIELD_H
