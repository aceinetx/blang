#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

static std::string ParseEscapeSequences(const std::string &input) {
  std::string output;
  for (size_t i = 0; i < input.length(); ++i) {
    if (input[i] == '*') {
      // Check the next character for escape sequences
      if (i + 1 < input.length()) {
        switch (input[i + 1]) {
        case 'n':
          output += '\n'; // Newline
          break;
        case 't':
          output += '\t'; // Tab
          break;
        case 'r':
          output += '\r'; // Carriage return
          break;
        case 'b':
          output += '\b'; // Backspace
          break;
        case '*':
          output += '*'; // Asterisk
          break;
        case 'f':
          output += '\f'; // Form feed
          break;
        case '\"':
          output += '\"'; // Double quote
          break;
        case '\'':
          output += '\''; // Single quote
          break;
        default:
          // If it's not a recognized escape sequence, just add the backslash
          output += input[i];
          output += input[i + 1];
          break;
        }
        // Skip the next character since it's part of the escape sequence
        ++i;
      } else {
        // If the backslash is the last character, just add it
        output += input[i];
      }
    } else {
      output += input[i]; // Regular character
    }
  }
  return output;
}

void AstStrRef::print(int indent) const {
  printIndent(indent);
  std::cout << "AstStrRef" << std::endl;
  printIndent(indent + 1);
  std::cout << "\"" << str << "\"\n";
}

bool AstStrRef::compile(Blang *blang) {
  if (blang->expr_types.top() == Blang::LVALUE) {
    blang->compile_error = fmt::format("string cannot be an lvalue");
    return false;
  }

  blang->values.push(blang->builder.CreateGlobalStringPtr(
      ParseEscapeSequences(str), "", 0U, &blang->fmodule));

  blang->expr_types.pop();

  return true;
}
