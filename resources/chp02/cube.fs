#version 330 core
// 片段经过模型变换后的位置
in vec3 FragPos;
// 片段法向量
in vec3 Normal;
// 片段纹理坐标
in vec2 TexCoords;

struct Material {
    // 将vec3漫反射颜色向量替换为漫反射贴图
    // sampler2D只能通过『uniform』方式进行实例化
    // 环境光材质颜色设置成与漫反射颜色相同
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;  // 物体高光反射度
}; 

struct Light {
    vec3 position;

    vec3 ambient;  // 光源自身的环境光『颜色分量』
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

// uniform vec3 objectColor;
// uniform vec3 lightColor;
// uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    // 计算环境光分量
    // -------------
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));  // lightColor -> light.ambient
    
    // 计算漫反射分量
    // -----------
    vec3 norm = normalize(Normal);
    // 计算光线到片段的方向向量
    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));  // lightColor -> light.diffuse

    // 计算镜面光照分量
    // ---------------
    float specularStrength = 0.9;
    // 计算观察向量
    vec3 viewDir = normalize(viewPos - FragPos);
    // 计算反射光向量
    vec3 reflectDir = reflect(-lightDir, norm);
    // 计算镜面分量
    // 32是高光的反光度(Shininess)。一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));  // lightColor -> light.specular

    // 计算放射光分量
    // -------------
    vec3 emission = vec3(texture(material.emission, TexCoords));
    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0);
}