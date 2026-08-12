const lexer = @import("lexer.zig");
const Token = @import("token.zig").Token;
const TokenKindTag = @import("token.zig").TokenKindTag;
const std = @import("std");
const ast = @import("ast.zig");

pub const ParserError = error{
    UnexpectedIvalToken,
    UnexpectedIvalListToken,
    ExpectedRbracket,
    ExpectedRbrace,
    ExpectedIdentifier,
    ExpectedLparen,
    ExpectedRparen,
    ExpectedSemicolon,
    UnexpectedStatementToken,
    UnexpectedFunctionArgsToken,
    ExpectedIdentifierOrRparen,
} || std.mem.Allocator.Error;

pub const ParserDiagnostic = struct {
    location: usize,
};

pub const Parser = struct {
    allocator: std.mem.Allocator,
    lexer: *lexer.Lexer,
    arena: std.heap.ArenaAllocator,
    root: ?*ast.Node,
    diagnostic: ParserDiagnostic,

    pub fn init(
        allocator: std.mem.Allocator,
        lex: *lexer.Lexer,
    ) @This() {
        return @This(){
            .allocator = allocator,
            .lexer = lex,
            .arena = .init(allocator),
            .root = null,
            .diagnostic = std.mem.zeroes(ParserDiagnostic),
        };
    }

    pub fn deinit(self: *Parser) void {
        self.arena.deinit();
    }

    fn create_node(self: *Parser) !*ast.Node {
        return try self.arena.allocator().create(ast.Node);
    }

    fn expect_token(self: *Parser, tag: TokenKindTag, err: ParserError) ParserError!Token {
        const token = self.lexer.next();
        if (token.kind != tag) {
            self.diagnostic.location = token.location;
            return err;
        }
        return token;
    }

    fn parse_ival(self: *Parser) !*ast.Node {
        const token = self.lexer.next();
        switch (token.kind) {
            .number => {
                var node = try self.create_node();
                node.kind = .{ .constant = token.kind.number };
                node.location = token.location;
                return node;
            },
            // .identifier => {
            //     var node = try self.create_node();
            //     node.kind = .{ .constant = 0 };
            //     node.location = token.location;
            //     return node;
            // },
            else => {
                self.diagnostic.location = token.location;
                return ParserError.UnexpectedIvalToken;
            },
        }
    }

    fn parse_ival_list(self: *Parser, list: *std.ArrayList(*ast.Node)) !void {
        const token = self.lexer.peek();
        switch (token.kind) {
            .semicolon => {},
            else => {
                try list.append(self.allocator, try self.parse_ival());
                const next = self.lexer.next();
                switch (next.kind) {
                    .semicolon => {},
                    .comma => {
                        try self.parse_ival_list(list);
                    },
                    else => {
                        self.diagnostic.location = next.location;
                        return ParserError.UnexpectedIvalListToken;
                    },
                }
            },
        }
    }

    fn parse_function_arguments(self: *Parser, list: *std.ArrayList([]const u8)) !void {
        const token = self.lexer.next();
        switch (token.kind) {
            .rparen => {},
            .identifier => {
                const name = try self.arena.allocator().dupe(u8, token.kind.identifier);
                try list.append(self.allocator, name);

                const next = self.lexer.next();
                switch (next.kind) {
                    .rparen => {},
                    .comma => {
                        try self.parse_function_arguments(list);
                    },
                    else => {
                        self.diagnostic.location = next.location;
                        return ParserError.UnexpectedFunctionArgsToken;
                    },
                }
            },
            else => {
                self.diagnostic.location = token.location;
                return ParserError.ExpectedIdentifierOrRparen;
            },
        }
    }

    fn parse_expression(self: *Parser) !*ast.Node {
        const num = try self.expect_token(.number, ParserError.OutOfMemory);
        const node = try self.create_node();
        node.* = .{
            .kind = .{
                .constant = num.kind.number,
            },
            .location = num.location,
        };
        return node;
    }

    fn parse_statement(self: *Parser) !*ast.Node {
        const stmt_token = self.lexer.next();
        switch (stmt_token.kind) {
            .lbrace => {
                const nodes_slice = blk: {
                    var nodes = std.ArrayList(*ast.Node).empty;
                    defer nodes.deinit(self.allocator);
                    while (self.lexer.peek().kind != .rbrace) {
                        try nodes.append(self.allocator, try self.parse_statement());
                    }

                    break :blk try self.arena.allocator().dupe(*const ast.Node, nodes.items);
                };

                _ = try self.expect_token(.rbrace, ParserError.ExpectedRbrace);

                const node = try self.create_node();
                node.* = .{
                    .kind = .{
                        .block = nodes_slice,
                    },
                    .location = stmt_token.location,
                };
                return node;
            },
            .kw_return => {
                _ = try self.expect_token(.lparen, ParserError.ExpectedLparen);
                const expr = try self.parse_expression();
                _ = try self.expect_token(.rparen, ParserError.ExpectedRparen);
                _ = try self.expect_token(.semicolon, ParserError.ExpectedSemicolon);

                const node = try self.create_node();
                node.* = .{
                    .kind = .{
                        .ret = expr,
                    },
                    .location = stmt_token.location,
                };
                return node;
            },
            else => {
                self.diagnostic.location = stmt_token.location;
                return ParserError.ExpectedIdentifier;
            },
        }
    }

    fn parse_definition(self: *Parser) !*ast.Node {
        const name_token = try self.expect_token(.identifier, ParserError.ExpectedIdentifier);
        const name = try self.arena.allocator().dupe(u8, name_token.kind.identifier);

        const is_variable = self.lexer.peek().kind != .lparen;
        if (is_variable) {
            const is_vector = self.lexer.peek().kind == .lbracket;
            var vector_size: i64 = 0;
            if (is_vector) {
                _ = self.lexer.next();

                const vector_size_token = self.lexer.peek();
                switch (vector_size_token.kind) {
                    .number => {
                        vector_size = vector_size_token.kind.number;
                        _ = self.lexer.next();
                    },
                    else => {},
                }

                _ = try self.expect_token(.rbracket, ParserError.ExpectedRbracket);
            }

            var list = std.ArrayList(*ast.Node).empty;
            try self.parse_ival_list(&list);
            const initializers = try self.arena.allocator().dupe(*ast.Node, list.items);
            list.deinit(self.allocator);

            if (vector_size <= 0)
                vector_size = @intCast(initializers.len);

            const node = try self.create_node();
            node.* = .{
                .kind = .{ .global = .{
                    .name = name,
                    .initializers = initializers,
                    .is_vector = is_vector,
                    .vector_size = vector_size,
                } },
                .location = name_token.location,
            };
            return node;
        } else {
            _ = try self.expect_token(.lparen, ParserError.ExpectedLparen);

            var list = std.ArrayList([]const u8).empty;
            try self.parse_function_arguments(&list);
            const args = try self.arena.allocator().dupe([]const u8, list.items);
            list.deinit(self.allocator);

            const statement = try self.parse_statement();

            const node = try self.create_node();
            node.* = .{
                .kind = .{ .func_def = .{
                    .name = name,
                    .args = args,
                    .statement = statement,
                } },
                .location = name_token.location,
            };
            return node;
        }
    }

    pub fn parse_program(self: *Parser) !void {
        _ = self.arena.reset(.free_all);
        try self.lexer.tokenize();

        var nodes = std.ArrayList(*ast.Node).empty;
        while (self.lexer.peek().kind != .eof) {
            try nodes.append(self.allocator, try self.parse_definition());
        }
        const nodes_slice = try self.arena.allocator().dupe(*const ast.Node, nodes.items);
        nodes.deinit(self.allocator);

        self.root = try self.arena.allocator().create(ast.Node);
        self.root.?.* = .{
            .kind = .{
                .root = nodes_slice,
            },
            .location = 0,
        };
    }
};
