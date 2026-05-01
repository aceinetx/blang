-- Custom yacc rule that echoes the output of bison
rule("blang-yacc")
    add_deps("c++")
    set_extensions(".y", ".yy")
    on_load(function (target)
        -- add yacc includedirs if there are yacc files
        -- @see https://github.com/xmake-io/xmake/issues/4820
        if target:sourcebatches()["blang-yacc"] then
            local sourcefile_dir = path.join(target:autogendir(), "rules", "yacc_yacc")
            target:add("includedirs", sourcefile_dir)
        end
    end)
    before_buildcmd_file(function (target, batchcmds, sourcefile_yacc, opt)

        -- get yacc
        import("lib.detect.find_tool")
        local yacc = assert(find_tool("bison") or find_tool("yacc"), "yacc/bison not found!")

        -- get c/c++ source file for yacc
        local extension = path.extension(sourcefile_yacc)
        local sourcefile_cx = path.join(target:autogendir(), "rules", "yacc_yacc", path.basename(sourcefile_yacc) .. ".tab" .. (extension == ".yy" and ".cpp" or ".c"))

        -- add objectfile
        local objectfile = target:objectfile(sourcefile_cx)
        table.insert(target:objectfiles(), objectfile)

        -- add includedirs
        local sourcefile_dir = path.directory(sourcefile_cx)

        -- add commands
        batchcmds:show_progress(opt.progress, "${color.build.object}compiling.yacc %s", sourcefile_yacc)
        batchcmds:mkdir(sourcefile_dir)
        batchcmds:execv(yacc.program, {"-d", "-o", path(sourcefile_cx), path(sourcefile_yacc)})
        batchcmds:compile(sourcefile_cx, objectfile)

        -- add deps
        batchcmds:add_depfiles(sourcefile_yacc)
        batchcmds:set_depmtime(os.mtime(objectfile))
        batchcmds:set_depcache(target:dependfile(objectfile))
    end)
rule_end()

