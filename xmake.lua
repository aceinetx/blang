add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"}) -- generate compile commands
add_rules("lex", "yacc")

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
