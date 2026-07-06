#include <Util.hh>
#include <iostream>
#include <libblang-symscan/symscan.hh>

static void print_scan_func(const std::string &name,
                            const blang::symscan::FunctionScanResult &func) {
  printf("\t- %s\n", name.c_str());
  puts("\t\t- Externs:");
  for (const auto &sym : func.extrnSymbols) {
    printf("\t\t\t- %s\n", sym.c_str());
  }
  puts("\t\t- Auto vars:");
  for (const auto &sym : func.autoSymbols) {
    printf("\t\t\t- %s\n", sym.c_str());
  }
}

static void print_scan(blang::symscan::ScanResult &scan) {
  puts("Scan results:");
  puts("- Globals:");
  for (const auto &global : scan.global) {
    printf("\t- %s\n", global.c_str());
  }
  puts("- Functions:");
  for (const auto &function : scan.functions) {
    print_scan_func(function.first, function.second);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    puts("usage: blang-symscan [filename]");
    return 1;
  }

  char *filename = argv[1];
  auto source_opt = blang::readFile(filename);
  if (!source_opt) {
    std::cerr << "failed to read " << filename << "\n";
    return 1;
  }
  auto source = *source_opt;

  auto scan = blang::symscan::scan_source(source);

  print_scan(scan);

  return 0;
}
