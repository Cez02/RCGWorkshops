# RCGWorkshops
RCG (Realistic Computer Graphics) workshops from Uni 2024

## How to build

First create a build directory, go into it and run
```
cmake ..
```

Next run 

```
cmake --build . -j
```

After that the executable can be found inside `build/basic_raytracer/`

## Usage

Here are the possible options to pass into the program:

```
basictraycer [OPTIONS]
        -i  --image                     Render a still frame to a png image render.png.
        -wd --width        [N]          Set window width, default: 160.
        -ht --height       [N]          Set window height, default: 160.
        -h  --help                      Print the program usage.
        -sn --sample-count [N]          Set the raytracer sample count per pixel.
        -d  --depth        [N]          Set how many ray bounces per ray are accounted for during rendering.
        -j  --jobs         [N]          Allow N jobs to run at once when rendering
        -cp --camera-pos   [x] [y] [z]  Set the camera position to (x,y,z)
        -cr --camera-rot   [x] [y] [z]  Set the camera rotation to (x,y,z) (euler)
        -m  --model        [PATH]       Load a Wavefront OBJ model in given PATH
```

When running in realtime mode (without the `-i` flag), the controls for the camera are as follows:
- W/S - forward/backwards
- A/D - left/right
- Space/Shift - up/down

## Current features

- Embree3 raytracing
- Model loading for OBJ files although should work with other formats as well
- Possible realtime rendering via an OpenGL texture
- Configurable settings
- Pretty nice logs

## Render configurations that give nice results


### Bedroom render
```
./basicraytracer -m "./models/bedroom/bed_room.obj" -i -wd 400 -ht 400 --sample-count 6 --depth 5 -j 10 --camera-pos -2.667 0.925 2.078 --camera-rot 0 130 0
```
> The normals seem to be messed up on one of the meshes for some reason

### Bedroom render
```
./basicraytracer -m "./models/20-livingroom_obj/InteriorTest.obj" -i -wd 600 -ht 600 --sample-count 10 --depth 9 -j 10 --camera-pos -2.229 1.178 4.156 --camera-rot 0 130 0
```

### Backpack render
```
./basicraytracer -m "./models/backpack/backpack.obj" -i -wd 400 -ht 400 --sample-count 6 --depth 5 -j 10 --camera-pos 3.519 0.411 3.440 --camera-rot 0 230 0
```