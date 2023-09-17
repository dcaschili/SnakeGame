@echo off

rem set this variable to the UnrealEditor-Cmd.exe path.
SET editor_cmd="C:\Engines\UnrealEngine\UE_5.2\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"

rem set this variable to the .uproject file to check
SET project="C:\Develop\UnrealEngineProjects\SnakeGame\SnakeGame\SnakeGame.uproject"

rem the output file will be placed within this directory, add it to your .gitignore file.
SET dir_name="Tmp"

if not exist "%dir_name%" (
    mkdir "%dir_name%"
)

rem the program will filter the output by "error" (case insensitive) and will redirect all rows to the .txt file.
%editor_cmd% %project% -run=CompileAllBlueprints | findstr /i "error" > "Tmp\output.txt"