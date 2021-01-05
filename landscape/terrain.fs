#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform sampler2D texture_diffuse1;//blendermap
uniform sampler2D texture_diffuse2;//background
uniform sampler2D texture_diffuse3;//r
uniform sampler2D texture_diffuse4;//g
uniform sampler2D texture_diffuse5;//b
//uniform sampler2D .....

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 inputColor)
{
    vec3 lightDir = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4.0);
    // 合并结果
    vec3 ambient  = light.ambient  * inputColor;
    vec3 diffuse  = light.diffuse  * diff * inputColor;
    vec3 specular = light.specular * spec * inputColor;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 inputColor)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * inputColor;
    vec3 diffuse = 0.5 * light.diffuse * diff * inputColor;
    vec3 specular = 0.5 * light.specular * spec * inputColor;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{
    vec4 blendermapColour = texture(texture_diffuse1, TexCoord);

    float backgroundAmount = clamp(1 - (blendermapColour.r + blendermapColour.g + blendermapColour.b),0.0,1.0);
    vec2 tiledCoord = TexCoord * 64.0f;
    vec4 backgroundColor = texture(texture_diffuse2, tiledCoord) * backgroundAmount;
    vec4 RColor = texture(texture_diffuse3, tiledCoord) * blendermapColour.r;
    vec4 GColor = texture(texture_diffuse4, tiledCoord) * blendermapColour.g;
    vec4 BColor = texture(texture_diffuse5, tiledCoord) * blendermapColour.b;
    vec4 totalColor = backgroundColor + RColor + GColor + BColor;

	vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = CalcDirLight(dirLight, norm, viewDir, vec3(totalColor)) * 1.2f;
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, vec3(totalColor));  

	FragColor = vec4(result, 1.0);
}

