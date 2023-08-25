#version 330 core
out vec4 FragColor;

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float linear;
    float quadratic;
};

uniform PointLight pointlight;

void main()
{
    FragColor = vec4(pointlight.specular, 1.0);
}