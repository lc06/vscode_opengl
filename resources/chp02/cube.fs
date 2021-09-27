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
uniform Material material;

struct Light {
    vec3 position;  // 聚光灯位置
    vec3 direction;  // 聚光灯朝向
    float cutOff;  // 聚光灯范围，内切光角
    float outerCutOff;  // 外切光角

    vec3 ambient;  // 光源自身的环境光『颜色分量』
    vec3 diffuse;
    vec3 specular;

    // 点光源衰减公式参数
    float constant;  
    float linear;
    float quadratic;
};
uniform Light light;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    vec3 lightDir = normalize(light.position - FragPos);  // 计算光线到片段的方向向量
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);  // 将内圆锥范围内 > 1 的光强置为1; 外圆锥范围外 < 0 的光强置为0

    // 计算环境光分量
    // -------------
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));  // lightColor -> light.ambient
    
    // 计算漫反射分量
    // -----------
    vec3 norm = normalize(Normal); 
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));  // lightColor -> light.diffuse

    // 计算镜面分量
    // -----------
    vec3 viewDir = normalize(viewPos - FragPos);  // 计算观察向量
    vec3 reflectDir = reflect(-lightDir, norm);  // 计算反射光向量
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);  //32是高光的反光度(Shininess)。一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));  // lightColor -> light.specular

    // 计算放射光分量
    // -------------
    vec3 emission = vec3(texture(material.emission, TexCoords));
    //  vec3 result = ambient + diffuse + specular + emission;

    // 计算光线到片段的距离
    float distance = length(light.position - FragPos);
    // 根据公式计算光强衰减率
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                light.quadratic * (distance * distance));
    
    /*
    * intensity: 聚光灯光强衰减 
    * 计算公式: \begin{equation} I = \frac{\theta - \gamma}{\epsilon} \end{equation}
    * attenuation: 光源强度随距离衰减
    * 计算公式: \begin{equation} F_{att} = \frac{1.0}{K_c + K_l * d + K_q * d^2} \end{equation}
    */
    vec3 result = emission + attenuation * (ambient + intensity * (diffuse + specular));
    FragColor = vec4(result, 1.0);
}