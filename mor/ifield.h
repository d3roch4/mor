#ifndef IFIELD_H
#define IFIELD_H
#include <string>
#include <memory>

namespace mor
{

struct iField
{
    void* value;

    virtual std::string getValue(const DescField& desc) const = 0;
    virtual void setValue(const char* value, const DescField& desc) = 0;
    virtual bool isNull() const = 0;
    virtual std::unique_ptr<iField> copy() = 0;
};

}
#endif // IFIELD_H
