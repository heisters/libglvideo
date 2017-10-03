-- Vertex.GL3

in vec4 Position;
in vec3 Normal;

uniform mat4 Projection;
uniform mat4 Modelview;
uniform mat3 NormalMatrix;
uniform vec3 DiffuseMaterial;

out vec3 EyespaceNormal;
out vec3 Diffuse;

void main()
{
    EyespaceNormal = NormalMatrix * Normal;
    gl_Position = Projection * Modelview * Position;
    Diffuse = DiffuseMaterial;
}

-- Fragment.GL3

in vec3 EyespaceNormal;
in vec3 Diffuse;
out vec4 FragColor;

uniform vec3 LightPosition;
uniform vec3 AmbientMaterial;
uniform vec3 SpecularMaterial;
uniform float Shininess;

void main()
{
    vec3 N = normalize(EyespaceNormal);
    vec3 L = normalize(LightPosition);
    vec3 Eye = vec3(0, 0, 1);
    vec3 H = normalize(L + Eye);
    
    float df = max(0.0, dot(N, L));
    float sf = max(0.0, dot(N, H));
    sf = pow(sf, Shininess);

    vec3 color = AmbientMaterial + df * Diffuse + sf * SpecularMaterial;
    FragColor = vec4(color, 1.0);
}

-- Vertex.GL2

attribute vec4 Position;
attribute vec3 Normal;

uniform mat4 Projection;
uniform mat4 Modelview;
uniform mat3 NormalMatrix;
uniform vec3 DiffuseMaterial;

varying vec3 EyespaceNormal;
varying vec3 Diffuse;

void main()
{
    EyespaceNormal = NormalMatrix * Normal;
    gl_Position = Projection * Modelview * Position;
    Diffuse = DiffuseMaterial;
}

-- Fragment.GL2

varying vec3 EyespaceNormal;
varying vec3 Diffuse;

uniform vec3 LightPosition;
uniform vec3 AmbientMaterial;
uniform vec3 SpecularMaterial;
uniform float Shininess;

void main()
{
    vec3 N = normalize(EyespaceNormal);
    vec3 L = normalize(LightPosition);
    vec3 Eye = vec3(0, 0, 1);
    vec3 H = normalize(L + Eye);
    
    float df = max(0.0, dot(N, L));
    float sf = max(0.0, dot(N, H));
    sf = pow(sf, Shininess);

    vec3 color = AmbientMaterial + df * Diffuse + sf * SpecularMaterial;
    gl_FragColor = vec4(color, 1.0);
}
