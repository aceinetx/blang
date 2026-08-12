pub const Instruction = union(enum) {};

pub const IR = struct {
    pub fn init() @This() {
        return @This(){};
    }

    pub fn deinit(self: *@This()) void {
        _ = self;
    }
};
