#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;
out vec4 FragPosInLightSpace;
out float visibility;

uniform mat4 view;
uniform mat4 projection;
uniform vec4 plane;

uniform mat4 lightView;
uniform mat4 lightProjection;
const float density = 0.001;
const float gradient = 1.5;

void main()
{
	vec4 World_Pos =  vec4(aPos, 1.0f);
	FragPos = vec3(World_Pos);
	FragPosInLightSpace = lightProjection * lightView * vec4(FragPos, 1.0f);
    Normal = aNormal; 
	gl_ClipDistance[0] = dot(World_Pos , plane);
	vec4 CamRelativePos = view * World_Pos;
	float CamRelativeDistance = length(CamRelativePos.xyz);
	visibility = clamp(exp(-pow((CamRelativeDistance * density), gradient)), 0.0, 1.0);
	gl_Position = projection * CamRelativePos;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}