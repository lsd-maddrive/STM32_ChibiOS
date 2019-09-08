@echo off
::Ask

:again

set /p project_name="Set new project name: "

set /p project_name_sure="New project name: %project_name%. Are you sure? (Y/n)"

If /I "%project_name_sure%"=="n" goto again

echo "Supported targets | Index"
echo "-------------------------"
echo "STM32F767ZI       | 1"
echo "STM32F446RE       | 2"
echo "STM32F103C8T6     | 3"

set /p target_idx="Choose target board (1, 2 or 3):"

if "%target_idx%"=="1" set target_name=STM32F767ZI
if "%target_idx%"=="2" set target_name=STM32F446RE
if "%target_idx%"=="3" set target_name=STM32F103C8T6

echo "%target_name% is your target"
set new_folder="..\workspace\%project_name%"

xcopy /s %target_name% %new_folder%\

powershell -Command "(gc '%new_folder%\.project') -replace '__project_name__', '%project_name%' | Out-File '%new_folder%\.project'"

echo "Project '%project_name%' created! Have nice day =)"

timeout /t 300 > NUL
exit


