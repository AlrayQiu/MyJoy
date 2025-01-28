

add_rules("mode.debug", "mode.release")

add_rules("plugin.compile_commands.autoupdate", {outputdir = "$(buildir)"})

option("mode")
do
    set_default("debug")
end

includes("driver")