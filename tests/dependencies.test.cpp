#define CONFIG_CATCH_MAIN
#include <catch2/catch.hpp>
#include <icon.hpp>

TEST_CASE("Check if dependencies are found", "[dependencies]")
{
    REQUIRE(iconpp::icon::is_found());
}