const std = @import("std");

pub const GlobalVariable = struct {
    name: []const u8,
    initializers: []const *Node,
    is_vector: bool,
    vector_size: i64,
};

pub const FunctionDefinition = struct {
    name: []const u8,
    args: []const []const u8,
    statement: *Node,
};

pub const NodeKind = union(enum) {
    root: []const *const Node,
    constant: i64,
    global: GlobalVariable,
    func_def: FunctionDefinition,
    ret: ?*const Node,
    block: []const *const Node,
};

pub const Node = struct {
    kind: NodeKind,
    location: usize,

    fn print_indent(indent: usize) void {
        for (0..indent) |_| {
            std.debug.print("\t", .{});
        }
    }

    pub fn print(self: *const @This(), indent: usize) void {
        Node.print_indent(indent);
        switch (self.kind) {
            .root => {
                std.debug.print("- root\n", .{});
                for (self.kind.root) |it| {
                    it.print(indent + 1);
                }
            },
            .constant => {
                std.debug.print("- constant {}\n", .{self.kind.constant});
            },
            .global => {
                std.debug.print("- global\n", .{});
                Node.print_indent(indent + 1);
                std.debug.print("- name: {s}\n", .{self.kind.global.name});
                Node.print_indent(indent + 1);
                std.debug.print("- initializers:\n", .{});
                for (self.kind.global.initializers) |it| {
                    it.print(indent + 2);
                }
                Node.print_indent(indent + 1);
                std.debug.print("- is_vector: {}\n", .{self.kind.global.is_vector});
                Node.print_indent(indent + 1);
                std.debug.print("- vector_size: {}\n", .{self.kind.global.vector_size});
            },
            .func_def => {
                std.debug.print("- func_def\n", .{});
                Node.print_indent(indent + 1);
                std.debug.print("- name: {s}\n", .{self.kind.func_def.name});
                Node.print_indent(indent + 1);
                std.debug.print("- args:\n", .{});
                for (self.kind.func_def.args) |it| {
                    Node.print_indent(indent + 2);
                    std.debug.print("- {s}\n", .{it});
                }
                Node.print_indent(indent + 1);
                std.debug.print("- statement:\n", .{});
                self.kind.func_def.statement.print(indent + 2);
            },
            .ret => {
                std.debug.print("- ret\n", .{});
                if (self.kind.ret) |expr| {
                    expr.print(indent + 1);
                }
            },
            .block => {
                std.debug.print("- block\n", .{});
                for (self.kind.block) |it| {
                    it.print(indent + 1);
                }
            },
        }
    }
};
