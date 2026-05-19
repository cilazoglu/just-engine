@echo off
SETLOCAL

SET TESTS_DIR=tests
SET ACTIVE_TEST_SYMNAME=active-test
SET /p ACTIVE_TEST=<"%TESTS_DIR%/%ACTIVE_TEST_SYMNAME%.txt"
SET OUTPUT=test.exe

echo Building active test: [%TESTS_DIR%/%ACTIVE_TEST%]
call build.bat %TESTS_DIR%/%ACTIVE_TEST%.c --out %OUTPUT%

SET LIB_DIR=justengine
SET BIN_DIR=bin
SET SUB_BIN_DIR=test

rmdir /s /q %BIN_DIR%\%SUB_BIN_DIR%     >nul 2>&1
mkdir %BIN_DIR%                         >nul 2>&1
mkdir %BIN_DIR%\%SUB_BIN_DIR%           >nul 2>&1
echo A | xcopy /q %OUTPUT% %BIN_DIR%\%SUB_BIN_DIR%\                     >nul 2>&1
echo A | xcopy /q %LIB_DIR%\bin\*.dll %BIN_DIR%\%SUB_BIN_DIR%\          >nul 2>&1

del %OUTPUT%
