@echo off
chcp 65001 > nul
setlocal

set STUBS=DIMA\eval\stubs
set GEN=DIMA\eval\generated
set LOG=compile_check_results.txt

echo 编译检查结果 > %LOG%
echo 生成时间: %DATE% %TIME% >> %LOG%
echo ======================== >> %LOG%

for %%s in (zeroshot cot) do (
    for %%p in (ps1 ps2 ps3 ps4 ps5) do (
        echo 检查 %%s\%%p ...
        echo [%%s/%%p] >> %LOG%
        cl /Zs /W3 /utf-8 /wd4819 /I %STUBS% %GEN%\%%s\%%p\*.c >> %LOG% 2>&1
        echo. >> %LOG%
    )
)

for %%s in (fewshot combined) do (
    for %%p in (pa pb pc) do (
        echo 检查 %%s\%%p ...
        echo [%%s/%%p] >> %LOG%
        cl /Zs /W3 /utf-8 /wd4819 /I %STUBS% %GEN%\%%s\%%p\*.c >> %LOG% 2>&1
        echo. >> %LOG%
    )
)

echo.
echo 完成，结果保存在 %LOG%
echo 运行 python DIMA\eval\parse_compile.py 查看摘要表格
