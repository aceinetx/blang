target("libblang-symscan")
	set_kind("static")
	set_basename("blang-symscan")

	add_files("src/**.cc")
	add_includedirs("include", {public=true})

	add_deps("libblang", {public=true})
target_end()

target("blang-symscan")
	set_kind("binary")

	add_files("main.cc")

	add_deps("libblang-symscan")
target_end()

target("libblang-symscan-test")
	set_kind("binary")

	add_files("test/**.cc")

	add_deps("libblang-symscan")
	add_packages("catch2")
target_end()
