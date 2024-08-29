#include "rdbus/communication/nmea/Response.hpp"
#include <gtest/gtest.h>

using namespace rdbus::communication::nmea;

TEST( TestResponse, TestWithoutChecksum )
{
    const std::string representation = "$VDSDDBK,1330.5,f,0405.5,M,0221.6,F*2E\r\n";
    const std::vector< uint8_t > data( representation.begin(), representation.end() );

    const Response response( data );

    EXPECT_EQ( response.getTalkerID(), "VD" );
    EXPECT_EQ( response.getSentenceID(), "SDDBK" );
    ASSERT_EQ( response.getFields().size(), 6 );
    EXPECT_EQ( response.getFields()[ 0 ], "1330.5" );
    EXPECT_EQ( response.getFields()[ 1 ], "f" );
    EXPECT_EQ( response.getFields()[ 2 ], "0405.5" );
    EXPECT_EQ( response.getFields()[ 3 ], "M" );
    EXPECT_EQ( response.getFields()[ 4 ], "0221.6" );
    EXPECT_EQ( response.getFields()[ 5 ], "F" );
}

TEST( TestResponse, TestWithChecksum )
{
    const std::string representation = "$VDSDDBK,1330.5,f,0405.5,M,0221.6*56\r\n";
    const std::vector< uint8_t > data( representation.begin(), representation.end() );

    const Response response( data, true );

    EXPECT_EQ( response.getTalkerID(), "VD" );
    EXPECT_EQ( response.getSentenceID(), "SDDBK" );
    ASSERT_EQ( response.getFields().size(), 5 );
    EXPECT_EQ( response.getFields()[ 0 ], "1330.5" );
    EXPECT_EQ( response.getFields()[ 1 ], "f" );
    EXPECT_EQ( response.getFields()[ 2 ], "0405.5" );
    EXPECT_EQ( response.getFields()[ 3 ], "M" );
    EXPECT_EQ( response.getFields()[ 4 ], "0221.6" );
}

TEST( TestResponse, TestWithIncorrectChecksum )
{
    const std::string representation = "$VDSDDBK,1330.5,f,0405.5,M,0221.6,F*2E\r\n";
    const std::vector< uint8_t > data( representation.begin(), representation.end() );

    EXPECT_THROW( {
        const Response response( data, true );
    },
                  Response::Exception );
}

TEST( TestResponse, TestMissingRequiredChecksum )
{
    const std::string representation = "$VDVBW,55.42,,V,424.33,2321.22,V,,V,1001.23\r\n";
    const std::vector< uint8_t > data( representation.begin(), representation.end() );

    EXPECT_THROW( {
        const Response response( data, true );
    },
                  Response::Exception );
}

TEST( TestResponse, TestMissingUnrequiredChecksum )
{
    const std::string representation = "$VDVBW,55.42,,V,424.33,2321.22,V,,V,1001.23\r\n";
    const std::vector< uint8_t > data( representation.begin(), representation.end() );

    const Response response( data );

    EXPECT_EQ( response.getTalkerID(), "VD" );
    EXPECT_EQ( response.getSentenceID(), "VBW" );
    ASSERT_EQ( response.getFields().size(), 9 );
    EXPECT_EQ( response.getFields()[ 0 ], "55.42" );
    EXPECT_EQ( response.getFields()[ 1 ], "" );
    EXPECT_EQ( response.getFields()[ 2 ], "V" );
    EXPECT_EQ( response.getFields()[ 3 ], "424.33" );
    EXPECT_EQ( response.getFields()[ 4 ], "2321.22" );
    EXPECT_EQ( response.getFields()[ 5 ], "V" );
    EXPECT_EQ( response.getFields()[ 6 ], "" );
    EXPECT_EQ( response.getFields()[ 7 ], "V" );
    EXPECT_EQ( response.getFields()[ 8 ], "1001.23" );
}

TEST( TestResponse, TestInvalidSyntax )
{
    const std::string representation = "$VDVHW,55,T,33,M,22,N,11,K**h22\r\n";
    const std::vector< uint8_t > data( representation.begin(), representation.end() );

    EXPECT_THROW( {
        const Response response( data, true );
    },
                  Response::Exception );
}

TEST( TestResponse, TestSingleField )
{
    const std::string representation = "$VDTEST,text*2E\r\n";
    const std::vector< uint8_t > data( representation.begin(), representation.end() );

    const Response response( data );

    EXPECT_EQ( response.getTalkerID(), "VD" );
    EXPECT_EQ( response.getSentenceID(), "TEST" );
    ASSERT_EQ( response.getFields().size(), 1 );
    EXPECT_EQ( response.getFields()[ 0 ], "text" );
}

TEST( TestResponse, TestEmptyFields )
{
    const std::string representation = "$VDSDMTW,*45\r\n";
    const std::vector< uint8_t > data( representation.begin(), representation.end() );

    const Response response( data );

    EXPECT_EQ( response.getTalkerID(), "VD" );
    EXPECT_EQ( response.getSentenceID(), "SDMTW" );
    ASSERT_EQ( response.getFields().size(), 1 );
    EXPECT_EQ( response.getFields()[ 0 ], "" );
}

TEST( TestResponse, TestProprietery )
{
    const std::string representation = "$PASMTEST,*45\r\n";
    const std::vector< uint8_t > data( representation.begin(), representation.end() );

    const Response response( data );

    EXPECT_EQ( response.getTalkerID(), "PASM" );
    EXPECT_EQ( response.getSentenceID(), "TEST" );
    ASSERT_EQ( response.getFields().size(), 1 );
    EXPECT_EQ( response.getFields()[ 0 ], "" );
}

TEST( TestResponse, TestInvalidEnding )
{
    {
        const std::string representation = "$PASMTEST,*45\n";
        const std::vector< uint8_t > data( representation.begin(), representation.end() );

        EXPECT_THROW( {
            const Response response( data, true );
        },
                      Response::Exception );
    }

    {
        const std::string representation = "$PASMTEST,*45\r";
        const std::vector< uint8_t > data( representation.begin(), representation.end() );

        EXPECT_THROW( {
            const Response response( data, true );
        },
                      Response::Exception );
    }
}
