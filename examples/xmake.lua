add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"}) -- generate compile commands

includes("../rules/blang.lua")

target("example-curses")
	add_rules("blang")
	set_kind("binary")

	add_files("curses.b")
	add_ldflags("-lncurses")
target_end()

target("example-duffs_device")
	add_rules("blang")
	set_kind("binary")

	add_files("duffs_device.b")
target_end()

target("example-hello")
	add_rules("blang")
	set_kind("binary")

	add_files("hello.b")
target_end()

target("example-mandelbrot")
	add_rules("blang")
	set_kind("binary")

	add_files("mandelbrot.b")
target_end()

target("example-turing")
	add_rules("blang")
	set_kind("binary")

	add_files("turing.b")
target_end()

target("example-c_interop")
	add_rules("blang")
	set_kind("binary")

	add_files("c_interop/*.c")
	add_files("c_interop/*.b")
target_end()
