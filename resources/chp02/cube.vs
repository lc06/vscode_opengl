#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	FragPos = vec3(model * vec4(aPos, 1.0f));
	/*
	* mat3(transpose(inverse(model))):
	* 法线矩阵被定义为「模型矩阵左上角的逆矩阵的转置矩阵」
	* 用于当模型矩阵(model)存在旋转及不等比缩放时造成的
	* 法向量不再垂直于片段表面的情况的修复
	*/
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoords;
}