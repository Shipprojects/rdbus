#pragma once

#include "rdbus/Data.hpp"
#include "rdbus/config/processors/Limits.hpp"
#include "rdbus/processing/Processor.hpp"
#include "rdbus/processing/limits/Data.hpp"
#include <chrono>

namespace rdbus::processing::limits::wago
{

// A processor that outputs minmax values of all selected rdbus::Data field values which came
// in a timespan of last 'duration' minutes
// NOTE! The processor is currently specialized to work with incoming Wago data only. In theory,
// it is possible to make this class a generic processor and specialize it for each protocol
// by inheriting this class and implementing a custom constructor for that specialization.
// The Processor currently supports data of type int16_t only.
class Processor : public processing::Processor
{
public:
    Processor( const config::processors::Limits& );
    Processor::OutputList process( const std::list< rdbus::Data >& ) override;

private:
    using DeviceName = std::string;
    using FieldName = std::string;
    using Timepoint = std::chrono::time_point< std::chrono::system_clock >;
    using TimestampedValue = std::pair< Timepoint, rdbus::Data::Field::Variant >;
    using TimestampedFieldValues = std::list< TimestampedValue >;
    std::map< DeviceName, std::map< FieldName, TimestampedFieldValues > > data;

    // This function is a bit of a hack to not to bring std time functions into this class
    // for the sake of simplicity. This function takes the timestamp of the first field
    // found in passed rdbus::Data::fields.
    // The time precision is good enough for this case, as all rdbus::Data fields in a
    // single reading will have time deviations of at most some milliseconds.
    Timepoint getLatestTime( const rdbus::Data& );

    void removeOldValues( const DeviceName& module, const Timepoint& currentTime );
    void addMissingFields( const DeviceName& module, const std::list< rdbus::Data::Field >& fields );
    void insertNewValues( const DeviceName& module, const std::list< rdbus::Data::Field >& fields );
    std::shared_ptr< limits::Data > generateOutput( const DeviceName& module );

    // How long to store the values for
    const std::chrono::minutes duration;
};

} // namespace rdbus::processing::limits::wago
