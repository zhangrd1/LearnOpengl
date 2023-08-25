#version 330 core
out vec4 FragColor;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float linear;
    float quadratic;
};

struct SpotLight
{
    vec3 direction;
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutoff;
    float outerCutoff;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 cameraPos;

uniform Material material;
uniform SpotLight spotlight;
uniform PointLight pointlight;
uniform DirLight dirlight;

vec3 CalcSpotLight(SpotLight, Material);
vec3 CalcPointLight(PointLight, Material);
vec3 CalcDirLight(DirLight, Material);

void main()
{
    // 在世界空间里做运算
    vec3 result;

    result = CalcPointLight(pointlight, material);
    result += CalcSpotLight(spotlight, material);

    FragColor = vec4(result, 1.0f);
}

vec3 CalcSpotLight(SpotLight light, Material material)
{
    // 聚光灯，没有衰减
    vec3  lightDir   = normalize(light.position - FragPos);
    float theta      = dot(lightDir, normalize(-light.direction));
    float epsilon    = light.cutoff - light.outerCutoff;
    float intensity  = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);

    vec3 normal = normalize(Normal);

    // 物体颜色
    vec3 objectDiffuse  = vec3(texture(material.diffuse, TexCoord));
    vec3 objectSpecular = vec3(texture(material.specular, TexCoord));

    // 环境光
    vec3 ambient = light.ambient * objectDiffuse; 

    // 漫反射
    float diff = max(dot(normal, lightDir), .0f);
    vec3 diffuse = diff * light.diffuse * objectDiffuse;

    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(cameraPos - FragPos);
    float spec = pow(max(dot(viewDir, reflectDir), .0f), material.shininess);
    vec3 specular = spec * light.specular * objectSpecular;

    vec3 result = ambient + diffuse * intensity + specular * intensity;
    return result;
}

vec3 CalcPointLight(PointLight light, Material material)
{
    vec3 lightDir   = normalize(light.position - FragPos);

    // 衰减值
    float distance = length(light.position - FragPos);
    float atten = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);

    vec3 normal = normalize(Normal);

    // 物体颜色
    vec3 objectDiffuse  = vec3(texture(material.diffuse, TexCoord));
    vec3 objectSpecular = vec3(texture(material.specular, TexCoord));

    // 环境光
    vec3 ambient = light.ambient * objectDiffuse; 

    // 漫反射
    float diff = max(dot(normal, lightDir), .0f);
    vec3 diffuse = diff * light.diffuse * objectDiffuse;

    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(cameraPos - FragPos);
    float spec = pow(max(dot(viewDir, reflectDir), .0f), material.shininess);
    vec3 specular = spec * light.specular * objectSpecular;

    vec3 result = ambient + diffuse * atten + specular * atten;
    return result;
}

vec3 CalcDirLight(DirLight light, Material material)
{
    vec3 lightDir = normalize(-light.direction);

    vec3 normal = normalize(Normal);

    // 物体颜色
    vec3 objectDiffuse  = vec3(texture(material.diffuse, TexCoord));
    vec3 objectSpecular = vec3(texture(material.specular, TexCoord));

    // 环境光
    vec3 ambient = light.ambient * objectDiffuse; 

    // 漫反射
    float diff = max(dot(normal, lightDir), .0f);
    vec3 diffuse = diff * light.diffuse * objectDiffuse;

    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(cameraPos - FragPos);
    float spec = pow(max(dot(viewDir, reflectDir), .0f), material.shininess);
    vec3 specular = spec * light.specular * objectSpecular;
   
    vec3 result = ambient + diffuse + specular;
    return result;
}