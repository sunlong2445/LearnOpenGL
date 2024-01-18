#ifndef PRIMITIVE_H
#define  PRIMITIVE_H
#include <glm/glm.hpp>
#include "shader.h"
#include<vector>
#include <glad/glad.h> 
#include <vector>
#include <glm/gtc/matrix_transform.hpp>



struct PrimitiveVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

enum PrimitiveType {
    Type_Plane,    // 默认值为0
    Type_Sphere,  // 默认值为1
    Type_Cube    // 默认值为2
};

class Primitive
{
public :
    Primitive() {};
    ~Primitive();
    Primitive(PrimitiveType type);
    void Draw(Shader* shader);
    glm::mat4 GetModelMatrix();
    std::vector< PrimitiveVertex> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 Position;
	glm::vec3 Scale;
    glm::vec3 Color;
	glm::qua<float> Rotation;
    unsigned int   texturesID;
    unsigned int   shadowMapID;
    unsigned int   shadowType;

    
private:
	// render data 
	unsigned int VBO, EBO, VAO;
    void setupMesh();
    void CreatPlane();
    void CreatSphere();
    void CreatCube();
};

Primitive::Primitive(PrimitiveType type) {
    switch (type) {
    case Type_Plane:
        CreatPlane();
        break; // 结束当前 case 块

    case Type_Sphere:
        CreatSphere();
        break;

    case Type_Cube:
        CreatCube();
        break;

    default:
        std::cout << "Invalid choice." << std::endl;
        break;
    }
    setupMesh();
};

