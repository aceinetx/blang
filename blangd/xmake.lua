add_requires("nlohmann_json")

target("blangd")
	set_kind("binary")

	add_includedirs(".")
	add_files("**.cc")

	add_packages("nlohmann_json")
target_end()
