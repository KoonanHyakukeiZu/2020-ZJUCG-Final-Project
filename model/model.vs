#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 plane;

void main()
{
    vec4 World_Pos =  model * vec4(aPos, 1.0f);
	FragPos = vec3(World_Pos);
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_ClipDistance[0] = dot(World_Pos , plane);
    TexCoord = aTexCoords;    
    gl_Position = projection * view * World_Pos;
}