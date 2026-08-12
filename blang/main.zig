const libblang = @import("libblang");
const std = @import("std");

pub fn main(init: std.process.Init) !void {
    var lexer = libblang.lexer.Lexer.init(init.gpa,
        \\a 0;
        \\main(argc){
        \\return(0);
        \\}
    );
    defer lexer.deinit();

    var parser = libblang.parser.Parser.init(init.gpa, &lexer);
    defer parser.deinit();

    try parser.parse_program();

    parser.root.?.print(0);

    var ir = libblang.ir.IR.init();
    defer ir.deinit();
}
