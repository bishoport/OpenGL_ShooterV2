@echo off
setlocal

REM Set paths assuming they are in the same directory as the .bat file
set TEMPLATE_PROJECT_PATH="%~dp0template.vcxproj"
set SRC_DIR="%~dp0src"
set MAIN_CPP_PATH="%~dp0main.cpp"
set LIBRARY_PATH="%~dp0Core.lib"

REM Ask the user for the name of the new solution
set /p NEW_SOLUTION_NAME="Enter the name of the new solution: "

REM Ask the user for the name of the new project
set /p NEW_PROJECT_NAME="Enter the name of the new project: "

REM Ask the user for the path where the new solution should be created
set /p SOLUTION_DIR="Enter the directory where the new solution should be created (e.g., C:\Path\To\Your\NewSolution): "

REM Create the new solution directory
echo Creating new solution directory...
mkdir "%SOLUTION_DIR%"

REM Navigate to the solution directory
cd "%SOLUTION_DIR%"

REM Create basic solution file
echo Creating new solution...
echo Microsoft Visual Studio Solution File, Format Version 12.00 > "%NEW_SOLUTION_NAME%.sln"
echo # Visual Studio Version 17 >> "%NEW_SOLUTION_NAME%.sln"
echo VisualStudioVersion = 17.0.31903.59 >> "%NEW_SOLUTION_NAME%.sln"
echo MinimumVisualStudioVersion = 10.0.40219.1 >> "%NEW_SOLUTION_NAME%.sln"

REM Create a new project directory
mkdir "%NEW_PROJECT_NAME%"
cd "%NEW_PROJECT_NAME%"

REM Copy the template project file to the new project directory and rename it
copy %TEMPLATE_PROJECT_PATH% "%NEW_PROJECT_NAME%.vcxproj"

REM Modify the copied project file with the new project name
powershell -Command "(Get-Content '%NEW_PROJECT_NAME%.vcxproj') -replace 'template', '%NEW_PROJECT_NAME%' | Set-Content '%NEW_PROJECT_NAME%.vcxproj'"

REM Add the Core.lib file to the project
echo Adding Core.lib to the project...
powershell -Command "(Get-Content .\%NEW_PROJECT_NAME%.vcxproj) -replace '<AdditionalDependencies>', '<AdditionalDependencies>%LIBRARY_PATH%;' | Set-Content .\%NEW_PROJECT_NAME%.vcxproj"

REM Create the assets directory
echo Creating assets directory...
mkdir assets

REM Copy the src directory and its contents to the new project directory
echo Copying src directory...
xcopy /E /I %SRC_DIR% "%SOLUTION_DIR%\%NEW_PROJECT_NAME%\src"

REM Copy main.cpp to the new project directory
echo Copying main.cpp...
copy %MAIN_CPP_PATH% "%SOLUTION_DIR%\%NEW_PROJECT_NAME%\main.cpp"

REM Add include directories (if needed)
set /p INCLUDE_PATH="Enter any additional include directories (or leave blank if none): "
if not "%INCLUDE_PATH%"=="" (
    powershell -Command "(Get-Content .\%NEW_PROJECT_NAME%.vcxproj) -replace '<AdditionalIncludeDirectories>', '<AdditionalIncludeDirectories>%INCLUDE_PATH%;' | Set-Content .\%NEW_PROJECT_NAME%.vcxproj"
)

REM Ask if the user wants to build the new project now
set /p BUILD_NOW="Do you want to build the new project now? (y/n): "

if /i "%BUILD_NOW%"=="y" (
    echo Building the new project...
    msbuild .\%NEW_PROJECT_NAME%.vcxproj /p:Configuration=Debug
) else (
    echo You chose not to build the project now.
)

echo All done!
pause
