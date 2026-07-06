#include "blangd/util.hh"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("index_from_linecol") {
  size_t pos{};
  pos = blang::blangd::index_from_linecol("abc\nblang\ndef", 1, 2);
  REQUIRE(pos == 6);
}

TEST_CASE("word_range1") {
  auto pos = blang::blangd::word_range("a bcd efh hji", 2);
  REQUIRE(std::get<0>(pos) == 2);
  REQUIRE(std::get<1>(pos) == 4);
}

TEST_CASE("word_range2") {
  auto pos = blang::blangd::word_range("abc bcd efh hji", 2);
  REQUIRE(std::get<0>(pos) == 0);
  REQUIRE(std::get<1>(pos) == 2);
}

TEST_CASE("word_range3") {
  auto pos = blang::blangd::word_range("abc bcd efh hji", 2);
  REQUIRE(std::get<0>(pos) == 0);
  REQUIRE(std::get<1>(pos) == 2);
}
