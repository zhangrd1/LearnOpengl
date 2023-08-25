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
    // ������ռ���������

    vec3 result = CalcPointLight(pointlight, material);
    vec3 result += CalcSpotLight(spotlight, material);

    FragColor = vec4(result, 1.0f);
}

struct calLightTemp
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 CalcSpotLight(SpotLight light, Material material)
{
    // �۹�ƣ�û��˥��
    vec3  lightDir   = normalize(light.position - FragPos);
    float theta      = dot(lightDir, normalize(-light.direction));
    float epsilon    = light.cutoff - light.outerCutoff;
    float intensity  = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);

    vec3 normal = normalize(Normal);

    // ������ɫ
    vec3 objectDiffuse  = vec3(texture(material.diffuse, TexCoord));
    vec3 objectSpecular = vec3(texture(material.specular, TexCoord));

    // ������
    vec3 ambient = light.ambient * objectDiffuse; 

    // ������
    float diff = max(dot(normal, lightDir), .0f);
    vec3 diffuse = diff * light.diffuse * objectDiffuse;

    // ���淴��
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

    // ˥��ֵ
    float distance = length(light.position - FragPos);
    float atten = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);

    vec3 normal = normalize(Normal);

    // ������ɫ
    vec3 objectDiffuse  = vec3(texture(material.diffuse, TexCoord));
    vec3 objectSpecular = vec3(texture(material.specular, TexCoord));

    // ������
    vec3 ambient = light.ambient * objectDiffuse; 

    // ������
    float diff = max(dot(normal, lightDir), .0f);
    vec3 diffuse = diff * light.diffuse * objectDiffuse;

    // ���淴��
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

    // ������ɫ
    vec3 objectDiffuse  = vec3(texture(material.diffuse, TexCoord));
    vec3 objectSpecular = vec3(texture(material.specular, TexCoord));

    // ������
    vec3 ambient = light.ambient * objectDiffuse; 

    // ������
    float diff = max(dot(normal, lightDir), .0f);
    vec3 diffuse = diff * light.diffuse * objectDiffuse;

    // ���淴��
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(cameraPos - FragPos);
    float spec = pow(max(dot(viewDir, reflectDir), .0f), material.shininess);
    vec3 specular = spec * light.specular * objectSpecular;
   
    vec3 result = ambient + diffuse + specular;
    return result;
}






















//#version 330 core
//out vec4 FragColor;

//struct Material {
//    sampler2D diffuse;
//    sampler2D specular;
//    float shininess;
//}; 

//struct DirLight {
//    vec3 direction;
	
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
//};

//struct PointLight {
//    vec3 position;
    
//    float constant;
//    float linear;
//    float quadratic;
	
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
//};

//struct SpotLight {
//    vec3 position;
//    vec3 direction;
//    float cutOff;
//    float outerCutOff;
  
//    float constant;
//    float linear;
//    float quadratic;
  
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;       
//};

//#define NR_POINT_LIGHTS 4

//in vec3 FragPos;
//in vec3 Normal;
//in vec2 TexCoords;

//uniform vec3 viewPos;
//uniform DirLight dirLight;
//uniform PointLight pointLights[NR_POINT_LIGHTS];
//uniform SpotLight spotLight;
//uniform Material material;

//// function prototypes
//vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
//vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

//void main()
//{    
//    // properties
//    vec3 norm = normalize(Normal);
//    vec3 viewDir = normalize(viewPos - FragPos);
    
//    // == =====================================================
//    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
//    // For each phase, a calculate function is defined that calculates the corresponding color
//    // per lamp. In the main() function we take all the calculated colors and sum them up for
//    // this fragment's final color.
//    // == =====================================================
//    // phase 1: directional lighting
//    vec3 result = CalcDirLight(dirLight, norm, viewDir);
//    // phase 2: point lights
//    for(int i = 0; i < NR_POINT_LIGHTS; i++)
//        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
//    // phase 3: spot light
//    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
//    FragColor = vec4(result, 1.0);
//}

//// calculates the color when using a directional light.
//vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
//{
//    vec3 lightDir = normalize(-light.direction);
//    // diffuse shading
//    float diff = max(dot(normal, lightDir), 0.0);
//    // specular shading
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    // combine results
//    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
//    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
//    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
//    return (ambient + diffuse + specular);
//}

//// calculates the color when using a point light.
//vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
//{
//    vec3 lightDir = normalize(light.position - fragPos);
//    // diffuse shading
//    float diff = max(dot(normal, lightDir), 0.0);
//    // specular shading
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    // attenuation
//    float distance = length(light.position - fragPos);
//    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
//    // combine results
//    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
//    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
//    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
//    ambient *= attenuation;
//    diffuse *= attenuation;
//    specular *= attenuation;
//    return (ambient + diffuse + specular);
//}

//// calculates the color when using a spot light.
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
//{
//    vec3 lightDir = normalize(light.position - fragPos);
//    // diffuse shading
//    float diff = max(dot(normal, lightDir), 0.0);
//    // specular shading
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    // attenuation
//    float distance = length(light.position - fragPos);
//    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
//    // spotlight intensity
//    float theta = dot(lightDir, normalize(-light.direction)); 
//    float epsilon = light.cutOff - light.outerCutOff;
//    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
//    // combine results
//    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
//    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
//    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
//    ambient *= attenuation * intensity;
//    diffuse *= attenuation * intensity;
//    specular *= attenuation * intensity;
//    return (ambient + diffuse + specular);
//}