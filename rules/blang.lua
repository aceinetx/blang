rule("blang")
	set_extensions(".b")

	on_load(function (target)
		if target:sourcebatches()["blang"] then
				local sourcefile_dir = path.join(target:autogendir(), "rules", "blang")
				target:add("includedirs", sourcefile_dir)
		end
	end)

	on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
		-- find blang
		import("lib.detect.find_program")
		local blang = assert(find_program("blang", {paths = {"/usr/bin", "/usr/local/bin", target:targetdir()}, check = "--help"}), "blang not found!")

		os.mkdir(target:targetdir())

		local objname = path.filename(target:objectfile(sourcefile))
		local objdir = path.directory(target:objectfile(sourcefile))
		local obj = objdir .. "/" .. objname

		local binding_path = path.absolute(path.join(target:autogendir(), "rules", "blang", path.basename(sourcefile) .. ".h"))

		-- add object file
		local objectfile = target:objectfile(sourcefile)
		table.insert(target:objectfiles(), objectfile)

		-- run blang
		batchcmds:show_progress(opt.progress, "${color.build.object}compiling.blang %s", sourcefile)
		batchcmds:mkdir(objdir)
		batchcmds:mkdir(path.directory(binding_path))
		batchcmds:execv(blang, {"-c", sourcefile, "-o", obj, "--bindings-out", binding_path})

		batchcmds:add_depfiles(sourcefile)
		batchcmds:set_depmtime(os.mtime(objectfile))
		batchcmds:set_depcache(target:dependfile(objectfile))
	end)
