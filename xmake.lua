add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"}) -- generate compile commands

includes("rules/blang-yacc.lua")
includes("rules/blang.lua")
add_rules("blang-yacc", "blang")

if is_plat("linux") then
	set_policy("build.sanitizer.address", true)
	set_policy("build.sanitizer.leak", true)
end

add_requires("fmt", {external=false})
add_requires("bison")

set_warnings("all") -- warns
set_languages("c++20")

target("blang")
	set_kind("binary")

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

target("brt")
	set_kind("static")

	add_files("libb/brt.b")

	add_deps("blang")
target_end()

target("b")
	set_kind("static")

	add_files("libb/libb.b")

	add_deps("blang")
target_end()
