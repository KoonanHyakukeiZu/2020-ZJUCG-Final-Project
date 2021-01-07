#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 aTexCoord;

out vec4 clipspace;
out vec2 TexCoord;
out vec3 FragPos;
out float visibility;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform float chunk_size;

const float tiling = 8;
const float density = 0.001;
const float gradient = 1.5;

void main()
{
    vec4 World_Pos =  vec4(aPos, 1.0f);
    vec4 CamRelativePos = view * World_Pos;
    float CamRelativeDistance = length(CamRelativePos.xyz);
	visibility = clamp(exp(-pow((CamRelativeDistance * density), gradient)), 0.0, 1.0);
	FragPos = vec3(World_Pos);
    clipspace = projection * CamRelativePos;
    gl_Position = clipspace;
    TexCoord = vec2(aPos.x/chunk_size + 0.5, aPos.z/chunk_size + 0.5) * tiling;

}
