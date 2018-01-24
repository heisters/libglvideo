-----------------------
   C   O   L   O   R   
-----------------------

-- Vertex.GL3

in vec2 Position;
in vec3 InColor;
out vec3 OutColor;

void main()
{
    OutColor = InColor;
    gl_Position = vec4(Position, 0, 1);
}

-- Fragment.GL3

in vec3 OutColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(OutColor, 1);
}

-- Vertex.GL2

attribute vec2 Position;
attribute vec3 InColor;
varying vec3 OutColor;

void main()
{
    OutColor = InColor;
    gl_Position = vec4(Position, 0, 1);
}

-- Fragment.GL2

varying vec3 OutColor;

void main()
{
    gl_FragColor = vec4(OutColor, 1);
}

--------------------------------
   T   E   X   T   U   R   E
--------------------------------

-- Vertex.Textured.GL3

in vec2 Position;
in vec2 InCoord;
out vec2 OutCoord;

void main()
{
    OutCoord = InCoord;
    gl_Position = vec4(Position, 0, 1);
}

-- Fragment.Textured.GL3

in vec2 OutCoord;
out vec4 FragColor;
uniform sampler2D Sampler;

void main()
{
    FragColor = texture(Sampler, OutCoord);
}

-- Vertex.Textured.GL2

attribute vec2 Position;
attribute vec2 InCoord;
varying vec2 OutCoord;

void main()
{
    OutCoord = InCoord;
    gl_Position = vec4(Position, 0, 1);
}

-- Fragment.Textured.GL2

varying vec2 OutCoord;
uniform sampler2D Sampler;

void main()
{
    gl_FragColor = texture2D(Sampler, OutCoord);
}
