
-- add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"})
if is_os("windows") then
    target("busjoy")
        add_rules("driver")
        add_values("wdk.tracewpp.flags", "-func:TraceEvents(LEVEL,FLAGS,MSG,...)", "-func:Hexdump((LEVEL,FLAGS,MSG,...))")
        add_includedirs("./include")
        add_files("**mof")
        -- add_files("**inx|**inf")
        add_files("./src/*.c", {rule = "wdk.tracewpp"})
end