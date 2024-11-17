#include "Exception.hpp"

namespace rdbus
{

Exception::Exception( const std::string& what )
: std::runtime_error( what )
{
}

void throwIf( bool condition, const std::string& message )
{
    if ( condition )
    {
        throw Exception( message );
    }
}

} // namespace rdbus
