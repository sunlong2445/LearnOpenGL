#ifndef SKYBOX_H
#define  SKYBOX_H
#include<vector>
#include<string>
#include"Camera.h"
#include"Shader.h"
#include"GLFunc.h"
#include"define.h"
class SkyBox
{
public:
	SkyBox(std::vector<std::string>& faces);
	void Draw(Camera* cam);
	~SkyBox();
private:
	// render data 
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int cubemapTexture;
	Shader shader;
};
#endif