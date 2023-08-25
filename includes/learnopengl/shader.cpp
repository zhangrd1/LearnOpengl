#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <string>
#include "TypeDef.h"
#include "shader.h"

MyShader::MyShader(const char* vertexPath, const char* fragmentPath, std::string name) : mName{ name }
{
    // һ����ȡ MyShader ����
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;

    // �� shader �ļ�
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);

    // ��ȡ�ļ����ݵ�����
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    // std::cout << vertexPath << std::endl;
    // std::cout << vShaderStream.str() << std::endl;
    // std::cout << fragmentPath << std::endl;
    // std::cout << fShaderStream.str() << std::endl;

    vShaderFile.close();
    fShaderFile.close();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    // ת��Ϊ C ����ַ���
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();


    // �������� MyShader
    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompiling(vertex, "Vertex");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompiling(fragment, "Pixel");


    // �������� MyShader Ϊ Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkLinking(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void MyShader::checkCompiling(unsigned int mShader, std::string nameShader)
{
    int success;
    char infoLog[512];
    glGetShaderiv(mShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(mShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << this->mName << nameShader << "::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void MyShader::checkLinking(unsigned int mProgram, std::string nameProgram)
{
    int success;
    char infoLog[512];
    glGetProgramiv(mProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(mProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << this->mName << nameProgram << "::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

void MyShader::use()
{
    glUseProgram(0);
    glUseProgram(ID);
}

void MyShader::setFloatUniform(std::string name, const float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void MyShader::setIDUniform(std::string name, const ResourceID value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void MyShader::setBoolUniform(std::string name, const bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

void MyShader::setMat4Uniform(std::string name, const float* value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);  // ��һ���������� Uniform ��λ��ֵ
    // �ڶ����������Ǿ�������
    // �������������Ƿ�Ծ���ת��
    // ���ĸ�������ָ������ָ�루GLM ����ҪתΪ���������飩

}

void MyShader::setVec3Uniform(std::string name, const float* value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value);
}