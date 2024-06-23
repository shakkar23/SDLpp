# SDL2 Window

###### Hello! Welcome to my SDL2 Window class!
Feel free to suggest additions, and changes to the class to encapsulate more of SDL2, comment on the specifics of each function, or even clean the already existing code!


## Window features
- NxM window size
- Texture rendering
- clearing the screen
- displaying the screen to the user
- creating textures from surfaces
- stack based coloring system
- drawing primitives such as
  - wire frame rectangles
  - filled rectangles
  - wire frame circles
- bad text rendering

## Dependencies
- SDL2
- SDL2_TTF

## TODO
- make the class a header only library
- add macro(s) specifying if we should include the TTF functions in the active source
- add more SDL2 functionality related to the SDL `SDL_Window` and `SDL_Renderer` to the class
- move out unrelated but useful functions to its own header only library