#pragma once

#include <stdexcept>

namespace rdbus
{

// This is main rdbus exception. All rdbus related exceptions inherit this exception.
// If such exception gets thrown internally in rdbus, then it can be processed
// there in any scope (either in the place where it was thrown or higher). However, if
// any std exception gets thrown and is not catched immediately in the same scope and
// converted to rdbus exception, then it is considered critical and shall make the program exit.
struct Exception : public std::runtime_error
{
    Exception( const std::string& what );
};

void throwIf( bool condition, const std::string& message );

} // namespace rdbus
