#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform float TIME;
uniform float BOUNCE_X;
uniform mat4 TRANSFORM;
uniform mat4 PROJECTION;
uniform mat4 VIEW;
void main()
{
   gl_Position = PROJECTION * VIEW * TRANSFORM * vec4(aPos.x + BOUNCE_X, aPos.y, aPos.z, 1.0f);
   ourColor = abs(vec3(gl_Position));
   TexCoord = aTexCoord;
}