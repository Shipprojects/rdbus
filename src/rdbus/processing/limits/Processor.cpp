#include "Processor.hpp"
#include "rdbus/processing/limits/Data.hpp"
#include <spdlog/spdlog.h>

namespace rdbus::processing::limits
{

Processor::Processor( const config::processors::Limits& limits )
: processing::Processor( Name::Limits ),
  duration( limits.duration )
{
    // Generate a map of enabled devices for which to process data
    for ( const auto& deviceName : limits.devices )
    {
        data.insert( { deviceName, {} } );
    }
}

auto Processor::process( const std::list< rdbus::Data >& inputList ) -> OutputList
{
    OutputList outputList;

    for ( const auto& input : inputList )
    {
        // Do not process errorous data
        if ( input.error.has_value() )
        {
            continue;
        }

        const auto& device = input.deviceName;
        // If given module is not configured for limit processing
        if ( data.find( device ) == data.end() )
        {
            continue;
        }

        try
        {
            removeOldValues( device, getLatestTime( input ) );

            const auto& fields = input.fields;
            addMissingFields( device, fields );
            insertNewValues( device, fields );

            auto output = generateOutput( device );
            outputList.push_back( std::move( output ) );
        }
        catch ( const std::exception& e )
        {
            SPDLOG_ERROR( e.what() );
            outputList.clear();
            break;
        }
    }

    return outputList;
}

// If we encounter the field for the first time, we first have to prepare our device-field map
void Processor::addMissingFields( const DeviceName& device, const std::list< rdbus::Data::Field >& fields )
{
    for ( const auto& field : fields )
    {
        // The std::map::insert adds the key only when the key did not exist yet
        data.at( device ).insert( { field.name, {} } );
    }
}

auto Processor::getLatestTime( const rdbus::Data& input ) -> Timepoint
{
    // The fields here must not be empty, otherwise this part will cause Undefined Behavior!
    return input.fields.front().timestamp;
}

void Processor::removeOldValues( const DeviceName& device, const Timepoint& currentTime )
{
    auto& fieldsMap = data.at( device );

    for ( auto& [ fieldName, values ] : fieldsMap )
    {
        auto it = std::remove_if( values.begin(), values.end(),
                                  [ & ]( const TimestampedValue& tv )
                                  { return ( tv.first + duration ) < currentTime; } );
        values.erase( it, values.end() );
    }
}

void Processor::insertNewValues( const DeviceName& device, const std::list< rdbus::Data::Field >& fields )
{
    for ( const auto& field : fields )
    {
        // We cannot process valueless fields
        if ( field.value.has_value() )
        {
            data.at( device ).at( field.name ).emplace_back( field.timestamp, field.value.value() );
        }
    }
}

std::shared_ptr< limits::Data > Processor::generateOutput( const DeviceName& device )
{
    auto output = std::make_shared< limits::Data >();
    output->deviceName = device;

    for ( const auto& [ fieldName, values ] : data.at( device ) )
    {
        limits::Data::FieldLimit entry;
        entry.name = fieldName;
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
