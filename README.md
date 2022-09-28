# Hungry Birds - Computer Graphics Project 2021-2022
This is the repository containing the code for the computer graphics course's project. The project has been developed in C++ using Vulkan to build a 3D application implementing the components of a 3D application: lighting, physics, shaders, textures and object management. The team was composed of three people.

# Screenshots
Here there is an image of one scene of the application using directional lighting, ambient lighting, Lambert diffuse model and Phong specular model.

![image with directional light, ambient light, lambert diffusion and phong specular](https://github.com/marcopetri98/HungryBirds_Vulkan_CG/blob/master/images/Directional_Ambient_Lambert_Phong.png)

Here there is the same image after the bird has been launched before it collids with any object.

![image with directional light, ambient light, lambert diffuse model and phong specular model after the bird has been launched](https://github.com/marcopetri98/HungryBirds_Vulkan_CG/blob/master/images/Directional_Ambient_Lambert_Phong_Launch.png)

Here there is an image of a scene with only a spot light that is not illuminating object on the scene but just the floor.

![sport light image](https://github.com/marcopetri98/HungryBirds_Vulkan_CG/blob/master/images/Spot.png)

# Projects details

## Project delivery
Design a game in which you have to throw very hungry birds to their food.

## Project implementation
The implementation of the 3D application has been structured by building a small interface to Vulkan to add shaders, lighting and a scene descriptor to easily create scenes. The main components of the project are:
* Graphics Engine: this class is a small engine handling the usage of the API of Vulkan. It is in charge of the rendering process for the application and everything related to it. It receives classes defining the Scene in terms of: background, objects, light types, light details and shaders. There is the class ``Scene`` which contains all the details needed by the graphics engine to set up the Vulkan pipeline for rendering.
* Physics Engine: this class is a small physics engine handling the translation, rotation and collisions of objects. The physics engine contains all the objects of the active scene of the game. At each interval, the engine updates the position and rotation of every object in the scene and it verifies for collisions. To verify collisions between objects, the physics engine uses the Ray Casting technique.
* Application: this class uses the ``Graphics Engine`` and the ``Physics Engine`` to actually create the scenes and to handle the input of the user. It has a function that is periodically called by the graphics engine to check for user input. When the user press a button, the commands are executed.

## Project commands
These are the commands to change scene:

* 1: opens the first scene.
* 2: opens the second scene.
* 3: opens the third scene.

These are the commands related to lighting:

* F1: activates and deactivates directional light.
* F2: activates and deactivates point light.
* F3: activates and deactivates spot light.
* F4: activates and deactivates ambient light.
* F5: use Lambert diffuse model.
* F6: use Toon diffuse model.
* F7: use Oren-Nayar diffuse model.
* F8: use Blinn specular model.
* F9: use Phong specular model.
* F10: use Toon specular model.
* P: increase X component in the directional light direction.
* O: decrease X component in the directional light direction.

These are the commands related to aiming:

* R: reset arrow start position.
* L: launch the bird.
* W: aim higher.
* S: aim lower.
* A: aim more to the left.
* D: aim more to the right.

These are the commands related to camera movement and aim:
* C: change the camera to use (there are multiple cameras in the scenes).
* Q: rotate camera to the left.
* E: rotate camera to the right.
* :arrow_up:: move camera forward.
* :arrow_left:: move camera to the left.
* :arrow_down:: move camera backward.
* :arrow_right:: move camera to the right.
* SPACE: move camera up.
* LEFT SHIFT: move camera down.
