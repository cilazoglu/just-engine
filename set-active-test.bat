@echo off
SETLOCAL

SET TESTS_DIR=C:\dev\c\just-engine\tests

:loop
if NOT "%1"=="" (
    if "%1"=="--test-name" (
        SET MAKE_ACTIVE_TEST_NAME=%2
        SHIFT
    ) else if "%1"=="--tests-dir" (
        SET TESTS_DIR=%2
        SHIFT
    )
    SHIFT
    goto :loop
)

SET ACTIVE_TEST_SYMNAME=active-test

echo %MAKE_ACTIVE_TEST_NAME%>%TESTS_DIR%/%ACTIVE_TEST_SYMNAME%.txt

@REM del "%TESTS_DIR%/%ACTIVE_TEST_SYMNAME%.lnk"
@REM powershell "$s=(New-Object -COM WScript.Shell).CreateShortcut('%TESTS_DIR%\\%ACTIVE_TEST_SYMNAME%.lnk');$s.TargetPath='%TESTS_DIR%\\%MAKE_ACTIVE_TEST_NAME%.c';$s.Save()"
@REM mklink "%TESTS_DIR%/%ACTIVE_TEST_SYMNAME%.c" "%TESTS_DIR%/%MAKE_ACTIVE_TEST_NAME%.c"
