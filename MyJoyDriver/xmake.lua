add_rules("mode.debug", "mode.release")

add_defines("_UNICODE", "UNICODE")

add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"})

target("MyJoyDriver")
do
    if is_host("windows") then
        add_rules("wdk.driver", "wdk.env.umdf","wdk.env.wdm")
        add_values("wdk.tracewpp.flags", "-func:TracePrint((LEVEL,FLAGS,MSG,...))")

        add_includedirs("./include","./include/windows/")

        add_files("./src/windows/*.c", { rule = "wdk.tracewpp" })
        add_files("*.rc", "*.inf")
        add_files("*.mof|msdsm.mof")
        add_files("msdsm.mof", { values = { wdk_mof_header = "msdsmwmi.h" } })
        -- set test sign
        -- set_values("wdk.sign.mode", "test")
    end
end
