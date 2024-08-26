#include "Listen.hpp"

namespace rdbus::tasks::nmea
{

Listen::Listen( const config::nmea::NMEA& nmea, const Communicator& communicator )
: nmea( nmea ),
  com( communicator )
{
}

std::list< Data > Listen::run()
{
    return com->receive( nmea );
}

} // namespace rdbus::tasks::nmea
