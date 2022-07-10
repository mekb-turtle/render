# render
OpenGL program to render a text file of triangles

Each line represents a single triangle, the format is as follows:

    X1 Y1 Z1  X2 Y2 Z2  X3 Y3 Z3  0xRRGGBBAA

Coordinates are float.
Colour is converted from hex but used as floats. If alpha is omitted, it will default to 0xFF.

Examples:
- [Cube](cube.tri)
