#include "utility.hpp"
#include "Exception.hpp"

namespace rdbus::config::tools
{

void throwIf( bool condition, const std::string& message )
{
    if ( condition )
    {
        throw ParseException( message );
    }
}

} // namespace rdbus::config::tools
