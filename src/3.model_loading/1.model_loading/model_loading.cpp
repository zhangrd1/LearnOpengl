#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "learnopengl/shader.h"
//#include <learnopengl/camera.h>
// #include <learnopengl/model.h>
#include "learnopengl/TypeDef.h"

#include <iostream>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


struct Material
{
    ResourceID diffuse;
    ResourceID specular;
    float      shininess;
};

struct DirLight
{
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight
{
    glm::vec3 lightcolor;

    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float linear;
    float quadratic;
};

struct SpotLight
{
    glm::vec3 lightcolor;

    glm::vec3 direction;
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float cutoff;
    float outerCutoff;
};

void framebuffer_size_callback(GLFWwindow*, int, int);
void processKeyInput(GLFWwindow*);
void processMouseInput(GLFWwindow* window, double xpos, double ypos);
void processMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
void setSpotLightShader(MyShader&, SpotLight&, const std::string&);
void setPointLightShader(MyShader&, PointLight&, const std::string&);
void setDirLightShader(MyShader&, DirLight&, const std::string&);
ResourceID loadTexture(std::string&&);
void setMaterialShader(MyShader&, Material&, const std::string&);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime{ .0f };  // 当前帧与上一帧的时间差
float lastFrame{ .0f };  // 上一帧的时间
float yaw{ .0f };        // 偏航角
float pitch{ .0f };      // 俯仰角

glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 cameramodel = glm::mat4(1.0f);


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 配置管线固定选项
    glEnable(GL_DEPTH_TEST);  // 开启深度测试
    // glDepthFunc(GL_ALWAYS);  // 永远通过深度测试
    // glDepthFunc(GL_GREATER);  // 在片段深度值大于缓冲区的深度值时通过测试



    // 准备数据
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    // 创建 Object VAO
    ResourceID objectVAO, VBO;

