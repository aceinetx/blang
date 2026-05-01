add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"}) -- generate compile commands

includes("rules/blang-yacc.lua")
includes("rules/blang.lua")
add_rules("blang-yacc")

if is_plat("linux") then
	set_policy("build.sanitizer.address", true)
	set_policy("build.sanitizer.leak", true)
end

add_requires("fmt", {external=false})
add_requires("bison")

set_warnings("all") -- warns
set_languages("c++20", "c90")

target("blang")
	set_kind("binary")
	set_default(true)

	add_includedirs("src")
	add_files("src/*.cc", "src/**/*.cc", "src/**/*.yy")

	add_packages("fmt")

	before_link(function(target)
		import("core.base.process")

		local stdout = os.tmpfile()
		local stderr = os.tmpfile()
		local proc = process.open("llvm-config --libs", {
				stdout = stdout,
				stderr = stderr
		})
		proc:wait()
		proc:close()

		target:add("ldflags", io.readfile(stdout):trim())
	end)
target_end()

-- Build libb
target("brt")
	add_rules("blang")
	set_kind("static")

	add_files("libb/brt.b")

	add_deps("blang")
target_end()

target("b")
	add_rules("blang")
	set_kind("static")

	add_files("libb/libb.b")

	add_deps("blang")
target_end()

-- Build examples
target("example-curses")
	add_rules("blang")
	set_kind("binary")

	add_files("examples/curses.b")
	add_links("ncurses")

	add_deps("blang")
target_end()

target("example-duffs_device")
	add_rules("blang")
	set_kind("binary")

	add_files("examples/duffs_device.b")

	add_deps("blang")
target_end()

target("example-hello")
	add_rules("blang")
	set_kind("binary")

	add_files("examples/hello.b")

	add_deps("blang")
target_end()

target("example-mandelbrot")
	add_rules("blang")
	set_kind("binary")

	add_files("examples/mandelbrot.b")

	add_deps("blang")
target_end()

target("example-turing")
	add_rules("blang")
	set_kind("binary")

	add_files("examples/turing.b")

	add_deps("blang")
target_end()

target("example-c_interop")
	add_rules("blang")
	set_kind("binary")

	add_files("examples/c_interop/*.c")
	add_files("examples/c_interop/*.b")

	add_deps("blang", "b")
target_end()
