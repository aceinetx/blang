rule("blang")
	set_extensions(".b")

	on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
		import("lib.detect.find_program")
		local blang = assert(find_program("blang", {paths = {"/usr/bin", "/usr/local/bin", target:targetdir()}, check = "--help"}), "blang not found!")

		os.mkdir(target:targetdir())

		local objname = path.filename(target:objectfile(sourcefile))
		local objdir = path.directory(target:objectfile(sourcefile))
		local obj = objdir .. "/" .. objname

		batchcmds:show_progress(opt.progress, "${color.build.object}compiling.blang %s", sourcefile)
		batchcmds:mkdir(objdir)
		batchcmds:vrunv(blang, {"-c", sourcefile, "-o", obj})

		local objectfile = target:objectfile(sourcefile)
		table.insert(target:objectfiles(), objectfile)

		batchcmds:add_depfiles(sourcefile)
		batchcmds:set_depmtime(os.mtime(target:objectfile(sourcefile)))
		batchcmds:set_depcache(target:dependfile(target:objectfile(sourcefile)))
	end)
