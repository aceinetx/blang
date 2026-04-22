import os
import sys
import pathlib

if len(sys.argv) == 1:
	print("no name provided (e.g. Something (would be turned into AstSomething class))")
	exit()

name = sys.argv[1]

directory = pathlib.Path(__file__).parent.parent.joinpath("src/frontend/ast")
header_path = directory.joinpath("Ast" + name + ".hh")
source_path = directory.joinpath("Ast" + name + ".cc")
if header_path.exists():
	print("header file already exists")
	exit()
if source_path.exists():
	print("source file already exists")
	exit()

header_template = """
#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct Ast%NAME% : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang) override;
};
} // namespace blang
"""
source_template = """
#include "frontend/ast/Ast%NAME%.hh"
#include "Blang.hh"
#include <fmt/base.h>

using namespace llvm;

namespace blang {
void Ast%NAME%::print(int indent) {
  printIndent(indent);
  fmt::println("- Ast%NAME%");
}

llvm::Value *Ast%NAME%::compile(Blang *blang) {
  return nullptr;
}

} // namespace blang
"""

with open(source_path, "w") as f:
	f.write(source_template.replace("%NAME%", name)[1:])
with open(header_path, "w") as f:
	f.write(header_template.replace("%NAME%", name)[1:])

print("ok")
