#include "Config.h"
#include "settings.h"
#include "raylib.h"
#include "scenes.h"
#include <vector>
using namespace std;


Scene
Settings()
{
  int display = GetCurrentMonitor();
  int screenWidthMax = GetMonitorWidth(display);
  int screenHeightMax = GetMonitorHeight(display);

  int screenW[] = { 720, 1280, 1920 };
  int screenH[] = { 480, 720, 1080 };
  static int i = 0;
  static bool fullscreenFlag = false;

  Rectangle set1 = {
    (float)screenWidth / 2 - 100, (float)screenHeight / 2 - 125, 200, 75
  };
  Rectangle set2 = {
    (float)screenWidth / 2 - 150, (float)screenHeight / 2 - 125, 50, 75
  };
  Rectangle set3 = {
    (float)screenWidth / 2 + 100, (float)screenHeight / 2 - 125, 50, 75
  };
  Rectangle full = {
    (float)screenWidth / 2 - 150, (float)screenHeight / 2 , 300, 75
  };
  Rectangle back = {
    (float)screenWidth / 2 - 150, (float)screenHeight / 2 + 100, 300, 75
  };

  if (CheckCollisionPointRec(GetMousePosition(), set2)) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && i < 2 &&
        fullscreenFlag == false && screenW[i+1] <= screenWidthMax &&
        screenH[i+1] <= screenHeightMax) {
      i++;
      screenWidth = screenW[i];
      screenHeight = screenH[i];
      SetWindowSize(screenWidth, screenHeight);
    }
  }
  if (CheckCollisionPointRec(GetMousePosition(), set3)) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && i > 0 &&
        fullscreenFlag == false) {
      i--;
      screenWidth = screenW[i];
      screenHeight = screenH[i];
      SetWindowSize(screenWidth, screenHeight);
    }
  }
  if (CheckCollisionPointRec(GetMousePosition(), full)) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      if (!fullscreenFlag) {
        fullscreenFlag = true;
        SetWindowSize(screenWidthMax, screenHeightMax);
        screenWidth = screenWidthMax;
        screenHeight = screenHeightMax;
        ToggleFullscreen();
        
      } else {
        ToggleFullscreen();
        fullscreenFlag = false;
        screenWidth = screenW[i];
        screenHeight = screenH[i];
        SetWindowSize(screenWidth, screenHeight);
      }
    }
  }

  if (CheckCollisionPointRec(GetMousePosition(), back)) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      return Scene::INTRO;
    }
  }

  ClearBackground(RAYWHITE);
  DrawRectangleRec(set1, PURPLE);
  DrawRectangleRec(set2, RED);
  DrawRectangleRec(set3, GREEN);
  DrawRectangleRec(full, YELLOW);
  DrawRectangleRec(back, BLUE);

  DrawText("+",
           set2.x + set2.width / 2 - MeasureText("+", 30) / 2,
           set2.y + set2.height / 2 - 10,
           30,
           DARKGRAY);
  DrawText("Resolution",
           screenWidth / 2 - MeasureText("Resolution", 30) / 2,
           set1.y + set1.height / 2 - 10,
           30,
           DARKGRAY);
  DrawText("-",
           set3.x + set3.width / 2 - MeasureText("-", 30) / 2,
           set3.y + set3.height / 2 - 10,
           30,
           DARKGRAY);
  DrawText("Toggle Fullscreen",
           full.x + full.width / 2 - MeasureText("Toggle Fullscreen", 30) / 2,
           full.y + full.height / 2 - 10,
           30,
           DARKGRAY);
  DrawText("Back",
           back.x + back.width / 2 - MeasureText("Back", 30) / 2,
           back.y + back.height / 2 - 10,
           30,
           DARKGRAY);

  return Scene::SETTINGS;
}