#ifndef GLFUN_H
#define  GLFUN_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include<opencv2/opencv.hpp>
#include <iostream>
#include "stb_image.h"
unsigned int createTexture();
unsigned int loadTexture(char const* path);
unsigned int CreatTexPlane(unsigned int VBO, unsigned int EBO);
void creatCubeMap();
unsigned int loadCubemap(std::vector<std::string> faces);
void createDepthMapFBO(unsigned int* depthMapFBO, unsigned int* depthMap);
void renderQuad();
#endif


