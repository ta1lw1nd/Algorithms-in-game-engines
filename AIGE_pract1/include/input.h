#pragma once
#include "raylib.h"

struct KeyBindings{
    KeyboardKey debugToggle;
    KeyboardKey fullscreenToggle;
    KeyboardKey cameraResetPosition;
    KeyboardKey moveLeft;
    KeyboardKey moveRight;
    KeyboardKey moveUp;
    KeyboardKey moveDown;
    KeyboardKey jump;
};

extern KeyBindings keyMap;
