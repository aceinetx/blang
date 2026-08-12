const Token = @import("token.zig").Token;
const std = @import("std");
const util = @import("util.zig");

pub const LexerError = error{
    UnexpectedEOF,
    InvalidCharacter,
    TokenIdentifierTooLong,
};

pub const Lexer = struct {
    code: []const u8,
    tokenize_pos: usize,
    index: usize,
    allocator: std.mem.Allocator,
    arena: std.heap.ArenaAllocator,
    tokens: std.ArrayList(Token),

    pub fn init(allocator: std.mem.Allocator, code: []const u8) @This() {
        return .{
            .code = code,
            .tokenize_pos = 0,
            .index = 0,
            .allocator = allocator,
            .arena = .init(allocator),
            .tokens = .empty,
        };
    }

    pub fn deinit(self: *@This()) void {
        self.arena.deinit();
        self.tokens.deinit(self.allocator);
    }

    pub fn tokenize(self: *@This()) !void {
        self.tokens.clearRetainingCapacity();
        while (true) {
            const token = try self.get_token();
            if (token.kind == .eof) break;
            try self.tokens.append(self.allocator, token);
        }
        self.index = 0;
    }

    pub fn next(self: *@This()) Token {
        if (self.index >= self.tokens.items.len) {
            return .{
                .kind = .eof,
                .location = self.code.len,
            };
        }
        const token = self.tokens.items[self.index];
        self.index += 1;
        return token;
    }

    pub fn peek(self: *@This()) Token {
        const index = self.index;
        const token = self.next();
        self.index = index;
        return token;
    }

    pub fn peek_n(self: *@This(), n: usize) Token {
        std.debug.assert(n > 0);
        var token: Token = undefined;
        const index = self.index;
        for (0..n) |_| {
            token = self.next();
        }
        self.index = index;
        return token;
    }

    fn isspace(c: u8) bool {
        return c == ' ' or (@as(u32, c) -% '\t' < 5);
    }

    fn isalpha(c: u8) bool {
        return (@as(u32, c) | 32) -% 'a' < 26;
    }

    fn isdigit(c: u8) bool {
        return (@as(u32, c) -% '0') < 10;
    }

    fn isalnum(c: u8) bool {
        return isalpha(c) | isdigit(c);
    }

    fn char(self: *@This(), off: usize) u8 {
        const pos = self.tokenize_pos + off;
        if (pos >= self.code.len) {
            return 0;
        } else {
            return self.code[pos];
        }
    }

    fn get_token(self: *@This()) !Token {
        while (self.tokenize_pos < self.code.len) {
            const c = self.char(0);

            if (isalpha(c)) {
                var len: usize = 0;
                while (isalnum(self.code[self.tokenize_pos + len])) {
                    len += 1;
                    if (self.tokenize_pos + len >= self.code.len) break;
                }
                const str = self.code[self.tokenize_pos .. self.tokenize_pos + len];
                const loc = self.tokenize_pos;

                self.tokenize_pos += len;

                // Keywords
                if (util.strcmp(str, "return")) {
                    return .{
                        .kind = .kw_return,
                        .location = loc,
                    };
                } else if (util.strcmp(str, "auto")) {
                    return .{
                        .kind = .kw_auto,
                        .location = loc,
                    };
                } else if (util.strcmp(str, "extrn")) {
                    return .{
                        .kind = .kw_extrn,
                        .location = loc,
                    };
                } else if (util.strcmp(str, "while")) {
                    return .{
                        .kind = .kw_while,
                        .location = loc,
                    };
                } else if (util.strcmp(str, "break")) {
                    return .{
                        .kind = .kw_break,
                        .location = loc,
                    };
                } else if (util.strcmp(str, "goto")) {
                    return .{
                        .kind = .kw_goto,
                        .location = loc,
                    };
                } else if (util.strcmp(str, "if")) {
                    return .{
                        .kind = .kw_if,
                        .location = loc,
                    };
                } else if (util.strcmp(str, "else")) {
                    return .{
                        .kind = .kw_else,
                        .location = loc,
                    };
                } else if (util.strcmp(str, "switch")) {
                    return .{
                        .kind = .kw_switch,
                        .location = loc,
                    };
                } else if (util.strcmp(str, "case")) {
                    return .{
                        .kind = .kw_case,
                        .location = loc,
                    };
                }

                return .{
                    .kind = .{
                        .identifier = str,
                    },
                    .location = loc,
                };
            }
            if (isdigit(c)) {
                var num: i64 = 0;
                const loc = self.tokenize_pos;

                while (isdigit(self.code[self.tokenize_pos])) {
                    num *= 10;
                    num += self.code[self.tokenize_pos] - '0';
                    self.tokenize_pos += 1;
                    if (self.tokenize_pos >= self.code.len) break;
                }

                return .{
                    .kind = .{
                        .number = num,
                    },
                    .location = loc,
                };
            }
            if (c == '"') {
                const loc = self.tokenize_pos;
                self.tokenize_pos += 1;

                var len: usize = 0;
                while (self.code[self.tokenize_pos + len] != '"') {
                    len += 1;
                    if (self.tokenize_pos + len >= self.code.len) {
                        return error.UnexpectedEOF;
                    }
                }

                // TODO: Escaping
                const str = self.code[self.tokenize_pos .. self.tokenize_pos + len];

                self.tokenize_pos += len + 1;

                return .{
                    .kind = .{
                        .string_lit = str,
                    },
                    .location = loc,
                };
            }

            if (c == '=') {
                self.tokenize_pos += 1;
                return .{
                    .kind = .equal,
                    .location = self.tokenize_pos - 1,
                };
            } else if (c == '(') {
                self.tokenize_pos += 1;
                return .{ .kind = .lparen, .location = self.tokenize_pos - 1 };
            } else if (c == ')') {
                self.tokenize_pos += 1;
                return .{ .kind = .rparen, .location = self.tokenize_pos - 1 };
            } else if (c == '{') {
                self.tokenize_pos += 1;
                return .{ .kind = .lbrace, .location = self.tokenize_pos - 1 };
            } else if (c == '}') {
                self.tokenize_pos += 1;
                return .{ .kind = .rbrace, .location = self.tokenize_pos - 1 };
            } else if (c == '[') {
                self.tokenize_pos += 1;
                return .{ .kind = .lbracket, .location = self.tokenize_pos - 1 };
            } else if (c == ']') {
                self.tokenize_pos += 1;
                return .{ .kind = .rbracket, .location = self.tokenize_pos - 1 };
            } else if (c == ';') {
                self.tokenize_pos += 1;
                return .{ .kind = .semicolon, .location = self.tokenize_pos - 1 };
            } else if (c == ':') {
                self.tokenize_pos += 1;
                return .{ .kind = .colon, .location = self.tokenize_pos - 1 };
            } else if (c == ',') {
                self.tokenize_pos += 1;
                return .{ .kind = .comma, .location = self.tokenize_pos - 1 };
            } else if (c == '?') {
                self.tokenize_pos += 1;
                return .{ .kind = .question, .location = self.tokenize_pos - 1 };
            } else if (c == '+') {
                self.tokenize_pos += 1;
                if (self.char(1) == '+') {
                    self.tokenize_pos += 1;
                    return .{ .kind = .plusplus, .location = self.tokenize_pos - 2 };
                }
                return .{ .kind = .plus, .location = self.tokenize_pos - 1 };
            } else if (c == '-') {
                self.tokenize_pos += 1;
                if (self.char(1) == '-') {
                    self.tokenize_pos += 1;
                    return .{ .kind = .minusminus, .location = self.tokenize_pos - 2 };
                }
                return .{ .kind = .minus, .location = self.tokenize_pos - 1 };
            } else if (c == '*') {
                self.tokenize_pos += 1;
                return .{ .kind = .mul, .location = self.tokenize_pos - 1 };
            } else if (c == '/') {
                self.tokenize_pos += 1;
                return .{ .kind = .div, .location = self.tokenize_pos - 1 };
            } else if (c == '=') {
                self.tokenize_pos += 1;
                if (self.char(1) == '=') {
                    self.tokenize_pos += 1;
                    return .{ .kind = .equal, .location = self.tokenize_pos - 2 };
                }
                return .{ .kind = .assign, .location = self.tokenize_pos - 1 };
            } else if (c == '!') {
                self.tokenize_pos += 1;
                if (self.char(1) == '=') {
                    self.tokenize_pos += 1;
                    return .{ .kind = .nequal, .location = self.tokenize_pos - 2 };
                }
                return .{ .kind = .exclamation, .location = self.tokenize_pos - 1 };
            } else if (c == '|') {
                self.tokenize_pos += 1;
                return .{ .kind = .bitor, .location = self.tokenize_pos - 1 };
            } else if (c == '&') {
                self.tokenize_pos += 1;
                return .{ .kind = .bitand, .location = self.tokenize_pos - 1 };
            } else if (c == '<') {
                self.tokenize_pos += 1;
                if (self.char(1) == '=') {
                    self.tokenize_pos += 1;
                    return .{ .kind = .nequal, .location = self.tokenize_pos - 2 };
                } else if (self.char(1) == '<') {
                    self.tokenize_pos += 1;
                    return .{ .kind = .bitshl, .location = self.tokenize_pos - 2 };
                }
                return .{ .kind = .less, .location = self.tokenize_pos - 1 };
            } else if (c == '>') {
                self.tokenize_pos += 1;
                if (self.char(1) == '=') {
                    self.tokenize_pos += 1;
                    return .{ .kind = .nequal, .location = self.tokenize_pos - 2 };
                } else if (self.char(1) == '>') {
                    self.tokenize_pos += 1;
                    return .{ .kind = .bitshr, .location = self.tokenize_pos - 2 };
                }
                return .{ .kind = .greater, .location = self.tokenize_pos - 1 };
            } else if (c == '%') {
                self.tokenize_pos += 1;
                return .{ .kind = .percent, .location = self.tokenize_pos - 1 };
            }

            if (!isspace(c)) {
                std.debug.print("Invalid character: {c}\n", .{c});
                return error.InvalidCharacter;
            } else self.tokenize_pos += 1;
        }
        return .{ .kind = .eof, .location = self.tokenize_pos };
    }
};
