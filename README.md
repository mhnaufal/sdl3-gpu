# BUBUK | Game Engine Development is CRAZY

My attempt to create some sort of "game engine" from _scratch_ by adding libraries that already exist out there.

| Category              | Tools                                                                           |
| --------------------- | ------------------------------------------------------------------------------- |
| Rendering             | [SDL3](https://wiki.libsdl.org/SDL3/FrontPage)                                  |
|                       | [The Forge Framework](https://github.com/ConfettiFX/The-Forge/wiki/)            |
|                       | [NVRHI](https://github.com/NVIDIA-RTX/NVRHI)                                    |
| Audio                 | [FMOD](https://www.fmod.com/docs/2.03/api/white-papers.html)                    |
| Logging               | ?                                                                               |
| GUI                   | [Dear ImGUI](https://github.com/ocornut/imgui)                                  |
| 3D Object/Mesh Loader | ?                                                                               |
| Math                  | [GLM](https://www.fmod.com/docs/2.03/api/white-papers.html)                     |
| Physic                | [PhysX](https://nvidia-omniverse.github.io/PhysX/physx/5.6.0/docs/Startup.html) |
| Skeletal Animation    | [Ozz](http://guillaumeblanc.github.io/ozz-animation/documentation)              |

## Documentations
First checkpoint achieved. Able to include most of the dependencies to the project and build & run on Windows x64.

Next target would be build & run on Android.

<img src="./docs/checkpoint1.gif" width="400" height="300" />

## Tutorial

### SDL 3 GPU Vulkan 🦄
#### https://www.youtube.com/watch?v=tfc3vschDVw&list=PLI3kBEQ3yd-CbQfRchF70BPLF9G1HEzhy
#### https://wiki.libsdl.org/SDL3/CategoryGPU

---

### Pikuma C Rasterize 🦖
#### https://www.youtube.com/watch?v=k5wtuKWmV48

1. Minimum Vulkan version is `1.4.309`
2. Issue in previous Vulkan version: [issue](https://github.com/KhronosGroup/Vulkan-ValidationLayers/issues/7258)
