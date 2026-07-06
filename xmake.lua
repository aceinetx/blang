add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"}) -- generate compile commands

includes("rules/blang-yacc.lua")

if is_plat("linux") then
	set_policy("build.sanitizer.address", true)
	set_policy("build.sanitizer.leak", true)
end

add_repositories("aceinet-xmake https://github.com/aceinetx/aceinet-xmake.git")

add_requires("fmt", {external=false})
add_requires("bison")
add_requires("nlohmann_json")

set_warnings("all") -- warns
set_languages("c++20", "c90")

includes("libblang")
includes("blang")
includes("blangd")
