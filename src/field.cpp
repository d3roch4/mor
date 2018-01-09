#include "field.h"



void iField::setProperties(const string& name, unordered_map<string, string>& options)
{
    this->name = name;
    this->options.swap(options);
}
