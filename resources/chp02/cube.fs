#version 330 core
// 片段经过模型变换后的位置
in vec3 FragPos;
// 片段法向量
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    // 计算环境光分量
    // -------------
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // 计算漫反射分量
    // -----------
    vec3 norm = normalize(Normal);
    // 计算光线到片段的方向向量
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 计算镜面光照分量
    // ---------------
    float specularStrength = 0.9;
    // 计算观察向量
    vec3 viewDir = normalize(viewPos - FragPos);
    // 计算反射光向量
    vec3 reflectDir = reflect(-lightDir, norm);
    // 计算镜面分量
    // 32是高光的反光度(Shininess)。一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}