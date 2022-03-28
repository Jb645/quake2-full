echo off
set gamedllPath="release\gamex86.dll"
set quake2PathLaptop="C:\Program Files (x86)\Steam\steamapps\common\Quake 2\mod"

if exist %quake2PathLaptop% (
xcopy /s /y %gamedllPath% %quake2PathLaptop%
)

set ShortcutPath="..\customMod\quake2ShortCut"

if exist %quake2PathLaptop% (
start "" %ShortcutPath%
)