#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "TypeDef.h"

#include <string>
#include <vector>

struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 texcoords;
	};

	struct Texture {
		ResourceID  id;
		textureType type;
	};

class Mesh
{
public:
	std::vector<Vertex>     mvertices;
	std::vector<ResourceID> mindices;
	std::vector<Texture>    mtextures;

	Mesh(std::vector<Vertex>, std::vector<ResourceID>, std::vector<Texture>);
	void draw(MyShader& shader);

private:
	ResourceID mVAO {};
	ResourceID mVBO {};
	ResourceID mEBO {};

	void setupMesh();
};

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<ResourceID> indices, std::vector<Texture> textures) :
	mvertices{ vertices },
	mindices { indices },
	mtextures{ textures } 
{
	setupMesh();
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mvertices.size() * sizeof(Vertex), &mvertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &mEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mindices.size() * sizeof(ResourceID), &mindices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);  // 坐标属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);  // 法线属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(2);  // 坐标属性
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texcoords)));

	glBindVertexArray(0);
}

void Mesh::draw(MyShader& shader)
{
	ResourceID diffuseNum  { 0 };
	ResourceID specularNum { 0 };

	for (ResourceID i{ 0 }; i < mtextures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);  // 激活纹理单元

		std::string propertyName;
		std::string propertyNum;

		auto texType { mtextures[i].type }; 

		switch (texType) 
		{
		case textureType::diffuse:
			propertyName = "texture_diffuse";
			propertyNum  = std::to_string(diffuseNum);
			diffuseNum++;
			break;
		case textureType::specular:
			propertyName = "texture_specular";
			propertyNum  = std::to_string(specularNum);
			specularNum++;
			break;
		default:
			std::cout << "texture type wrong" << std::endl;
			break;
		}

		std::string nameInShader {"material." + propertyName + "_" + propertyNum};
		shader.setIDUniform(nameInShader, i);

		glBindTexture(GL_TEXTURE_2D, mtextures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// 绘制
	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, mindices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}














//#ifndef MESH_H
//#define MESH_H
//
//#include <glad/glad.h> // holds all OpenGL type declarations
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#include <learnopengl/shader.h>
//
//#include <string>
//#include <vector>
//using namespace std;
//
//#define MAX_BONE_INFLUENCE 4
//
//struct Vertex {
//    // position
//    glm::vec3 Position;
//    // normal
//    glm::vec3 Normal;
//    // texCoords
//    glm::vec2 TexCoords;
//    // tangent
//    glm::vec3 Tangent;
//    // bitangent
//    glm::vec3 Bitangent;
//	//bone indexes which will influence this vertex
//	int m_BoneIDs[MAX_BONE_INFLUENCE];
//	//weights from each bone
//	float m_Weights[MAX_BONE_INFLUENCE];
//};
//
//struct Texture {
//    unsigned int id;
//    string type;
//    string path;
//};
//
//class Mesh {
//public:
//    // mesh Data
//    vector<Vertex>       vertices;
//    vector<unsigned int> indices;
//    vector<Texture>      textures;
//    unsigned int VAO;
//
//    // constructor
//    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
//    {
//        this->vertices = vertices;
//        this->indices = indices;
//        this->textures = textures;
//
//        // now that we have all the required data, set the vertex buffers and its attribute pointers.
//        setupMesh();
//    }
//
//    // render the mesh
//    void Draw(Shader &shader) 
//    {
//        // bind appropriate textures
//        unsigned int diffuseNr  = 1;
//        unsigned int specularNr = 1;
//        unsigned int normalNr   = 1;
//        unsigned int heightNr   = 1;
//        for(unsigned int i = 0; i < textures.size(); i++)
//        {
//            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
//            // retrieve texture number (the N in diffuse_textureN)
//            string number;
//            string name = textures[i].type;
//            if(name == "texture_diffuse")
//                number = std::to_string(diffuseNr++);
//            else if(name == "texture_specular")
//                number = std::to_string(specularNr++); // transfer unsigned int to string
//            else if(name == "texture_normal")
//                number = std::to_string(normalNr++); // transfer unsigned int to string
//             else if(name == "texture_height")
//                number = std::to_string(heightNr++); // transfer unsigned int to string
//
//            // now set the sampler to the correct texture unit
//            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
//            // and finally bind the texture
//            glBindTexture(GL_TEXTURE_2D, textures[i].id);
//        }
//        
//        // draw mesh
//        glBindVertexArray(VAO);
//        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
//        glBindVertexArray(0);
//
//        // always good practice to set everything back to defaults once configured.
//        glActiveTexture(GL_TEXTURE0);
//    }
//
//private:
//    // render data 
//    unsigned int VBO, EBO;
//
//    // initializes all the buffer objects/arrays
//    void setupMesh()
//    {
//        // create buffers/arrays
//        glGenVertexArrays(1, &VAO);
//        glGenBuffers(1, &VBO);
//        glGenBuffers(1, &EBO);
//
//        glBindVertexArray(VAO);
//        // load data into vertex buffers
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        // A great thing about structs is that their memory layout is sequential for all its items.
//        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
//        // again translates to 3/2 floats which translates to a byte array.
//        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  
//
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
//
//        // set the vertex attribute pointers
//        // vertex Positions
//        glEnableVertexAttribArray(0);	
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
//        // vertex normals
//        glEnableVertexAttribArray(1);	
//        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
//        // vertex texture coords
//        glEnableVertexAttribArray(2);	
//        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
//        // vertex tangent
//        glEnableVertexAttribArray(3);
//        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
//        // vertex bitangent
//        glEnableVertexAttribArray(4);
//        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
//		// ids
//		glEnableVertexAttribArray(5);
//		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
//
//		// weights
//		glEnableVertexAttribArray(6);
//		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
//        glBindVertexArray(0);
//    }
//};
//#endif
