#include <libblang-symscan/symscan.hh>
#if 1
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TestAuto1") {
  const std::string source = R"(
main(){
  auto i, j, k;
}
  )";
  auto result = blang::symscan::scan_source(source);
  REQUIRE(result.functions.size() == 1);
  auto &func_name = result.functions.begin()->first;
  auto &func = result.functions.begin()->second;
  REQUIRE(func_name == "main");

  REQUIRE(func.autoSymbols.size() == 3);
  REQUIRE(func.autoSymbols[0] == "i");
  REQUIRE(func.autoSymbols[1] == "j");
  REQUIRE(func.autoSymbols[2] == "k");
}

TEST_CASE("TestGlobal1") {
  const std::string source = R"(
test;
a;
  )";
  auto result = blang::symscan::scan_source(source);
  REQUIRE(result.global.size() == 2);
  REQUIRE(result.global[0] == "test");
  REQUIRE(result.global[1] == "a");
}

TEST_CASE("TestFuncArg1") {
  const std::string source = R"(
main(argc, argv){};
  )";
  auto result = blang::symscan::scan_source(source);
  REQUIRE(result.functions.size() == 1);
  auto &func_name = result.functions.begin()->first;
  auto &func = result.functions.begin()->second;
  REQUIRE(func_name == "main");

  REQUIRE(func.args.size() == 2);
  REQUIRE(func.args[0] == "argc");
  REQUIRE(func.args[1] == "argv");
}

TEST_CASE("TestExtrn1") {
  const std::string source = R"(
main(){
  extrn i, j, k;
}
  )";
  auto result = blang::symscan::scan_source(source);
  REQUIRE(result.functions.size() == 1);
  auto &func_name = result.functions.begin()->first;
  auto &func = result.functions.begin()->second;
  REQUIRE(func_name == "main");

  REQUIRE(func.extrnSymbols.size() == 3);
  REQUIRE(func.extrnSymbols[0] == "i");
  REQUIRE(func.extrnSymbols[1] == "j");
  REQUIRE(func.extrnSymbols[2] == "k");
}

TEST_CASE("TestExtrnAuto1") {
  const std::string source = R"(
main(){
  extrn i, j, k;
  auto a, b, c;
}
  )";
  auto result = blang::symscan::scan_source(source);
  REQUIRE(result.functions.size() == 1);
  auto &func_name = result.functions.begin()->first;
  auto &func = result.functions.begin()->second;
  REQUIRE(func_name == "main");

  REQUIRE(func.extrnSymbols.size() == 3);
  REQUIRE(func.extrnSymbols[0] == "i");
  REQUIRE(func.extrnSymbols[1] == "j");
  REQUIRE(func.extrnSymbols[2] == "k");

  REQUIRE(func.autoSymbols.size() == 3);
  REQUIRE(func.autoSymbols[0] == "a");
  REQUIRE(func.autoSymbols[1] == "b");
  REQUIRE(func.autoSymbols[2] == "c");
}

TEST_CASE("TestMultiFunc1") {
  const std::string source = R"(
a(){
  auto i, j, k;
}
b(){
  auto a, b, c;
}
  )";
  auto result = blang::symscan::scan_source(source);
  REQUIRE(result.functions.size() == 2);
  REQUIRE(result.functions.contains("a"));
  auto &a = result.functions["a"];

  REQUIRE(a.autoSymbols.size() == 3);
  REQUIRE(a.autoSymbols[0] == "i");
  REQUIRE(a.autoSymbols[1] == "j");
  REQUIRE(a.autoSymbols[2] == "k");

  REQUIRE(result.functions.contains("b"));
  auto &b = result.functions["b"];

  REQUIRE(b.autoSymbols.size() == 3);
  REQUIRE(b.autoSymbols[0] == "a");
  REQUIRE(b.autoSymbols[1] == "b");
  REQUIRE(b.autoSymbols[2] == "c");
}

TEST_CASE("TestSingle1") {
  const std::string source = R"(
a() auto i; extrn j; return(0);
  )";
  REQUIRE_THROWS(blang::symscan::scan_source(source));
}
#endif
