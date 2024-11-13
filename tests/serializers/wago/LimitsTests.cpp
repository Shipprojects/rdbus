#include "rdbus/config/Exception.hpp"
#include "rdbus/config/wago/Limits.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/wago/limits/";
using namespace nlohmann;
using namespace rdbus::config::wago;
using namespace rdbus;

TEST( TestLimits, Valid )
{
    const auto path = testFilePath + "valid.json";

    const auto jsonIn = getJsonFromPath( path );
    const Limits limits = jsonIn;

    EXPECT_EQ( limits.duration, Limits::Minutes( 1000 ) );
    EXPECT_EQ( limits.modules.size(), 2 );
    EXPECT_EQ( *limits.modules.begin(), "Sensors" );
    EXPECT_EQ( *std::next( limits.modules.begin() ), "More sensors" );
}

TEST( TestLimits, EmptyModules )
{
    const auto path = testFilePath + "empty_modules.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const Limits limits = jsonIn;
    },
                  config::ParseException );
}

TEST( TestLimits, NoModules )
{
    const auto path = testFilePath + "no_modules.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const Limits limits = jsonIn;
    },
                  config::ParseException );
}

TEST( TestLimits, NoPollTimeMs )
{
    const auto path = testFilePath + "no_storage_duration_minutes.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const Limits limits = jsonIn;
    },
                  config::ParseException );
}
