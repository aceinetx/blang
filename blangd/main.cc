#include <cassert>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

constexpr size_t MAX_REQUEST_LENGTH = 0xFFFFF;

static size_t id = -1;

static std::vector<std::string> readfile(const std::string &path) {
  std::vector<std::string> lines;
  std::ifstream file(path);
  std::string line;
  while (std::getline(file, line)) {
    lines.push_back(line);
  }
  return lines;
}

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
  auto lines = readfile(path);

  std::cerr << position_line << ":" << position_char << "@" << path << "\n";

  nlohmann::json resp;
  resp["jsonrpc"] = "2.0";
  resp["id"] = id;
  resp["result"] = nlohmann::json{};
  resp["result"]["contents"] = lines[position_line];
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
      respond(hover(json));
    } else {
      std::cerr << "Unknown method: " << method << "\n";
    }
  }
}