    glGenVertexArrays(1, &objectVAO);
    glBindVertexArray(objectVAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 创建 Lighter VAO
    ResourceID lighterVAO;

    glGenVertexArrays(1, &lighterVAO);
    glBindVertexArray(lighterVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // 创建纹理
    ResourceID diffuseMap  = loadTexture("E:/OpenGL/Course/resources/textures/container2.png");
    ResourceID specularMap = loadTexture("E:/OpenGL/Course/resources/textures/container2_specular.png");


    // 创建着色器
    MyShader lightingShader(
        "1.model_loading.vs",
        "1.model_loading.fs",
        "lightingShader"
    );

    MyShader lighterShader(
        "E:/OpenGL/Course/src/2.lighting/6.multiple_lights/6.light_cube.vs",
        "E:/OpenGL/Course/src/2.lighting/6.multiple_lights/6.light_cube.fs",
        "lighterShader"
    );

    // 盒子的 MVP 设置
    glm::mat4 objectModel = glm::mat4(1.0f);
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f);

    // 光源的 MVP 设置
    glm::vec3 lighterPos(1.0f, 1.0f, 1.0f);
    glm::mat4 lightermodel = glm::mat4(1.0f);
    lightermodel = glm::translate(lightermodel, lighterPos);
    lightermodel = glm::scale(lightermodel, glm::vec3(0.3f));

    std::cout << "zhangrd111's program" << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        // 计算时间差
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 处理输入
        processKeyInput(window);
        glfwSetCursorPosCallback(window, processMouseInput);
        glfwSetScrollCallback(window, processMouseScroll);

        // 清理上一帧屏幕
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 更新相机
        cameramodel = glm::translate(cameramodel, cameraPos);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // 更新运动
        //lightermodel = glm::translate(lightermodel, lighterPos);
        //lightermodel = glm::translate(lightermodel, glm::vec3(sin(time) * 2, cos(time) * 2, sin(time) * 3));
        //lightermodel = glm::scale(lightermodel, glm::vec3(0.2f));
        glm::vec3 lighterWorldPos = lighterPos;

        // 配置 Object 的 Shader
        lightingShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // 配置 MVP
        lightingShader.setMat4Uniform("view", glm::value_ptr(view));
        lightingShader.setMat4Uniform("projection", glm::value_ptr(projection));
        lightingShader.setVec3Uniform("cameraPos", glm::value_ptr(cameraPos));

        // 配置材质
        Material material;
        material.diffuse = 0;
        material.specular = 1;
        material.shininess = 32.0f;

        setMaterialShader(lightingShader, material, "material");

        // 配置光源
        SpotLight spotlight;
        spotlight.lightcolor = glm::vec3(1.0f, 1.0f, 0.0f);
        spotlight.ambient = glm::vec3(0.01f) * spotlight.lightcolor;
        spotlight.diffuse = glm::vec3(0.3f) * spotlight.lightcolor;
        spotlight.specular = glm::vec3(1.0f) * spotlight.lightcolor;
        spotlight.position = cameraPos;
        spotlight.direction = cameraFront;
        spotlight.cutoff = 12.5f;
        spotlight.outerCutoff = 15.0f;

        PointLight pointlight;
        pointlight.lightcolor = glm::vec3(.4f, 0.0f, 0.0f);
        pointlight.ambient = glm::vec3(0.01f) * pointlight.lightcolor;
        pointlight.diffuse = glm::vec3(0.6f) * pointlight.lightcolor;
        pointlight.specular = glm::vec3(1.0f) * pointlight.lightcolor;
        pointlight.position = glm::vec3(lighterWorldPos.x, lighterWorldPos.y, lighterWorldPos.z);
        pointlight.linear = 0.1f;
        pointlight.quadratic = 0.1f;

        setSpotLightShader(lightingShader, spotlight, "spotlight");
        setPointLightShader(lightingShader, pointlight, "pointlight");

        glBindVertexArray(objectVAO);

        for (size_t i{ 0 }; i < 10; i++) {
            glm::mat4 tempmodel = glm::translate(objectModel, cubePositions[i]);
            // float angle = 45.0f + 20.0f * i;
            // model = glm::rotate(model, time * glm::radians(angle), glm::vec3(1.0f, i * 0.3f, 0.0f));
            lightingShader.setMat4Uniform("model", glm::value_ptr(tempmodel));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // 配置 点光源 的 Shader
        lighterShader.use();

        // 配置 MVP
        lighterShader.setMat4Uniform("model", glm::value_ptr(lightermodel));
        lighterShader.setMat4Uniform("view", glm::value_ptr(view));
        lighterShader.setMat4Uniform("projection", glm::value_ptr(projection));

        // 配置 颜色
        setPointLightShader(lighterShader, pointlight, "pointlight");

        glBindVertexArray(lighterVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}


void processKeyInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cameraSpeed *= 3;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraSpeed * cameraFront;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * cameraFront;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        cameraPos += glm::vec3(0.0f, cameraSpeed, 0.0f);
        //auto rightaxis = glm::normalize(glm::cross(cameraFront, cameraUp));
        //cameraPos -= glm::normalize(glm::cross(cameraFront, rightaxis)) * cameraSpeed;
        //std::cout << cameraPos.x << "," << cameraPos.y << "," << cameraPos.z << std::endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        cameraPos -= glm::vec3(0.0f, cameraSpeed, 0.0f);
        // std::cout << cameraPos.x << "," << cameraPos.y << "," << cameraPos.z << std::endl;
    }
}

void processMouseInput(GLFWwindow* window, double xpos, double ypos)
{
    static bool  firstMouse{ true };
    static float lastX{};
    static float lastY{};

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    // std::cout << lastY << "," << ypos << std::endl;
    lastX = xpos;
    lastY = ypos;

    static float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    else if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    static float fov{ 30.f };

    if (fov >= 1.0f && fov <= 60.0f)
        fov -= yoffset;

    if (fov <= 1.0f) {
        fov = 1.0f;
    }
    if (fov >= 60.0f) {
        fov = 60.0f;
    }

    projection = glm::perspective(glm::radians(fov), static_cast<float>(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void setMaterialShader(MyShader& shader, Material& material, const std::string& nameInShader)
{
    shader.setIDUniform(nameInShader + ".diffuse", material.diffuse);
    shader.setIDUniform(nameInShader + ".specular", material.specular);
    shader.setFloatUniform(nameInShader + ".shininess", material.shininess);
}

void setSpotLightShader(MyShader& shader, SpotLight& light, const std::string& nameInShader)
{
    shader.setVec3Uniform(nameInShader + ".position", glm::value_ptr(light.position));
    shader.setVec3Uniform(nameInShader + ".direction", glm::value_ptr(light.direction));
    shader.setFloatUniform(nameInShader + ".cutoff", glm::cos(glm::radians(light.cutoff)));
    shader.setFloatUniform(nameInShader + ".outerCutoff", glm::cos(glm::radians(light.outerCutoff)));

    shader.setVec3Uniform(nameInShader + ".ambient", glm::value_ptr(light.ambient));
    shader.setVec3Uniform(nameInShader + ".diffuse", glm::value_ptr(light.diffuse));
    shader.setVec3Uniform(nameInShader + ".specular", glm::value_ptr(light.specular));
}

void setPointLightShader(MyShader& shader, PointLight& light, const std::string& nameInShader)
{
    shader.setVec3Uniform(nameInShader + ".position", glm::value_ptr(light.position));

    shader.setVec3Uniform(nameInShader + ".ambient", glm::value_ptr(light.ambient));
    shader.setVec3Uniform(nameInShader + ".diffuse", glm::value_ptr(light.diffuse));
    shader.setVec3Uniform(nameInShader + ".specular", glm::value_ptr(light.specular));

    shader.setFloatUniform(nameInShader + ".linear", light.linear);
    shader.setFloatUniform(nameInShader + ".quadratic", light.quadratic);
}

void setDirLightShader(MyShader& shader, DirLight& light, const std::string& nameInShader)
{
    shader.setVec3Uniform(nameInShader + ".direction", glm::value_ptr(light.direction));

    shader.setVec3Uniform(nameInShader + ".ambient", glm::value_ptr(light.ambient));
    shader.setVec3Uniform(nameInShader + ".diffuse", glm::value_ptr(light.diffuse));
    shader.setVec3Uniform(nameInShader + ".specular", glm::value_ptr(light.specular));

}

ResourceID loadTexture(std::string&& path)
{
    ResourceID textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    stbi_set_flip_vertically_on_load(true);

    if (data)
    {
        GLenum format;
        if (nrChannels == 1) {
            format = GL_RED;
        }
        else if (nrChannels == 3) {
            format = GL_RGB;
        }
        else if (nrChannels == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}






























//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void processInput(GLFWwindow *window);
//
//// settings
//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 600;
//
//// camera
//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//float lastX = SCR_WIDTH / 2.0f;
//float lastY = SCR_HEIGHT / 2.0f;
//bool firstMouse = true;
//
//// timing
//float deltaTime = 0.0f;
//float lastFrame = 0.0f;
//
//int main()
//{
//    // glfw: initialize and configure
//    // ------------------------------
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//#ifdef __APPLE__
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//
//    // glfw window creation
//    // --------------------
//    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//    if (window == NULL)
//    {
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//    glfwSetCursorPosCallback(window, mouse_callback);
//    glfwSetScrollCallback(window, scroll_callback);
//
//    // tell GLFW to capture our mouse
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//    // glad: load all OpenGL function pointers
//    // ---------------------------------------
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }
//
//    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
//    stbi_set_flip_vertically_on_load(true);
//
//    // configure global opengl state
//    // -----------------------------
//    glEnable(GL_DEPTH_TEST);
//
//    // build and compile shaders
//    // -------------------------
//    Shader ourShader("1.model_loading.vs", "1.model_loading.fs");
//
//    // load models
//    // -----------
//    Model ourModel(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
//
//    
//    // draw in wireframe
//    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//    // render loop
//    // -----------
//    while (!glfwWindowShouldClose(window))
//    {
//        // per-frame time logic
//        // --------------------
//        float currentFrame = static_cast<float>(glfwGetTime());
//        deltaTime = currentFrame - lastFrame;
//        lastFrame = currentFrame;
//
//        // input
//        // -----
//        processInput(window);
//
//        // render
//        // ------
//        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        // don't forget to enable shader before setting uniforms
//        ourShader.use();
//
//        // view/projection transformations
//        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//        glm::mat4 view = camera.GetViewMatrix();
//        ourShader.setMat4("projection", projection);
//        ourShader.setMat4("view", view);
//
//        // render the loaded model
//        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
//        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
//        ourShader.setMat4("model", model);
//        ourModel.Draw(ourShader);
//
//
//        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//        // -------------------------------------------------------------------------------
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    // glfw: terminate, clearing all previously allocated GLFW resources.
//    // ------------------------------------------------------------------
//    glfwTerminate();
//    return 0;
//}
//
//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//// ---------------------------------------------------------------------------------------------------------
//void processInput(GLFWwindow *window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        camera.ProcessKeyboard(FORWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        camera.ProcessKeyboard(BACKWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        camera.ProcessKeyboard(LEFT, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        camera.ProcessKeyboard(RIGHT, deltaTime);
//}
//
//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//// ---------------------------------------------------------------------------------------------
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    // make sure the viewport matches the new window dimensions; note that width and 
//    // height will be significantly larger than specified on retina displays.
//    glViewport(0, 0, width, height);
//}
//
//// glfw: whenever the mouse moves, this callback is called
//// -------------------------------------------------------
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//{
//    float xpos = static_cast<float>(xposIn);
//    float ypos = static_cast<float>(yposIn);
//
//    if (firstMouse)
//    {
//        lastX = xpos;
//        lastY = ypos;
//        firstMouse = false;
//    }
//
//    float xoffset = xpos - lastX;
//    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//    lastX = xpos;
//    lastY = ypos;
//
//    camera.ProcessMouseMovement(xoffset, yoffset);
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//// ----------------------------------------------------------------------
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    camera.ProcessMouseScroll(static_cast<float>(yoffset));
//}
