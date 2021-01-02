#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 aTexCoord;

out vec4 clipspace;
out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform float chunk_size;

const float tiling = 8;

void main()
{
    vec4 World_Pos =  vec4(aPos, 1.0f);
	FragPos = vec3(World_Pos);
    clipspace = projection * view * World_Pos;
    gl_Position = clipspace;
    TexCoord = vec2(aPos.x/chunk_size + 0.5, aPos.z/chunk_size + 0.5) * tiling;

}
