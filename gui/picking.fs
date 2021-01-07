#version 330 core

uniform uint drawIndex;
uniform uint objIndex;

out vec3 FragColor;

void main()
{
    FragColor = vec3(float(objIndex), float(drawIndex), float(gl_PrimitiveID + 1));
}