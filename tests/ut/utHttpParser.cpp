/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "catch/catch.hpp"

#include "HttpParser/HttpParser.hpp"

namespace parser = HttpParser;

TEST_CASE( "Simple increment function", "[unit test framework]" ) {
    int x = 5;
    REQUIRE( parser::increment(x) == 6 );
    REQUIRE( x == 6 );
}
