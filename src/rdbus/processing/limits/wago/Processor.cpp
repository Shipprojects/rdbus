#include "Processor.hpp"
#include "rdbus/processing/limits/Data.hpp"
#include <iostream>

using namespace rdbus::config::wago;

namespace rdbus::processing::limits::wago
{

Processor::Processor( Name name, const std::list< config::wago::Module >& modules, const config::wago::Limits& limits )
: Base( name ),
  duration( limits.duration )
{
    // Generate map of modules which are defined in configuration
    for ( const auto& moduleName : limits.modules )
    {
        data.insert( { moduleName, {} } );
    }

    // Generate map of instances for each module in configuration
    for ( auto& [ moduleName, instanceValueMap ] : data )
    {
        for ( const auto& module : modules )
        {
            // If the module is not present in limits configuration
            if ( module.name != moduleName )
            {
                continue;
            }

            for ( const auto& instanceName : module.instances )
            {
                instanceValueMap.insert( { instanceName, {} } );
            }
        }
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
        insertNewValues( module, fields );

        auto output = generateOutput( module );
        outputList.push_back( std::move( output ) );
    }

    return outputList;
}

auto Processor::getLatestTime( const rdbus::Data& input ) -> Timepoint
{
    // The fields here must not be empty, otherwise this part will cause Undefined Behavior!
    return input.fields.front().timestamp;
}

void Processor::removeOldValues( const ModuleName& module, const Timepoint& currentTime )
{
    std::cout << "removeOldValues" << std::endl;
    auto& instanceMap = data.at( module );

    for ( auto& [ instanceName, values ] : instanceMap )
    {
        std::cout << instanceName << " " << values.size() << std::endl;
        auto it = std::remove_if( values.begin(), values.end(),
                                  [ & ]( const TimestampedValue& tv )
                                  { return ( tv.first + duration ) < currentTime; } );
        values.erase( it, values.end() );
        std::cout << instanceName << " " << values.size() << std::endl;
    }
}

void Processor::insertNewValues( const ModuleName& module, const std::list< rdbus::Data::Field >& fields )
{
    for ( const auto& field : fields )
    {
        data.at( module ).at( field.name ).emplace_back( field.timestamp, std::get< int16_t >( field.value ) );
    }
}

std::shared_ptr< limits::Data > Processor::generateOutput( const ModuleName& module )
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

} // namespace rdbus::processing::limits::wago