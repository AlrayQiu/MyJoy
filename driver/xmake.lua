rule("driver_test")
    on_load(function (target)
        target:set("kind","binary")
        target:set("targetdir", "$(buildir)/driver/"..os.arch().."/$(os)/test_bin")
    end)

rule("driver")
    if is_os("windows") then
        add_deps("wdk.driver", "wdk.env.kmdf")
    end
    on_load(function (target)
        target:set("targetdir", "$(buildir)/driver/"..os.arch().."/$(os)/")
        
        if is_host("windows") then
            target:set("wdk.sign.mode", "test")
            target:set("wdk.sign.digest_algorithm", "sha1")
            target:set("wdk.sign.store", "PrivateCertStore")
            target:set("wdk.sign.company", "tboox.org(test)")
        end
    end)

    -- add_includedirs(os.getenv("DDK_INC_PATH"))
    
includes("hidjoy","busjoy","test")