#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

uniform mat4 view;
uniform mat4 projection;
uniform vec4 plane;

void main()
{
	vec4 World_Pos =  vec4(aPos, 1.0f);
	FragPos = vec3(World_Pos);
    Normal = aNormal; 
	gl_ClipDistance[0] = dot(World_Pos , plane);
	gl_Position = projection * view * World_Pos;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}