Primitive::~Primitive()
{
    /*delete[] Vertices;
    delete[] Indices;*/
}
void Primitive::Draw(Shader* shader)
{
    shader->setMat4("model", GetModelMatrix());
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
};
void Primitive::setupMesh()
{
    Position = glm::vec3(0, 0, 0);
    Rotation = glm::qua<float>(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)));
    Scale = glm::vec3(1, 1, 1);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // 绑定顶点缓冲对象（VBO）
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PrimitiveVertex), vertices.data(), GL_STATIC_DRAW);

    // 绑定索引缓冲对象（EBO）
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PrimitiveVertex), (void*)offsetof(PrimitiveVertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PrimitiveVertex), (void*)offsetof(PrimitiveVertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(PrimitiveVertex), (void*)offsetof(PrimitiveVertex, texCoords));
    glEnableVertexAttribArray(2);

    // 解绑 VAO，以防止其他 VAO 修改它
    glBindVertexArray(0);

};
glm::mat4 Primitive:: GetModelMatrix()
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), Position);
    model = glm::mat4_cast(Rotation) * model;
    model = glm::scale(model, Scale);
    return model;
}
void Primitive::CreatPlane()
{
    float positions[] = {
        // positions            // normals         // texcoords
         10.0f, 0.0f,  10.0f,    
        -10.0f, 0.0f,  10.0f,    
        -10.0f, 0.0f, -10.0f,     

         10.0f, 0.0f,  10.0f,    
        -10.0f, 0.0f, -10.0f,    
         10.0f, 0.0f, -10.0f,   
    };
  

    float normals[] = {

        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f
    };


    float texCoords[] = {

        10.0f,  0.0f,
        0.0f,  0.0f,
        0.0f, 10.0f,

        10.0f,  0.0f,
        0.0f, 10.0f,
        10.0f, 10.0f
    };

    for (int i = 0; i < 6; ++i) {
        PrimitiveVertex vertex;
        vertex.position = glm::vec3(positions[3 * i], positions[3 * i + 1], positions[3 * i + 2]);
        vertex.normal = glm::vec3(normals[3 * i], normals[3 * i + 1], normals[3 * i + 2]);
        vertex.texCoords = glm::vec2(texCoords[2 * i], texCoords[2 * i + 1]);
        vertices.push_back(vertex);
    }
    // 定义平面的面片索引
    int index[] = {
        0, 1, 2,
        3, 4, 5,
    };
    for (int i = 0; i < 2 * 3; i++)
    {
        indices.push_back(index[i]);
    }
}
 void Primitive::CreatSphere()
{
     // 构建顶点数据
     int sectorCount = 20;
     int stackCount = 20;
     const float PI = glm::pi<float>();
     const float sectorStep = 2 * PI / static_cast<float>(sectorCount);
     const float stackStep = PI / static_cast<float>(stackCount);

     for (int i = 0; i <= stackCount; ++i) {
         float stackAngle = PI / 2 - i * stackStep;
         float xy = std::cos(stackAngle);
         float z = std::sin(stackAngle);

         for (int j = 0; j <= sectorCount; ++j) {
             float sectorAngle = j * sectorStep;

             PrimitiveVertex vertex;
             // Position
             vertex.position.x = xy * std::cos(sectorAngle);
             vertex.position.y = xy * std::sin(sectorAngle);
             vertex.position.z = z;

             // Normal
             vertex.normal = glm::normalize(vertex.position);

             // Texture Coordinates
             vertex.texCoords.x = static_cast<float>(j) / sectorCount;
             vertex.texCoords.y = static_cast<float>(i) / stackCount;

             vertices.push_back(vertex);
         }
     }

     //构建索引数据
     for (int i = 0; i < stackCount; ++i) {
         for (int j = 0; j < sectorCount; ++j) {
             int k1 = i * (sectorCount + 1) + j;
             int k2 = k1 + 1;
             int k3 = (i + 1) * (sectorCount + 1) + j;
             int k4 = k3 + 1;

             indices.push_back(k1);
             indices.push_back(k3);
             indices.push_back(k2);

             indices.push_back(k2);
             indices.push_back(k3);
             indices.push_back(k4);
         }
     }
}
 void Primitive::CreatCube()
{  
     // 构建顶点数据
     float positions[] = {
    -0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f,  0.5f, -0.5f,  
    -0.5f,  0.5f, -0.5f,  

    -0.5f, -0.5f,  0.5f,  
     0.5f, -0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f, 

    -0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f, -0.5f, 
    -0.5f, -0.5f, -0.5f, 
    -0.5f, -0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f,  

    -0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f, 
    -0.5f, -0.5f,  0.5f,  

    -0.5f,  0.5f, -0.5f, 
     0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f, 
     };

     float normals[] = {
        0.0f,  0.0f, -1.0f,  // Back
       0.0f,  0.0f, -1.0f,
       0.0f,  0.0f, -1.0f,
      0.0f,  0.0f, -1.0f,

      0.0f,  0.0f,  1.0f,
      0.0f,  0.0f,  1.0f,
       0.0f,  0.0f,  1.0f,
       0.0f,  0.0f,  1.0f,

       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,

       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,

       0.0f, -1.0f,  0.0f,
       0.0f, -1.0f,  0.0f,
       0.0f, -1.0f,  0.0f,
       0.0f, -1.0f,  0.0f,

        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f
     };

     float texCoords[] = {
         0.0f, 0.0f,  // 0
         1.0f, 0.0f,  // 1
         1.0f, 1.0f,  // 2
         0.0f, 1.0f,  // 3

         1.0f, 0.0f,  // 4
         0.0f, 0.0f,  // 5
         0.0f, 1.0f,  // 6
         1.0f, 1.0f,   // 7

         0.0f, 0.0f,  // 0
         1.0f, 0.0f,  // 1
         1.0f, 1.0f,  // 2
         0.0f, 1.0f,  // 3

         1.0f, 0.0f,  // 4
         0.0f, 0.0f,  // 5
         0.0f, 1.0f,  // 6
         1.0f, 1.0f,   // 7

         0.0f, 0.0f,  // 0
         1.0f, 0.0f,  // 1
         1.0f, 1.0f,  // 2
         0.0f, 1.0f,  // 3

         1.0f, 0.0f,  // 4
         0.0f, 0.0f,  // 5
         0.0f, 1.0f,  // 6
         1.0f, 1.0f   // 7
     };

     for (int i = 0; i < 24; ++i) {
         PrimitiveVertex vertex;
         vertex.position = glm::vec3(positions[3 * i], positions[3 * i + 1], positions[3 * i + 2]);
         vertex.normal = glm::vec3(normals[3 * i], normals[3 * i + 1], normals[3 * i + 2]);
         vertex.texCoords = glm::vec2(texCoords[2 * i], texCoords[2 * i + 1]);
         vertices.push_back(vertex);
     }

     // 构建面片数据
     int index[]=
     {
     0, 1, 2,
    2, 3, 0,

    4, 5, 6,
    6, 7, 4,

    8, 9, 10,
    10, 11, 8,

    12, 13, 14,
    14, 15, 12,

    16, 17, 18,
    18, 19, 16,

    20, 21, 22,
    22, 23, 20
     };
     for (int i = 0; i < 12 * 3; i++)
     {
         indices.push_back(index[i]);
     }
}
#endif
