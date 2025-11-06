# Outpost Zero
## Brief
A 2D tower defense game where players deploy turrets along a fixed path to stop enemy waves. Defeating enemies earns resources to build and upgrade defenses. Periodic boss waves provide bonus rewards. Featuring multiple enemy and turret types that offer wide variety to actions performable in-game. 

## Instructions on how to run the game
### Pre-requisites
- Instructions only for POSIX compliant systems - windows support later maybe idk im not smart enough
- raylib must be installed on the system. raygui is a library (also required) but is included in /src. 
- cpp compiler. C++17+ 
- This project uses a Makefile for builds. Ensure GNU Make is installed. 
--- 
- Clone the entire repo. 
- Either run `make` while in the main directory, or run the script `run.sh`
- if ran `make`, run the final executable as `$ ./build/output`

## Game details
- This game is a simplistic sci-fi tower defense game. 
- Enemies spawn at one point, and travel a pre-defined path towards the end. 
- Once the enemies cross the entire path, they deplete your health, 1 per enemy. 
- There are three types of turrets provided - 
    - Projectile based
    - Laser based
    - Status effect based
- Each turret has 3 tiers which are in increasing order of power and efficacy. (total 9 turrets)
- The textures for turrets are taken from the open-source game "Mindustry"
- While their functionality is NOT the same as in mindustry, it takes close inspiration. 
- Place turrets at strategic location to kill all enemies before they reach your base. 


## Code structure
- There is a `main.cpp` that is responsible for handling what screen you are currently on. The main game loop run here
    - Either it displays intro, or settings, or the game.
    - The game (for now) does not implement variable resolution settings, HOWEVER, the resolution is changeable in settings
- function Game() defined in Game.cpp is called per frame to show the game. 
- Turrets, Projectiles, Enemies are handled in different files, declaring classes and using inheritance for individual entities. 
- A seperate Particles class in `Particle.h` is defined to handle particle effect for visual pleasure.
- Map functionality is handled indpendently with the help of Map.h, generation and storage, while other entities make use of it occasionally. 
- main game loop puts all entities in one array, and calls required function on it, which are overriden in their class definition and call the correct function
- smart pointers are used to manage these entities 
- GUI is drawn at end using `raygui.h` (included in the source files). 

