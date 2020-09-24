#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform;
out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main()
{
    FragPos = vec3(transform * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(transform))) * aNormal;
    gl_Position = projection * view * transform *vec4(aPos, 1.0);
    TexCoords = vec2(aTexCoord.x, aTexCoord.y);
}
