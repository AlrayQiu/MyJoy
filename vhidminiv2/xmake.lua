
-- add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"})

target("vhidmini")
do
    if is_host("windows") then
        add_rules("wdk.driver", "wdk.env.kmdf") 
        add_values("wdk.tracewpp.flags", "-func:TraceEvents(LEVEL,FLAGS,MSG,...)", "-func:Hexdump((LEVEL,FLAGS,MSG,...))")

        add_includedirs("./")

        add_files("./*.c", {rule = "wdk.tracewpp"})
        add_files("**.inx")
        -- add_files("*.mof|msdsm.mof")
        -- add_files("msdsm.mof", { values = { wdk_mof_header = "msdsmwmi.h" } })
        -- set test sign
        set_values("wdk.sign.mode", "test")
        set_values("wdk.sign.digest_algorithm", "sha1")
        set_values("wdk.sign.store", "PrivateCertStore")
        set_values("wdk.sign.company", "tboox.org(test)")
    end
end

