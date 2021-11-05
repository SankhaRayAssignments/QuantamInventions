# QuantamInventions

/******************************************************/
    README : MapDraw
    Author : SANKHA RAY
    Date   : 18-Apr-2016
/********************************************************/

Dependencies (Libraries):
========================
    1. glew 1.5
    2. glm 1.0
    3. glut 3.7.6
    4. FreeType 2.6.2
    5. OpenGL 2.0
    6. Visual Studio 2008

Description:
=============
    The application reads data from text file from disk and renders a Map on screen
    Map is projected (rendered) in to two views (2D/3D).
    Use Keyboard to navigate through the map

Setup and Build:
=================
    1. Untar the source packet (Source_MapDraw) using 7-zip or relevant software.
    2. Goto Source/MapDraw Directory.
    3. Open Solution File MapDraw.sln using Visual Studio 2008
    4. Build MapDraw (It will also build freetype as per build dependencies)
    5. After successful build, executable should be generated at Debug/MapDraw.exe

Execution:
=========
    Execute the Binary using Ctrl + F5 or F5 in Visual Studio 2008 after successful build.
    Notes: Copy glew32.dll and glut32.dll at same level (Debug)

    Alternately, Debug/MapDraw.exe can be executed outside Visual Studio
    Notes:
    1. Copy shaders to Debug/shaders
    2. Copy data to Debug/data
    3. Copy glew32.dll and glut32.dll at same level (Debug)
    4. Execute MapDraw.exe

Controls:
=========
    F6 - Toggles View Mode (2D/3D)
    ESC - EXIT
    -----------
    In 2D Mode:
    -----------
        W/S - Zoom In/Out
        A/D - Pan Left/Right
        Q/E - Pan Up/Down
        R/T - Rotate ClockWise/AntiClockWise
    -----------
    In 3D Mode:
    -----------
        W/S - Move Front/Back
        A/D - Move Left/Right
        R/T - Rotate ClockWise/AntiClockWise
        
FAQs:
=====
1. How to change resolution
    Open main.cpp, goto line no 25/26, change values for g_Width and g_Height (Default is 1280x720)
    Re-Build solution to create executable
2. Display driver stopped responding and has recovered
    Temporary disable Rendering of Text info.
    Open main.cpp, Goto line 13, enable macro 'DISABLE_TEXT_RENDERING'
