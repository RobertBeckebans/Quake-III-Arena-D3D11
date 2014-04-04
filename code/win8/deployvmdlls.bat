@echo off
rem %1 = build dir where the project is built to
rem %2 = configuration of the VMs
rem %3 = target dir

xcopy /C /F /Y /K %1\..\%2\*.dll %3

