
-- add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"})

rule("third_party")
    on_load(function (target)
        set_targetdir("$(buildir)/third_patry")
    end)