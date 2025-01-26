

add_rules("mode.debug", "mode.release")

add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"})

option("mode")
do
    set_default("release")
end 

includes("driver_test", "myJoy_driver")