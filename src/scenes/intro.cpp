#include "Config.h"
#include "intro.h"
#include "raylib.h"
#include <vector>
using namespace std;

Scene Intro()
{

  // Define the box's properties
  Rectangle box1 = {
    (float)screenWidth / 2 - 125, (float)screenHeight / 2 - 125, 250, 100
  };
  Rectangle box2 = {
    (float)screenWidth / 2 - 125, (float)screenHeight / 2 - 25, 250, 100
  };
  Rectangle box3 = {
    (float)screenWidth / 2 - 125, (float)screenHeight / 2 + 75, 250, 100
  };
  vector<Rectangle> box = { box1, box2, box3 };
  float initialWidth = 250;
  float initialHeight = 50;
  float expandedWidth = 275;
  float expandedHeight = 75;
  float zoomSpeed = 4.0f;

  for (int i = 0; i < 3; i++) {
    if (CheckCollisionPointRec(GetMousePosition(), box[i])) {
      // If hovering, gradually expand
      if (box[i].width < expandedWidth)
        box[i].width += zoomSpeed;
      if (box[i].height < expandedHeight)
        box[i].height += zoomSpeed;
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        switch (i) {
          case 0:
            return Scene::GAME;
          case 1:
            return Scene::SETTINGS;
          case 2:
            return Scene::EXIT;
        }
      }
    } else {
      // If not hovering, gradually shrink
      if (box[i].width > initialWidth)
        box[i].width -= zoomSpeed;
      if (box[i].height > initialHeight)
        box[i].height -= zoomSpeed;
    }

    // Clamp the size to ensure it doesn't go beyond the limits
    if (box[i].width > expandedWidth)
      box[i].width = expandedWidth;
    if (box[i].width < initialWidth)
      box[i].width = initialWidth;
    if (box[i].height > expandedHeight)
      box[i].height = expandedHeight;
    if (box[i].height < initialHeight)
      box[i].height = initialHeight;

    // Center the box[i] as it scales
    box[i].x = (float)screenWidth / 2 - box[i].width / 2;
    box[i].y = (float)screenHeight / 2 - 75 + 100 * i - box[i].height / 2;
  }

  ClearBackground(RAYWHITE);

  DrawRectangleRec(box[0], PURPLE);
  DrawRectangleRec(box[1], RED);
  DrawRectangleRec(box[2], GREEN);
  DrawText("Enter game!",
           screenWidth / 2 - MeasureText("Enter game!", 20) / 2,
           box1.y + box1.height / 2 - 10,
           20,
           DARKGRAY);
  DrawText("Settings!",
           screenWidth / 2 - MeasureText("Settings!", 20) / 2,
           box2.y + box2.height / 2 - 10,
           20,
           DARKGRAY);
  DrawText("Exit!",
           screenWidth / 2 - MeasureText("Exit!", 20) / 2,
           box3.y + box3.height / 2 - 10,
           20,
           DARKGRAY);

  return Scene::INTRO;
}