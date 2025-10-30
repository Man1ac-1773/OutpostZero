#include "Config.h" // Include data declaration

#include "Projectile.h"
#include "Turret.h"
#include "scenes.h"
#include "scenes/Game.h"
#include "scenes/intro.h"
#include "scenes/settings.h"
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Castle Defenders 2D");
    SetExitKey(KEY_NULL);
    // SetTargetFPS(60);

    Scene currentScene = Scene::INTRO;

    while (currentScene != Scene::EXIT && !WindowShouldClose())
    {
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
        BeginDrawing();
        switch (currentScene)
        {
        case Scene::INTRO:
            currentScene = Intro();
            break;
        case Scene::GAME:
            currentScene = Game();
            break;
        case Scene::SETTINGS:
            currentScene = Settings();
            break;
        case Scene::EXIT:
            break;
        }

        EndDrawing();
    }
    Turret::DestroyTextures();
    Projectile::DestroyTextures();
    Enemy::DestroyTextures();
    CloseWindow();
    return 0;
}
