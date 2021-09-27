#include <FreeImage/FreeImage.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <learnopengl/camera.h>
#include <learnopengl/shader.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(string path);
/** Returns
 * 1. 带默认值的形参必须放在最后；
 * 2. 带默认值的形参只需在函数声明时赋予默认值，函数定义时不能再出现赋值部分
 */
void setCubeShader(Shader shader, glm::vec3 pointLightPositions[], int index,
                   glm::vec3 pointLightColors[] = {});

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

// 相机
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// 时间
float deltaTime = 0.0f;  // 当前帧与上一帧之间的时间差
float lastFrame = 0.0f;

// 光源位置
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main() {
  // glfw配置初始化
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // 创建glfw窗口
  GLFWwindow *window =
      glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Learn OpenGL", NULL, NULL);
  if (window == NULL) {
    cout << "Failed to create GLFW window" << endl;
    glfwTerminate();
    return -1;
  }

  // 将创建的glfw窗口绑定至当前opengl绘制上下文
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // 启用鼠标捕获
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad初始化： 加载所有opengl函数指针
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD" << endl;
    return -1;
  }

  // 开启深度测试
  glEnable(GL_DEPTH_TEST);

  Shader cubeShader(
      "D:/{Code}/Code "
      "Repository/vscode_c++/openglcmake/resources/chp02/cube.vs",
      "D:/{Code}/Code "
      "Repository/vscode_c++/openglcmake/resources/chp02/cube_exercise.fs");

  Shader lightShader(
      "D:/{Code}/Code "
      "Repository/vscode_c++/openglcmake/resources/chp02/light.vs",
      "D:/{Code}/Code "
      "Repository/vscode_c++/openglcmake/resources/chp02/light.fs");

  float vertices[] = {
      // positions          // normals           // texture coords
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
      0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
      0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
      -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
      -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
      -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
      -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

  // 所有木箱的位置
  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

  // 所有点光源的位置
  glm::vec3 pointLightPositions[] = {
      glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
      glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};

  glm::vec3 pointLightColors[][4] = {
      {glm::vec3(1.0f, 0.6f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
       glm::vec3(1.0f, 1.0, 0.0), glm::vec3(0.2f, 0.2f, 1.0f)},
      {glm::vec3(0.2f, 0.2f, 0.6f), glm::vec3(0.3f, 0.3f, 0.7f),
       glm::vec3(0.0f, 0.0f, 0.3f), glm::vec3(0.4f, 0.4f, 0.4f)},
      {glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.1f, 0.1f, 0.1f),
       glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.3f, 0.1f, 0.1f)},
      {glm::vec3(0.4f, 0.7f, 0.1f), glm::vec3(0.4f, 0.7f, 0.1f),
       glm::vec3(0.4f, 0.7f, 0.1f), glm::vec3(0.4f, 0.7f, 0.1f)}};

  unsigned int cubeVAO, lightVAO, VBO;
  glGenVertexArrays(1, &cubeVAO);
  glGenVertexArrays(1, &lightVAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(cubeVAO);
  // 位置
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // 法向量
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // 纹理坐标
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(lightVAO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  unsigned int diffuseMap = loadTexture(
      "D:/{Code}/Code Repository/vscode_c++/openglcmake/assets/container2.png");
  unsigned int specularMap = loadTexture(
      "D:/{Code}/Code "
      "Repository/vscode_c++/openglcmake/assets/"
      "container2_specular.png");

  // opengl绘制虚拟循环
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    cubeShader.use();
    cubeShader.setVec3("viewPos", camera.Position);
    cubeShader.setFloat("material.shininess", 32.0f);

    setCubeShader(cubeShader, pointLightPositions, 0, pointLightColors[0]);

    // 设置投影/视图变换矩阵
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
        0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    cubeShader.setMat4("projection", projection);
    cubeShader.setMat4("view", view);

    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    cubeShader.setInt("material.diffuse", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    cubeShader.setInt("material.specular", 1);

    // 渲染盒子
    glBindVertexArray(cubeVAO);
    for (unsigned int i = 0; i < 10; i++) {
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * i;
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      cubeShader.setMat4("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    lightShader.use();

    lightShader.setMat4("projection", projection);
    lightShader.setMat4("view", view);

    // 渲染灯光
    glBindVertexArray(lightVAO);
    for (unsigned int i = 0; i < 4; i++) {
      model = glm::translate(model, pointLightPositions[i]);
      model = glm::scale(model, glm::vec3(0.2f));  // Make it a smaller cube
      lightShader.setMat4("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &lightVAO);
  glDeleteBuffers(1, &VBO);

  // glfw结束清理分配的资源
  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(string path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height;
  FIBITMAP *image;
  string ext = path.substr(strlen(path.c_str()) - 3, strlen(path.c_str()));
  int targetType, originType;
  if (ext == "png") {
    image = FreeImage_Load(FIF_PNG, path.c_str(), PNG_IGNOREGAMMA);
    targetType = GL_RGBA;
    originType = GL_BGRA;
  } else if (ext == "jpg") {
    image = FreeImage_Load(FIF_JPEG, path.c_str(), JPEG_DEFAULT);
    targetType = GL_RGB;
    originType = GL_BGR;
  }

  BYTE *data = FreeImage_GetBits(image);
  width = FreeImage_GetWidth(image);
  height = FreeImage_GetHeight(image);

  if (data) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, targetType, width, height, 0, originType,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
  }

  FreeImage_Unload(image);
  return textureID;
}

void setCubeShader(Shader cubeShader, glm::vec3 pointLightPositions[],
                   int index, glm::vec3 pointLightColors[]) {
  switch (index) {
    case 0:
      // ==============================================================================================
      //       DESERT
      // ==============================================================================================
      glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      // Directional light
      cubeShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
      cubeShader.setVec3("dirLight.ambient", 0.3f, 0.24f, 0.14f);
      cubeShader.setVec3("dirLight.diffuse", 0.7f, 0.42f, 0.26f);
      cubeShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
      // Point light 1
      cubeShader.setVec3("pointLights[0].position", pointLightPositions[0]);
      cubeShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1,
                         pointLightColors[0].y * 0.1,
                         pointLightColors[0].z * 0.1);
      cubeShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x,
                         pointLightColors[0].y, pointLightColors[0].z);
      cubeShader.setVec3("pointLights[0].specular", pointLightColors[0].x,
                         pointLightColors[0].y, pointLightColors[0].z);
      cubeShader.setFloat("pointLights[0].constant", 1.0f);
      cubeShader.setFloat("pointLights[0].linear", 0.09);
      cubeShader.setFloat("pointLights[0].quadratic", 0.032);
      // Point light 2
      cubeShader.setVec3("pointLights[1].position", pointLightPositions[1]);
      cubeShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1,
                         pointLightColors[1].y * 0.1,
                         pointLightColors[1].z * 0.1);
      cubeShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x,
                         pointLightColors[1].y, pointLightColors[1].z);
      cubeShader.setVec3("pointLights[1].specular", pointLightColors[1].x,
                         pointLightColors[1].y, pointLightColors[1].z);
      cubeShader.setFloat("pointLights[1].constant", 1.0f);
      cubeShader.setFloat("pointLights[1].linear", 0.09);
      cubeShader.setFloat("pointLights[1].quadratic", 0.032);
      // Point light 3
      cubeShader.setVec3("pointLights[2].position", pointLightPositions[2]);
      cubeShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1,
                         pointLightColors[2].y * 0.1,
                         pointLightColors[2].z * 0.1);
      cubeShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x,
                         pointLightColors[2].y, pointLightColors[2].z);
      cubeShader.setVec3("pointLights[2].specular", pointLightColors[2].x,
                         pointLightColors[2].y, pointLightColors[2].z);
      cubeShader.setFloat("pointLights[2].constant", 1.0f);
      cubeShader.setFloat("pointLights[2].linear", 0.09);
      cubeShader.setFloat("pointLights[2].quadratic", 0.032);
      // Point light 4
      cubeShader.setVec3("pointLights[3].position", pointLightPositions[3]);
      cubeShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1,
                         pointLightColors[3].y * 0.1,
                         pointLightColors[3].z * 0.1);
      cubeShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x,
                         pointLightColors[3].y, pointLightColors[3].z);
      cubeShader.setVec3("pointLights[3].specular", pointLightColors[3].x,
                         pointLightColors[3].y, pointLightColors[3].z);
      cubeShader.setFloat("pointLights[3].constant", 1.0f);
      cubeShader.setFloat("pointLights[3].linear", 0.09);
      cubeShader.setFloat("pointLights[3].quadratic", 0.032);
      // SpotLight
      cubeShader.setVec3("spotLight.position", camera.Position);
      cubeShader.setVec3("spotLight.direction", camera.Front);
      cubeShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
      cubeShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.0f);
      cubeShader.setVec3("spotLight.specular", 0.8f, 0.8f, 0.0f);
      cubeShader.setFloat("spotLight.constant", 1.0f);
      cubeShader.setFloat("spotLight.linear", 0.09);
      cubeShader.setFloat("spotLight.quadratic", 0.032);
      cubeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
      cubeShader.setFloat("spotLight.outerCutOff",
                          glm::cos(glm::radians(13.0f)));
      break;
    case 1:
      // ==============================================================================================
      //       FACTORY
      // ==============================================================================================
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Directional light
      cubeShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
      cubeShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.1f);
      cubeShader.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.7);
      cubeShader.setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);
      // Point light 1
      cubeShader.setVec3("pointLights[0].position", pointLightPositions[0]);
      cubeShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1,
                         pointLightColors[0].y * 0.1,
                         pointLightColors[0].z * 0.1);
      cubeShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x,
                         pointLightColors[0].y, pointLightColors[0].z);
      cubeShader.setVec3("pointLights[0].specular", pointLightColors[0].x,
                         pointLightColors[0].y, pointLightColors[0].z);
      cubeShader.setFloat("pointLights[0].constant", 1.0f);
      cubeShader.setFloat("pointLights[0].linear", 0.09);
      cubeShader.setFloat("pointLights[0].quadratic", 0.032);
      // Point light 2
      cubeShader.setVec3("pointLights[1].position", pointLightPositions[1]);
      cubeShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1,
                         pointLightColors[1].y * 0.1,
                         pointLightColors[1].z * 0.1);
      cubeShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x,
                         pointLightColors[1].y, pointLightColors[1].z);
      cubeShader.setVec3("pointLights[1].specular", pointLightColors[1].x,
                         pointLightColors[1].y, pointLightColors[1].z);
      cubeShader.setFloat("pointLights[1].constant", 1.0f);
      cubeShader.setFloat("pointLights[1].linear", 0.09);
      cubeShader.setFloat("pointLights[1].quadratic", 0.032);
      // Point light 3
      cubeShader.setVec3("pointLights[2].position", pointLightPositions[2]);
      cubeShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1,
                         pointLightColors[2].y * 0.1,
                         pointLightColors[2].z * 0.1);
      cubeShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x,
                         pointLightColors[2].y, pointLightColors[2].z);
      cubeShader.setVec3("pointLights[2].specular", pointLightColors[2].x,
                         pointLightColors[2].y, pointLightColors[2].z);
      cubeShader.setFloat("pointLights[2].constant", 1.0f);
      cubeShader.setFloat("pointLights[2].linear", 0.09);
      cubeShader.setFloat("pointLights[2].quadratic", 0.032);
      // Point light 4
      cubeShader.setVec3("pointLights[3].position", pointLightPositions[3]);
      cubeShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1,
                         pointLightColors[3].y * 0.1,
                         pointLightColors[3].z * 0.1);
      cubeShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x,
                         pointLightColors[3].y, pointLightColors[3].z);
      cubeShader.setVec3("pointLights[3].specular", pointLightColors[3].x,
                         pointLightColors[3].y, pointLightColors[3].z);
      cubeShader.setFloat("pointLights[3].constant", 1.0f);
      cubeShader.setFloat("pointLights[3].linear", 0.09);
      cubeShader.setFloat("pointLights[3].quadratic", 0.032);
      // SpotLight
      cubeShader.setVec3("spotLight.position", camera.Position);
      cubeShader.setVec3("spotLight.direction", camera.Front);
      cubeShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
      cubeShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
      cubeShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
      cubeShader.setFloat("spotLight.constant", 1.0f);
      cubeShader.setFloat("spotLight.linear", 0.09);
      cubeShader.setFloat("spotLight.quadratic", 0.032);
      cubeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
      cubeShader.setFloat("spotLight.outerCutOff",
                          glm::cos(glm::radians(12.5f)));
      break;
    case 2:
      // ==============================================================================================
      //       HORROR
      // ==============================================================================================
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Directional light
      cubeShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
      cubeShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
      cubeShader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05);
      cubeShader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);
      // Point light 1
      cubeShader.setVec3("pointLights[0].position", pointLightPositions[0]);
      cubeShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1,
                         pointLightColors[0].y * 0.1,
                         pointLightColors[0].z * 0.1);
      cubeShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x,
                         pointLightColors[0].y, pointLightColors[0].z);
      cubeShader.setVec3("pointLights[0].specular", pointLightColors[0].x,
                         pointLightColors[0].y, pointLightColors[0].z);
      cubeShader.setFloat("pointLights[0].constant", 1.0f);
      cubeShader.setFloat("pointLights[0].linear", 0.09);
      cubeShader.setFloat("pointLights[0].quadratic", 0.032);
      // Point light 2
      cubeShader.setVec3("pointLights[1].position", pointLightPositions[1]);
      cubeShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1,
                         pointLightColors[1].y * 0.1,
                         pointLightColors[1].z * 0.1);
      cubeShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x,
                         pointLightColors[1].y, pointLightColors[1].z);
      cubeShader.setVec3("pointLights[1].specular", pointLightColors[1].x,
                         pointLightColors[1].y, pointLightColors[1].z);
      cubeShader.setFloat("pointLights[1].constant", 1.0f);
      cubeShader.setFloat("pointLights[1].linear", 0.09);
      cubeShader.setFloat("pointLights[1].quadratic", 0.032);
      // Point light 3
      cubeShader.setVec3("pointLights[2].position", pointLightPositions[2]);
      cubeShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1,
                         pointLightColors[2].y * 0.1,
                         pointLightColors[2].z * 0.1);
      cubeShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x,
                         pointLightColors[2].y, pointLightColors[2].z);
      cubeShader.setVec3("pointLights[2].specular", pointLightColors[2].x,
                         pointLightColors[2].y, pointLightColors[2].z);
      cubeShader.setFloat("pointLights[2].constant", 1.0f);
      cubeShader.setFloat("pointLights[2].linear", 0.09);
      cubeShader.setFloat("pointLights[2].quadratic", 0.032);
      // Point light 4
      cubeShader.setVec3("pointLights[3].position", pointLightPositions[3]);
      cubeShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1,
                         pointLightColors[3].y * 0.1,
                         pointLightColors[3].z * 0.1);
      cubeShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x,
                         pointLightColors[3].y, pointLightColors[3].z);
      cubeShader.setVec3("pointLights[3].specular", pointLightColors[3].x,
                         pointLightColors[3].y, pointLightColors[3].z);
      cubeShader.setFloat("pointLights[3].constant", 1.0f);
      cubeShader.setFloat("pointLights[3].linear", 0.09);
      cubeShader.setFloat("pointLights[3].quadratic", 0.032);
      // SpotLight
      cubeShader.setVec3("spotLight.position", camera.Position);
      cubeShader.setVec3("spotLight.direction", camera.Front);
      cubeShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
      cubeShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
      cubeShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
      cubeShader.setFloat("spotLight.constant", 1.0f);
      cubeShader.setFloat("spotLight.linear", 0.09);
      cubeShader.setFloat("spotLight.quadratic", 0.032);
      cubeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
      cubeShader.setFloat("spotLight.outerCutOff",
                          glm::cos(glm::radians(12.5f)));
      break;
    case 3:
      // ==============================================================================================
      //       BIOCHEMICAL LAB
      // ==============================================================================================
      glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Directional light
      cubeShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
      cubeShader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
      cubeShader.setVec3("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
      cubeShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
      // Point light 1
      cubeShader.setVec3("pointLights[0].position", pointLightPositions[0]);
      cubeShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1,
                         pointLightColors[0].y * 0.1,
                         pointLightColors[0].z * 0.1);
      cubeShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x,
                         pointLightColors[0].y, pointLightColors[0].z);
      cubeShader.setVec3("pointLights[0].specular", pointLightColors[0].x,
                         pointLightColors[0].y, pointLightColors[0].z);
      cubeShader.setFloat("pointLights[0].constant", 1.0f);
      cubeShader.setFloat("pointLights[0].linear", 0.09);
      cubeShader.setFloat("pointLights[0].quadratic", 0.032);
      // Point light 2
      cubeShader.setVec3("pointLights[1].position", pointLightPositions[1]);
      cubeShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1,
                         pointLightColors[1].y * 0.1,
                         pointLightColors[1].z * 0.1);
      cubeShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x,
                         pointLightColors[1].y, pointLightColors[1].z);
      cubeShader.setVec3("pointLights[1].specular", pointLightColors[1].x,
                         pointLightColors[1].y, pointLightColors[1].z);
      cubeShader.setFloat("pointLights[1].constant", 1.0f);
      cubeShader.setFloat("pointLights[1].linear", 0.09);
      cubeShader.setFloat("pointLights[1].quadratic", 0.032);
      // Point light 3
      cubeShader.setVec3("pointLights[2].position", pointLightPositions[2]);
      cubeShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1,
                         pointLightColors[2].y * 0.1,
                         pointLightColors[2].z * 0.1);
      cubeShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x,
                         pointLightColors[2].y, pointLightColors[2].z);
      cubeShader.setVec3("pointLights[2].specular", pointLightColors[2].x,
                         pointLightColors[2].y, pointLightColors[2].z);
      cubeShader.setFloat("pointLights[2].constant", 1.0f);
      cubeShader.setFloat("pointLights[2].linear", 0.09);
      cubeShader.setFloat("pointLights[2].quadratic", 0.032);
      // Point light 4
      cubeShader.setVec3("pointLights[3].position", pointLightPositions[3]);
      cubeShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1,
                         pointLightColors[3].y * 0.1,
                         pointLightColors[3].z * 0.1);
      cubeShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x,
                         pointLightColors[3].y, pointLightColors[3].z);
      cubeShader.setVec3("pointLights[3].specular", pointLightColors[3].x,
                         pointLightColors[3].y, pointLightColors[3].z);
      cubeShader.setFloat("pointLights[3].constant", 1.0f);
      cubeShader.setFloat("pointLights[3].linear", 0.09);
      cubeShader.setFloat("pointLights[3].quadratic", 0.032);
      // SpotLight
      cubeShader.setVec3("spotLight.position", camera.Position);
      cubeShader.setVec3("spotLight.direction", camera.Front);
      cubeShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
      cubeShader.setVec3("spotLight.diffuse", 0.0f, 1.0f, 0.0f);
      cubeShader.setVec3("spotLight.specular", 0.0f, 1.0f, 0.0f);
      cubeShader.setFloat("spotLight.constant", 1.0f);
      cubeShader.setFloat("spotLight.linear", 0.07);
      cubeShader.setFloat("spotLight.quadratic", 0.017);
      cubeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(7.0f)));
      cubeShader.setFloat("spotLight.outerCutOff",
                          glm::cos(glm::radians(10.0f)));
      break;
    default:
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // 设置所有光源属性
      // 定向光源
      cubeShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
      cubeShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
      cubeShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
      cubeShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
      // 点光源 1
      cubeShader.setVec3("pointLights[0].position", pointLightPositions[0]);
      cubeShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
      cubeShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
      cubeShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
      cubeShader.setFloat("pointLights[0].constant", 1.0f);
      cubeShader.setFloat("pointLights[0].linear", 0.09);
      cubeShader.setFloat("pointLights[0].quadratic", 0.032);
      // 点光源 2
      cubeShader.setVec3("pointLights[1].position", pointLightPositions[1]);
      cubeShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
      cubeShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
      cubeShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
      cubeShader.setFloat("pointLights[1].constant", 1.0f);
      cubeShader.setFloat("pointLights[1].linear", 0.09);
      cubeShader.setFloat("pointLights[1].quadratic", 0.032);
      // 点光源 3
      cubeShader.setVec3("pointLights[2].position", pointLightPositions[2]);
      cubeShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
      cubeShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
      cubeShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
      cubeShader.setFloat("pointLights[2].constant", 1.0f);
      cubeShader.setFloat("pointLights[2].linear", 0.09);
      cubeShader.setFloat("pointLights[2].quadratic", 0.032);
      // 点光源 4
      cubeShader.setVec3("pointLights[3].position", pointLightPositions[3]);
      cubeShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
      cubeShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
      cubeShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
      cubeShader.setFloat("pointLights[3].constant", 1.0f);
      cubeShader.setFloat("pointLights[3].linear", 0.09);
      cubeShader.setFloat("pointLights[3].quadratic", 0.032);
      // 聚光源
      cubeShader.setVec3("spotLight.position", camera.Position);
      cubeShader.setVec3("spotLight.direction", camera.Front);
      cubeShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
      cubeShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
      cubeShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
      cubeShader.setFloat("spotLight.constant", 1.0f);
      cubeShader.setFloat("spotLight.linear", 0.09);
      cubeShader.setFloat("spotLight.quadratic", 0.032);
      cubeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
      cubeShader.setFloat("spotLight.outerCutOff",
                          glm::cos(glm::radians(15.0f)));
      break;
  }
}