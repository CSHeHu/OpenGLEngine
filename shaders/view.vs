#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 view;

void main()
{
    // Just pass through the vertex position
    gl_Position = view * vec4(aPos, 1.0);
}