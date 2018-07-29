#ifndef IENTITY_H
#define IENTITY_H
#include <vector>
#include <memory>
#include "descfield.h"
#include "ifield.h"

namespace mor
{

struct iEntity{
    virtual std::vector<DescField> _get_desc_fields()=0;
    virtual std::vector<std::shared_ptr<iField>> _get_fields()=0;
};

}
#endif // IENTITY_H
