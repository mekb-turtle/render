### OpenGL program to render a text file of triangles

Each line represents a single triangle, the format is as follows:

    X1 Y1 Z1  X2 Y2 Z2  X3 Y3 Z3  0xRRGGBBAA

Coordinates are floats.
Colour is converted from hex to floats. If alpha is omitted, it will default to 0xFF.

Controls:
- Arrow keys or move left click to rotate
- <kbd>W</kbd>, <kbd>A</kbd>, <kbd>S</kbd>, <kbd>D</kbd> or move right click to move
- <kbd>I</kbd>, <kbd>O</kbd> or scroll wheel to zoom in/out
- <kbd>Q</kbd> to quit
- <kbd>R</kbd> to re-render
- <kbd>T</kbd> to reset view
- <kbd>L</kbd> to toggle lighting

Examples:
- [Cube](cube.tri)
- [Nintendo 64](n64.tri)

Node.js script to [convert from Blender .obj file](https://gist.github.com/mekb-turtle/fa1f8392b5be5922c09f5e2cfeeca034)

