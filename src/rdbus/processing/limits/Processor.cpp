#include "Processor.hpp"
#include "rdbus/processing/limits/Data.hpp"

namespace rdbus::processing::limits
{

Processor::Processor( const config::processors::Limits& limits )
: processing::Processor( Name::Limits ),
  duration( limits.duration )
{
    // Generate a map of enabled devices for which to process data
    for ( const auto& moduleName : limits.devices )
    {
        data.insert( { moduleName, {} } );
    }
}

auto Processor::process( const std::list< rdbus::Data >& inputList ) -> OutputList
{
    OutputList outputList;

    for ( const auto& input : inputList )
    {
        const auto& module = input.deviceName;
        // If given module is not configured for limit processing
        if ( data.find( module ) == data.end() )
        {
            continue;
        }

        removeOldValues( module, getLatestTime( input ) );

        const auto& fields = input.fields;
        addMissingFields( module, fields );
        insertNewValues( module, fields );

        auto output = generateOutput( module );
        outputList.push_back( std::move( output ) );
    }

    return outputList;
}

// If we encounter the field for the first time, we first have to prepare our device-field map
void Processor::addMissingFields( const DeviceName& module, const std::list< rdbus::Data::Field >& fields )
{
    for ( const auto& field : fields )
    {
        // The std::map::insert adds the key only when the key did not exist yet
        data.at( module ).insert( { field.name, {} } );
    }
}

auto Processor::getLatestTime( const rdbus::Data& input ) -> Timepoint
{
    // The fields here must not be empty, otherwise this part will cause Undefined Behavior!
    return input.fields.front().timestamp;
}

void Processor::removeOldValues( const DeviceName& module, const Timepoint& currentTime )
{
    auto& instanceMap = data.at( module );

    for ( auto& [ instanceName, values ] : instanceMap )
    {
        auto it = std::remove_if( values.begin(), values.end(),
                                  [ & ]( const TimestampedValue& tv )
                                  { return ( tv.first + duration ) < currentTime; } );
        values.erase( it, values.end() );
    }
}

void Processor::insertNewValues( const DeviceName& module, const std::list< rdbus::Data::Field >& fields )
{
    for ( const auto& field : fields )
    {
        // We cannot process valueless fields
        if ( field.value.has_value() )
        {
            data.at( module ).at( field.name ).emplace_back( field.timestamp, field.value.value() );
        }
    }
}

std::shared_ptr< limits::Data > Processor::generateOutput( const DeviceName& module )
{
    auto output = std::make_shared< limits::Data >();
    output->deviceName = module;

    for ( const auto& [ instanceName, values ] : data.at( module ) )
    {
        limits::Data::FieldLimit entry;
        entry.name = instanceName;
        if ( !values.empty() )
        {
            const auto minmax = std::minmax_element( values.begin(), values.end(),
                                                     []( const TimestampedValue& left, const TimestampedValue& right )
                                                     { return left.second < right.second; } );
            entry.min = minmax.first->second;
            entry.max = minmax.second->second;
        }
        output->instanceLimits.emplace_back( std::move( entry ) );
    }

    return output;
}

} // namespace rdbus::processing::limits
