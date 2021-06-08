# CRAFT-Box2D

This is an extension to famous 2D physics engine [Box2D](https://github.com/erincatto/box2d).

box2d provides 2-dimensional physics engine for our simulations. To draw simulated objects, we extended it and add the following classes. Some of them are derived from their counterparts in box2d.

- b2VisBody
- b2VisWorld
- b2VisTexure
- b2VisPolygonShape

b2VisBody holds color and/or texture information for box2d bodies to be rendered in our own renderer. b2VisWorld is the corresponding class which knows that the attached bodies has the type of b2VisBody.

For material textures (if used), b2VisTexture depend on [libpng](http://www.libpng.org/) to load texture from file. Texture coordinates of polygons can be obtained from the interface provided in b2VisPolygonShape.

b2VisWorld has the interface to reach the renderer. By the help of this interface, you can have renderer to output rendered images to output file for each step required by the simulation.

To be able to import the simulation data from json files or to export the simulation data to json files (settings, objects, scene information etc.), following repository is used for serialization/deserialization:

[nlohmann's json serialization/deserialization library](https://github.com/nlohmann/json)

It can be installed via brew as stated in the documentation of the library if you are using OSX. Please refer readme of the repository for other operating systems.

To output video, ffmpeg is also required.

### Running a simulation

After building the executable `Testbed`, executing:
```
$ ./Testbed controller.json
```
on a directory with [`Data/Textures`](./Testbed) folder starts a simulation based on the information provided in the `controller.json`. 

Output of a simulation consists of an MPG video and a JSON file with the following format:
```
{
  "causal_graph": {
    "edges": [...],
    "nodes": [...],   (Events that occured at various steps in the simulation)
    "vis": ...        (DOT DSL source code describing the graph for visualization purposes)
  },
  "scene_states": [...]  (Object informations at the start and at the end of the simulation)
}
```