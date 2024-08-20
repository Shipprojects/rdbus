#include "rdbus/out/http/Sessioner.hpp"
#include <chrono>
#include <gtest/gtest.h>

using namespace rdbus::out::http;

TEST( TestSessioner, TestAdd )
{
    Sessioner sessioner;

    {
        const int id = sessioner.create();
        EXPECT_EQ( id, 1 );
    }

    {
        const int id = sessioner.create();
        EXPECT_EQ( id, 2 );
    }

    {
        const int id = sessioner.create();
        EXPECT_EQ( id, 3 );
    }
}

TEST( TestSessioner, TestGet )
{
    Sessioner sessioner;

    const auto creationTime = std::chrono::system_clock::now();
    sessioner.create( creationTime );
    sessioner.create( creationTime );

    auto accessTime = std::chrono::system_clock::now();
    auto lastAccess = sessioner.getLastAccessTime( 1, accessTime );
    EXPECT_EQ( lastAccess, creationTime );

    accessTime = std::chrono::system_clock::now();
    lastAccess = sessioner.getLastAccessTime( 2, accessTime );
    EXPECT_EQ( lastAccess, creationTime );

    lastAccess = sessioner.getLastAccessTime( 2, std::chrono::system_clock::now() );
    EXPECT_EQ( lastAccess, accessTime );
}

TEST( TestSessioner, TestGetTimeout )
{
    Sessioner sessioner;
    sessioner.create();

    sessioner.getLastAccessTime( 1, std::chrono::system_clock::now() - Sessioner::inactivityTimeout );

    // The session expired and there is no such cookie anymore
    {
        EXPECT_THROW( {
            sessioner.getLastAccessTime( 1, std::chrono::system_clock::now() );
        },
                      Sessioner::Exception );
    }
}
