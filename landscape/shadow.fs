#version 330 core

out vec4 FragColor;
void main()
{
    FragColor = vec4(1-gl_FragCoord.z,1-gl_FragCoord.z,1-gl_FragCoord.z,1.0);
    // gl_FragDepth = gl_FragCoord.z;
}