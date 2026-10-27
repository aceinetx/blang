const std = @import("std");
const ast = @import("ast.zig");

pub const Instruction = struct {
    source_index: usize,
    tag: Tag,
    data: Data,

    pub const Tag = enum(u8) {
        func,

        // Contains an index to the blocks field of IR
        block,

        ret,

        int,
    };

    pub const Data = union {
        func: usize,

        block: usize,

        ret: usize,

        int: usize,
    };
};

pub const IR = struct {
    const Self = @This();

    allocator: std.mem.Allocator,
    arena: std.heap.ArenaAllocator,
    blocks: std.ArrayList(std.ArrayList(usize)) = .empty,
    strings: std.ArrayList([]const u8) = .empty,
    instructions: std.ArrayList(Instruction) = .empty,

    pub fn init(allocator: std.mem.Allocator) Self {
        return Self{
            .arena = .init(allocator),
            .allocator = allocator,
        };
    }

    pub fn deinit(self: *Self) void {
        self.instructions.deinit(self.allocator);

        for (self.strings.items) |it| {
            self.allocator.free(it);
        }

        self.strings.deinit(self.allocator);

        for (self.blocks.items) |*block| {
            block.deinit(self.allocator);
        }

        self.blocks.deinit(self.allocator);

        self.arena.deinit();
    }

    pub fn gen(self: *Self, node: *const ast.Node) !?usize {
        return switch (node.kind) {
            .root => |root| {
                var block = std.ArrayList(usize).empty;

                for (root) |it| {
                    try block.append(self.allocator, (try self.gen(it)).?);
                }

                try self.blocks.append(self.allocator, block);
                const block_id = self.blocks.items.len - 1;

                try self.instructions.append(
                    self.allocator,
                    .{
                        .source_index = node.location,
                        .tag = .block,
                        .data = .{
                            .block = block_id,
                        },
                    },
                );
                const instruction_id = self.instructions.items.len - 1;
                return instruction_id;
            },
            .global => 0,
            .func_def => 0,
            else => @panic("todo"),
        };
    }
};
