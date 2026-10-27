const std = @import("std");

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
    blocks: std.ArrayList(std.ArrayList(usize)) = .empty,

    pub fn init(allocator: std.mem.Allocator) Self {
        return Self{
            .allocator = allocator,
        };
    }

    pub fn deinit(self: *Self) void {
        for (self.blocks.items) |*block| {
            block.deinit(self.allocator);
        }

        self.blocks.deinit(self.allocator);
    }
};
