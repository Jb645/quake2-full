echo off
set gamedllPath="release\gamex86.dll"
set quake2PathLaptop="C:\Program Files (x86)\Steam\steamapps\common\Quake 2\mod"
set gameAddsDesk="..\customMod\Adds"


if exist %quake2PathLaptop% (
xcopy /s /y %gameAddsDesk% %quake2PathLaptop%
xcopy /s /y %gamedllPath% %quake2PathLaptop%

)

set ShortcutPath="..\customMod\quake2ShortCut"

if exist %quake2PathLaptop% (
start "" %ShortcutPath%
)






set gamedllDesk="release\gamex86.dll"
set quakePathDesk="D:\SteamLibrary\steamapps\common\Quake 2\mod"


set ShortcutPathDesk="..\customMod\quake2ShortCutDesk"

if exist %quakePathDesk% (
xcopy /s /y %gameAddsDesk% %quakePathDesk%
xcopy /s /y %gamedllDesk% %quakePathDesk%
)

if exist %quakePathDesk% (
start "" %ShortcutPathDesk%
)
