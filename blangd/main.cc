#include "location.hh"
#include <Util.hh>
#include <blangd/util.hh>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <libblang-symscan/symscan.hh>
#include <nlohmann/json.hpp>

constexpr size_t MAX_REQUEST_LENGTH = 0xFFFFF;

static size_t id = -1;

static size_t get_content_length() {
  std::string line;
  size_t content_length = 0;

  while (true) {
    line.clear();
    int c;

    while ((c = getchar()) != '\n') {
      if (c == EOF)
        throw std::runtime_error("EOF");
      line.push_back((char)c);
      if (line.length() > MAX_REQUEST_LENGTH)
        throw std::runtime_error("Maximum request length exceeded");
    }

    if (line == "\r")
      break; // end of headers

    if (line.starts_with("Content-Length:")) {
      content_length = std::stoull(line.substr(strlen("Content-Length:")));
    }
  }

  return content_length;
}

static std::string get_content(size_t length) {
  std::string content;
  for (size_t i = 0; i < length; i++) {
    content.push_back(char(getchar()));
    if (content.length() > MAX_REQUEST_LENGTH)
      throw std::runtime_error("Maximum request length exceeded");
  }
  return content;
}

static void respond(const nlohmann::json &json) {
  std::string content = json.dump();

  std::cout << "Content-Length: " << content.size() << "\r\n"
            << "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
            << "\r\n"
            << content;

  std::cout.flush();
}

static nlohmann::json initialize(nlohmann::json &req) {
  id = req["id"];

  nlohmann::json resp;
  resp["jsonrpc"] = "2.0";
  resp["id"] = id;
  resp["result"] = nlohmann::json{};
  resp["result"]["capabilities"]["hoverProvider"] = true;
  return resp;
}

static nlohmann::json hover(nlohmann::json &req) {
  id = req["id"];

  size_t position_char = req["params"]["position"]["character"];
  size_t position_line = req["params"]["position"]["line"];
  std::string path = req["params"]["textDocument"]["uri"];
  path = path.substr(strlen("file://"));

  std::cerr << position_line << ":" << position_char << "@" << path << "\n";

  nlohmann::json resp;
  resp["jsonrpc"] = "2.0";
  resp["id"] = id;
  resp["result"] = nlohmann::json{};
  resp["result"]["contents"] = "unknown";

  auto source_opt = blang::readFile(path);
  if (!source_opt) {
    resp["result"]["contents"] = "can't read file";
    return resp;
  }
  auto source = *source_opt;

  try {
    auto pos =
        blang::blangd::index_from_linecol(source, position_line, position_char);
    auto word_range = blang::blangd::word_range(source, pos);
    auto word =
        source.substr(std::get<0>(word_range),
                      std::get<1>(word_range) - std::get<0>(word_range) + 1);
    resp["result"]["contents"] = "unknown " + word;

    auto pos_yacc =
        blang::position(nullptr, int(position_line), int(position_char));

    auto scan = blang::symscan::scan_source(source, pos_yacc);

    for (const auto &[sym, _] : scan.functions) {
      if (sym == word) {
        resp["result"]["contents"] = "function " + sym;
      }
    }

    for (const auto &sym : scan.global) {
      if (sym == word) {
        resp["result"]["contents"] = "global " + sym;
      }
    }

    if (!scan.focusedFunction.empty()) {
      auto func = scan.functions[scan.focusedFunction];
      for (const auto &sym : func.autoSymbols) {
        if (sym == word) {
          resp["result"]["contents"] = "automatic " + sym;
        }
      }

      for (const auto &sym : func.extrnSymbols) {
        if (sym == word) {
          resp["result"]["contents"] = "extern " + sym;
        }
      }

      for (const auto &sym : func.args) {
        if (sym == word) {
          resp["result"]["contents"] = "argument " + sym;
        }
      }
    }
  } catch (std::exception e) {
    std::cerr << "Exception while handling hover: " << e.what() << "\n";
  }

  return resp;
}

int main() {
  for (;;) {
    size_t length = get_content_length();
    std::string content = get_content(length);
    nlohmann::json json = nlohmann::json::parse(content);
    std::string method = json["method"];
    if (method == "initialize") {
      respond(initialize(json));
    } else if (method == "initialized") {
      // nop
    } else if (method == "shutdown") {
      return 0;
    } else if (method == "textDocument/hover") {
      std::cerr << json << "\n";
      respond(hover(json));
    } else {
      std::cerr << "Unknown method: " << method << "\n";
    }
  }
}
