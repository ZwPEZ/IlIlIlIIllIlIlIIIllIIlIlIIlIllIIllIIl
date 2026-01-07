@echo off
set "IMGUI_DIR=overlay/imgui"
g++ -std=c++17 ^
    main.cpp ^
    overlay/overlay.cpp ^
    %IMGUI_DIR%/imgui.cpp ^
    %IMGUI_DIR%/imgui_demo.cpp ^
    %IMGUI_DIR%/imgui_draw.cpp ^
    %IMGUI_DIR%/imgui_tables.cpp ^
    %IMGUI_DIR%/imgui_widgets.cpp ^
    %IMGUI_DIR%/imgui_impl_win32.cpp ^
    %IMGUI_DIR%/imgui_impl_dx11.cpp ^
    -o overlay.exe ^
    -lgdi32 -ld3d11 -ldwmapi
if %errorlevel% == 0 (
    echo Build successful!
) else (
    echo Build failed.
)
pause
