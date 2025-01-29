
if is_os("windows") then
    target("hidjoy")
        add_rules("driver")
        add_values("wdk.tracewpp.flags", "-func:TraceEvents(LEVEL,FLAGS,MSG,...)", "-func:Hexdump((LEVEL,FLAGS,MSG,...))")
        add_includedirs("./include/","./include/hid_def")
        add_files("**inx|**inf")
        add_files("./src/*.c", {rule = "wdk.tracewpp"})

        
        set_values("wdk.sign.mode", "test")
        set_values("wdk.sign.digest_algorithm", "sha1")
        set_values("wdk.sign.store", "PrivateCertStore")
        set_values("wdk.sign.company", "tboox.org(test)")
end