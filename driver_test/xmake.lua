

-- 获取源目录下所有的 .c 文件
local src_files = os.files("*.cpp")

-- 遍历每个 .c 文件，创建目标
for _, file in ipairs(src_files) do
    local filename = path.basename(file)
    target(filename)
        set_kind("binary")
        add_files(file)
        add_links("CfgMgr32","hid")
        set_targetdir("$(buildir)/test")
end
