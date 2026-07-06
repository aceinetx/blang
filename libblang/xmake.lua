target("libblang")
	add_rules("blang-yacc", {public=true})

	set_kind("static")
	set_basename("blang")

	add_includedirs("src", {public=true})
	add_files("src/**.cc")
	add_files("src/**.yy", {public=true})

	add_packages("fmt")

	on_load(function(target)
            local sourcefile_dir = path.join(target:autogendir(), "rules", "yacc_yacc")
            target:add("includedirs", sourcefile_dir, {public=true})
	end)

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
target_end()


