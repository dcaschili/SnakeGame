# SnakeGame
Simple snake clone made in Unreal. This repository doesn't contain the assets I used, just the code.
Downloading the source zip file doesn't include the submodule, therefore it won't compile. 
To clone the repository use:
```
git clone --recurse-submodules https://github.com/dcaschili/SnakeGame.git
```

# Debug
## CVar
- **Snake.EnableOccupancyDebug**
  - true: draw boxes on the map to visualize the occupancy and some map information.
    - Green: Free
    - Red: Occupied
    - Orange: Opposite side of origin
    - Blue: Origin
  - false: disable.
- **Snake.ShowSnakeBodyColliders**
  - true: show box colliders on snake body parts (it calls SetHiddenInGame(true)).
  - false: hide box colliders

## Shader
- **MPC_Debug** (material param collection)
  - **EnableGrassBendingDebugColors**:
    - 0 -> Show standard color (default)
    - 1 -> Enable bending grass color (red == bending/ green == no bending)

# Assets
- https://www.kenney.nl/assets/food-kit
- https://www.kenney.nl/assets/interface-sounds
- https://freesound.org/people/unfa/sounds/193438/
- https://thoseawesomeguys.com/prompts/
- https://quaternius.com/packs/easyenemy.html


- https://www.dafont.com/hack.font?l[]=10
- https://www.dafont.com/nordica.font?l[]=10