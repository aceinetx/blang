add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"}) -- generate compile commands

includes("../rules/blang.lua")

target("brt")
	add_rules("blang")
	add_rules("utils.install.pkgconfig_importfiles")

	set_kind("static")

	add_files("brt.b")
target_end()

target("b")
	add_rules("blang")
	add_rules("utils.install.pkgconfig_importfiles")

	set_kind("static")

	add_files("libb.b")
target_end()
