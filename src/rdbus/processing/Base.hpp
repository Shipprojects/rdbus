#pragma once

#include "rdbus/Data.hpp"
#include <list>
#include <memory>

namespace rdbus::processing
{

// Enum of all processor names
enum Name
{
    Read, // Not a real processor yet
    Limits
};

// After getting rdbus::Data it is then possible to process it as necessary by various implementations of this class,
// and then return type-erased data that can be serialized into JSON and outputted. So, when you implement your custom
// processor of rdbus::Data, you shall:
// - write an implementation of your processsor;
// - create your Base::Data structure that will be returned by the processor, and define it's to_json function;
// - update output module(-s), to output your serialized data (it is recommended to create separate output for each processor).
class Base
{
public:
    Base( Name name )
    : name( name )
    {
    }

    // A pure virtual class to type erase output of different processors
    struct Data
    {
        virtual const std::string& getDeviceName() const = 0;
        virtual nlohmann::json toJson() const = 0;

        virtual ~Data() = default;
    };

    using OutputList = std::list< std::shared_ptr< Base::Data > >;
    virtual OutputList process( const std::list< rdbus::Data >& ) = 0;

    virtual ~Base() = default;

    Name getName() const
    {
        return name;
    }

private:
    const Name name;
};

} // namespace rdbus::processing
