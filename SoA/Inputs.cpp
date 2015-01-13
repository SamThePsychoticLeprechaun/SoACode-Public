#include "stdafx.h"
#include "Inputs.h"

#include "GameManager.h"
#include "InputManager.h"

// Input Commands Sorted Alphabetically
i32 INPUT_BACKWARD = -1;
i32 INPUT_BLOCK_DRAG = -1;
i32 INPUT_BLOCK_SCANNER = -1;
i32 INPUT_CROUCH = -1;
i32 INPUT_DEBUG = -1;
i32 INPUT_DRAW_MODE = -1;
i32 INPUT_FLASH_LIGHT = -1;
i32 INPUT_FLY = -1;
i32 INPUT_FORWARD = -1;
i32 INPUT_GRID = -1;
i32 INPUT_HORIZONTAL = -1;
i32 INPUT_HUD = -1;
i32 INPUT_INVENTORY = -1;
i32 INPUT_JUMP = -1;
i32 INPUT_LEFT = -1;
i32 INPUT_MARKER = -1;
i32 INPUT_MEGA_SPEED = -1;
i32 INPUT_MOUSE_LEFT = -1;
i32 INPUT_MOUSE_RIGHT = -1;
i32 INPUT_NIGHT_VISION = -1;
i32 INPUT_NIGHT_VISION_RELOAD = -1;
i32 INPUT_PAUSE = -1;
i32 INPUT_PHYSICS_BLOCK_UPDATES = -1;
i32 INPUT_PLANET_DRAW_MODE = -1;
i32 INPUT_PLANET_ROTATION = -1;
i32 INPUT_RANDOM_DEBUG = -1;
i32 INPUT_RELOAD_BLOCKS = -1;
i32 INPUT_RELOAD_SHADERS = -1;
i32 INPUT_RELOAD_TEXTURES = -1;
i32 INPUT_RELOAD_UI = -1;
i32 INPUT_RIGHT = -1;
i32 INPUT_SCAN_WSO = -1;
i32 INPUT_SONAR = -1;
i32 INPUT_SPRINT = -1;
i32 INPUT_UPDATE_FRUSTUM = -1;
i32 INPUT_VERTICAL = -1;
i32 INPUT_WATER_UPDATE = -1;
i32 INPUT_ZOOM = -1;

// Reduce Some Code
#define CREATE_INPUT(ID,KEY,VAR) \
    VAR = inputManager->createAxis(#ID, KEY);

#define CREATE_INPUT_D(ID, KEY_NEG, KEY_POS, VAR) \
    VAR = inputManager->createAxis(#ID, KEY_NEG, KEY_POS);

// Generate Input Handles
void initInputs(InputManager* inputManager) {
    CREATE_INPUT(Random Debug, VKEY_6, INPUT_RANDOM_DEBUG);

    // The Greatest Input In The Cosmos
    CREATE_INPUT(Pause, VKEY_ESCAPE, INPUT_PAUSE);

    // Game Information
    CREATE_INPUT(Debug, VKEY_H, INPUT_DEBUG);
    CREATE_INPUT(Inventory, VKEY_TAB, INPUT_INVENTORY);
    CREATE_INPUT(HUD, VKEY_T, INPUT_HUD);

    // Visual Aid
    CREATE_INPUT(Zoom, VKEY_RCTRL, INPUT_ZOOM);
    CREATE_INPUT(Sonar, VKEY_R, INPUT_SONAR);
    CREATE_INPUT(Flash Light, VKEY_L, INPUT_FLASH_LIGHT);
    CREATE_INPUT(Night Vision, VKEY_N, INPUT_NIGHT_VISION);

    // Refreshing Functions
    CREATE_INPUT(Reload Textures, VKEY_F4, INPUT_RELOAD_TEXTURES);
    CREATE_INPUT(Reload Blocks, VKEY_F6, INPUT_RELOAD_BLOCKS);
    CREATE_INPUT(Reload Shaders, VKEY_F11, INPUT_RELOAD_SHADERS);
    CREATE_INPUT(Reload UI, VKEY_F5, INPUT_RELOAD_UI);
    CREATE_INPUT(Reload Night Vision, VKEY_F3, INPUT_NIGHT_VISION_RELOAD);

    // Visual Debugging
    CREATE_INPUT(Grid Toggle, VKEY_G, INPUT_GRID);
    CREATE_INPUT(Draw Mode, VKEY_M, INPUT_DRAW_MODE);
    CREATE_INPUT(Planet Draw Mode, VKEY_J, INPUT_PLANET_DRAW_MODE);
    CREATE_INPUT(Update Frustum, VKEY_U, INPUT_UPDATE_FRUSTUM);

    // Movement
    CREATE_INPUT(Fly, VKEY_F, INPUT_FLY);
    CREATE_INPUT_D(Vertical, VKEY_W, VKEY_S, INPUT_VERTICAL);
    CREATE_INPUT_D(Horizontal, VKEY_D, VKEY_A, INPUT_HORIZONTAL);
    CREATE_INPUT(Sprint, VKEY_LSHIFT, INPUT_SPRINT);
    CREATE_INPUT(Crouch, VKEY_LCTRL, INPUT_CROUCH);
    CREATE_INPUT(Mega Speed, VKEY_LALT, INPUT_MEGA_SPEED);
    CREATE_INPUT(Jump, VKEY_SPACE, INPUT_JUMP);
    CREATE_INPUT(Forward, VKEY_W, INPUT_FORWARD);
    CREATE_INPUT(Left, VKEY_A, INPUT_LEFT);
    CREATE_INPUT(Right, VKEY_D, INPUT_RIGHT);
    CREATE_INPUT(Backward, VKEY_S, INPUT_BACKWARD);

    // Gameplay
    CREATE_INPUT(Marker, VKEY_C, INPUT_MARKER);
    CREATE_INPUT(Scan WSO, VKEY_LEFTBRACKET, INPUT_SCAN_WSO);

    // Physics
    CREATE_INPUT(Water Update, VKEY_N, INPUT_WATER_UPDATE);
    CREATE_INPUT(Update Physics Blocks, VKEY_P, INPUT_PHYSICS_BLOCK_UPDATES);
    CREATE_INPUT_D(Planet Rotation, VKEY_1, VKEY_2, INPUT_PLANET_ROTATION);
    
    // Mouse Buttons
    CREATE_INPUT(Mouse Right, (VirtualKey)SDL_BUTTON_RIGHT, INPUT_MOUSE_RIGHT);
    CREATE_INPUT(Mouse Left, (VirtualKey)SDL_BUTTON_LEFT, INPUT_MOUSE_LEFT);
    
    // Block Utilities
    CREATE_INPUT(Block Scanner, VKEY_Q, INPUT_BLOCK_SCANNER);
    CREATE_INPUT(Block Select, VKEY_B, INPUT_BLOCK_DRAG);
}