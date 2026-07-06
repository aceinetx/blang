target("libblang")
	set_kind("static")
	set_basename("blang")

	add_includedirs("src", {public=true})
	add_files("src/**.cc")
	add_files("src/**.yy")

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

		target:add("ldflags", io.readfile(stdout):trim(), {force=true, public=true})
	end)

    set_policy('build.fence', true)
target_end()


