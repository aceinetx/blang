target("libblangd")
	set_kind("static")
    set_basename("blangd")

	add_includedirs("include", {public=true})
	add_files("src/**.cc")

	add_packages("nlohmann_json", {public=true})
    add_deps("libblang-symscan", {public=true})
target_end()

target("blangd")
	set_kind("binary")

	add_files("main.cc")

    add_deps("libblangd")
target_end()

target("blangd-test")
	set_kind("binary")

	add_files("test/**.cc")

    add_deps("libblangd")
    add_packages("catch2")
target_end()
