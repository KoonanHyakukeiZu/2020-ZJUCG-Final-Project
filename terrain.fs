#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;

uniform sampler2D texture_diffuse0;
//uniform sampler2D .....
void main()
{
    FragColor = texture(texture_diffuse0, TexCoord);
}