add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"}) -- generate compile commands

includes("../rules/blang.lua")
add_rules("c", "c++", "blang")

target("example-curses")
	set_kind("binary")

	add_files("curses.b")
	add_links("ncurses")
target_end()

target("example-duffs_device")
	set_kind("binary")

	add_files("duffs_device.b")
target_end()

target("example-hello")
	set_kind("binary")

	add_files("hello.b")
target_end()

target("example-mandelbrot")
	set_kind("binary")

	add_files("mandelbrot.b")
target_end()

target("example-turing")
	set_kind("binary")

	add_files("turing.b")
target_end()

target("example-c_interop")
	set_kind("binary")

	add_files("c_interop/*.c")
	add_files("c_interop/*.b")
target_end()

target("example-array")
	set_kind("binary")

	add_files("array.b")
target_end()
