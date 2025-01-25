add_rules("mode.debug", "mode.release")

add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"})

includes("driver_test", "myJoy_driver")