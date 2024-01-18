#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include"imgui/ImGuiFileDialog.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "MiniGL/shader.h"
#include "MiniGL/camera.h"
#include "MiniGL/model.h"
#include"MiniGL/GLFunc.h"
#include "MiniGL/Primitive.h"
#include "MiniGL/AR.h"
#include"MiniGL/skybox.h"
#include "MiniGL/Primitive.h"
#include"MiniGL/define.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
// settings
// camera
Camera* camera;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLFWwindow* init_flew_window()
{
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4); //抗锯齿， 4个样本（可以根据需要调整）
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);   //不可改变大小  
    //glfwWindowHint(GLFW_DECORATED, GL_FALSE);   //没有边框和标题栏 
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
   
    int monitorCount;
    GLFWmonitor** pMonitor = glfwGetMonitors(&monitorCount);
    const GLFWvidmode* mode = glfwGetVideoMode(pMonitor[0]);
    SCR_WIDTH = mode->width*0.9999;
    SCR_HEIGHT=mode->height*0.9999;
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MyEngine", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1); // Enable vsync
    //some setting 
   // stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);
    return window;
}
struct Gui_param
{
    Gui_param(bool & ref) : ARmode(ref) {}
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show_demo_window = false;         //imgui demo 窗口控制
    bool wire_mode = false;                //线框模式
    bool SSA0 = false;
    bool& ARmode;                   //开启AR模式
    bool hard_shadow = false;
    bool shadow_debug = false;
    bool back_ground = true;               
    float fps=0;                           //显示帧率
    float alpha = 1.0f;
    string filePath="";                    //当前加载路径
};
void init_imgui(GLFWwindow* window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    const char* glsl_version = "#version 130";
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}
void RenderMainImGui(Gui_param& gui_param)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (gui_param.show_demo_window)
        ImGui::ShowDemoWindow(&gui_param.show_demo_window);
        bool flag = true;
        if (gui_param.back_ground)
        {
            ImGui::Begin("Menu", &flag, ImGuiWindowFlags_NoTitleBar |ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |ImGuiWindowFlags_NoMove );
        }
        else
        {
            ImGui::Begin("Menu", &flag, ImGuiWindowFlags_NoTitleBar |ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
        }
        ImVec2 screen_size = ImGui::GetIO().DisplaySize;
        ImVec2 Menue_size = ImVec2(screen_size.x/5, screen_size.y/3);
        ImVec2 Menue_pos= ImVec2(screen_size.x - Menue_size.x, 0);
        ImGui::SetWindowSize("Menu", Menue_size);
        ImGui::SetWindowPos("Menu", Menue_pos);
        //ImGui::Text("Use 'Left Alter' to focus on window");
        ImGui::Checkbox("Demo Window", &gui_param.show_demo_window);
        ImGui::SameLine();
        ImGui::Checkbox("WireMode", &gui_param.wire_mode);
        ImGui::SameLine();
        ImGui::Checkbox("ARMode", &gui_param.ARmode);
        ImGui::SameLine();
        ImGui::Checkbox("background", &gui_param.back_ground);
        ImGui::SameLine();
        ImGui::Text("FPS: %f", gui_param.fps);
        //换行
        ImGui::ColorEdit4("clear color", (float*)&gui_param.clear_color); // Edit 3 floats representing a color
        ImGui::SliderFloat("alpha", &gui_param.alpha, 0.0, 1.0f);
        //换行
        ImGui::Checkbox("hard_shadow", &gui_param.hard_shadow);
        ImGui::SameLine();
        ImGui::Checkbox("shadow_debug", &gui_param.shadow_debug);
        ImGui::SameLine();

        //ImGui::DragFloat("alpha", &gui_param.alpha,0.01f,0.0,1.0f);
        
        if (ImGui::Button("Import model"))
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".obj", ".");
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                  gui_param.filePath = ImGuiFileDialog::Instance()->GetFilePathName();   
                  while (gui_param.filePath.find('\\') != gui_param.filePath.npos)
                  {
                      gui_param.filePath = gui_param.filePath.replace(gui_param.filePath.find('\\'), 1, 1, '/');
                  }
                 // gui_param.filePath.replace("","");
            }
            
            ImGuiFileDialog::Instance()->Close();
        }
        ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void RenderScene(vector<Shader>& shaders, vector<Model>& models, Gui_param& gui_param)
{
    if (models.size() != shaders.size())
    {
        std::cout << "模型数与shader数量不一致" << std::endl;
        return;
    }
    for (int i = 0; i < models.size(); i++)
    {
        shaders[i].use();
        glm::mat4 projection = camera->GetPerspectiveMatrix();
        glm::mat4 view = camera->GetViewMatrix();
        shaders[i].setMat4("projection", projection);
        shaders[i].setMat4("view", view);
        shaders[i].setFloat("alpha", gui_param.alpha);
        models[i].Draw(shaders[i]);
    }
}
void RenderPrimativeShadow(Shader* shader, Primitive* primitive, Primitive* light, unsigned int* depthMapFBO)
{
    float near_plane = 1.0f, far_plane = 100;                                                                                                                          .0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
  
    //glm::mat4 lightProjection =glm::perspective(glm::radians(45.0f), (16.0f / 9.0f), near_plane, far_plane);

  
    glm::mat4 lightView = glm::lookAt(light->Position,
                                      glm::vec3(0.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    shader->use();
    shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
   
    primitive->Draw(shader);
}
void RenderPrimitive(Shader* shader, Primitive* primitive, Primitive* light,Camera* cam)
{
    shader->use();
    shader->setMat4("projection", cam->GetPerspectiveMatrix());
    shader->setMat4("view", cam->GetViewMatrix());
    float near_plane = 1.0f, far_plane = 100.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
   // glm::mat4 lightProjection = glm::perspective(glm::radians(45.0f), (16.0f / 9.0f), near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(light->Position,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    shader->setVec3("viewPos",cam->Position);
    shader->setVec3("lightPos", light->Position);
    shader->setVec3("lightColor", light->Color);
    shader->setInt("shadowType", primitive->shadowType);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, primitive->texturesID);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, primitive->shadowMapID);
   

    primitive->Draw(shader);
}
void RenderLight(Shader* shader,  Primitive* light, Camera* cam)
{
    shader->use();
    shader->setMat4("projection", cam->GetPerspectiveMatrix());
    shader->setMat4("view", cam->GetViewMatrix());
    shader->setInt("shadowType", light->shadowType);
    light->Draw(shader);
}
void configure_opengl_state()
{
    // configure global opengl state
 // -----------------------------
    glEnable(GL_DEPTH_TEST);                     //启用深度测试
    glDepthFunc(GL_LESS);     
    // 启用垂直同步
    glfwSwapInterval(1); // 参数为1表示启用垂直同步
}
int main()
{
   
    bool Is_cam_ready = false;
    GLFWwindow* window=init_flew_window();
    init_imgui(window);
    AR ar;
    std::vector<std::string> faces
    {
     "data/pic/skybox/right.jpg",
     "data/pic/skybox/left.jpg",
     "data/pic/skybox/top.jpg",
      "data/pic/skybox/bottom.jpg",
      "data/pic/skybox/front.jpg",
     "data/pic/skybox/back.jpg"
    };
    SkyBox skybox(faces);
    Gui_param gui_param(ar.ARmode);
    Camera camera_ = Camera(glm::vec3(0.0f, 5.0f, 20.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT);
    camera = &camera_;
    vector<Model> models;
    vector<Shader> shaders;

    //创建深度缓冲帧
    unsigned int depthMapFBO;
    unsigned int depthMap;
    createDepthMapFBO(&depthMapFBO, &depthMap);


    Primitive floor(Type_Plane);
    floor.shadowType = 0;
    Shader floor_shader("data/shader/primitive.vert", "data/shader/primitive.frag");
    floor.Position = glm::vec3(0.0f, 0.0f, 0.0);
    floor.shadowMapID = depthMap;
    floor.texturesID = loadTexture("data/pic/wood.png");
    floor_shader.use();
    floor_shader.setInt("texture1", 0);
    floor_shader.setInt("shadowMap", 1);


    Primitive Cube(Type_Cube);
    Cube.shadowType = 0;
    Cube.Position = glm::vec3(0.0f, 2.0f, 0.0f);
    Cube.Scale = glm::vec3(1,4,1);
    Shader Cube_shader("data/shader/primitive.vert", "data/shader/primitive.frag");
    Cube.shadowMapID = depthMap;
    Cube_shader.use();
    Cube_shader.setInt("texture1", 0);
    Cube_shader.setInt("shadowMap", 1);
   


    Primitive lightCube(Type_Sphere);
    lightCube.Scale= glm::vec3(0.2f, 0.2f, 0.2f);
    lightCube.Color = glm::vec3(1,1,1);
    lightCube.Position = glm::vec3(5, 10, 0); //-2.0f, 4.0f, -1.0f  //10,20,0
    Shader light_shader("data/shader/light.vert", "data/shader/light.frag");
    Shader shodowmap_shader("data/shader/FBO_shadow.vert", "data/shader/FBO_shadow.frag");


    Shader debugDepthQuad("data/shader/FBO_debug.vert", "data/shader/FBO_debug.frag");
    debugDepthQuad.use();
    debugDepthQuad.setInt("depthMap", 0);



    configure_opengl_state();
    while (!glfwWindowShouldClose(window))
    {
        //显示图片
        if (gui_param.wire_mode)  
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (gui_param.filePath != "")
        {
           Model ourModel(gui_param.filePath);
           //ourModel.OnCenter(camera.Position,  camera.Front, camera.Zoom, camera.AspectRatio);
           Shader model_Shader("data/shader/model_loading.vert", "data/shader/model_loading.frag");
           shaders.push_back(model_Shader);
           models.push_back(ourModel);
           gui_param.filePath = "";
        }
        ImVec4 clear_color = gui_param.clear_color;
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        if (ar.ARmode)
        {
            if (!ar.IsCamInit)
            {
                ar.InitCam();
            }
            ar.draw();
        }
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST); 
        glEnable(GL_MULTISAMPLE);
        floor.shadowType = gui_param.hard_shadow;
        Cube.shadowType = gui_param.hard_shadow;
        if(gui_param.hard_shadow)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glViewport(0, 0, 1024, 1024);                  //视口位置大小的设置与当前绑定的FBO有关
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            RenderPrimativeShadow(&shodowmap_shader, &floor, &lightCube, &depthMapFBO);
            RenderPrimativeShadow(&shodowmap_shader, &Cube, &lightCube, &depthMapFBO);
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            //恢复渲染设置
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, SCR_WIDTH ,SCR_HEIGHT);
            
            if (gui_param.shadow_debug)
            {
                debugDepthQuad.use();

                debugDepthQuad.setFloat("near_plane",1);
                debugDepthQuad.setFloat("far_plane", 1000);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, depthMap);
                renderQuad();
            }
            else
            {

              RenderPrimitive(&floor_shader, &floor, &lightCube, camera);
              RenderPrimitive(&Cube_shader, &Cube, &lightCube, camera);
              RenderLight(&light_shader, &lightCube, camera);
            }
        }
        else
        {
            RenderPrimitive(&floor_shader, &floor, &lightCube, camera);
            RenderPrimitive(&Cube_shader, &Cube, &lightCube, camera);
            RenderLight(&light_shader, &lightCube, camera);

        }
        skybox.Draw(camera);
    
       /* glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        RenderScene(shaders, models,gui_param);*/
        

        // UI
        RenderMainImGui(gui_param);
        processInput(window);
        glfwPollEvents();        
        glfwSwapBuffers(window); //刷新缓冲
    }
    glfwTerminate();

    return 0;
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, deltaTime);

    ImVec2 screen_size = ImGui::GetIO().DisplaySize;
    ImVec2 Menue_size = ImVec2(screen_size.x / 5, screen_size.y / 3);
    ImVec2 Menue_pos = ImVec2(screen_size.x - Menue_size.x, 0);
    ImGuiIO& io = ImGui::GetIO();
    if (io.MouseDown[0])
    {
        bool on_menue = io.MousePos.x > Menue_pos.x && io.MousePos.x <(Menue_pos.x + Menue_size.x) &&
            io.MousePos.y> Menue_pos.y && io.MousePos.y < (Menue_pos.y + Menue_size.y);
        if (!on_menue)
        {
            camera->ProcessMouseMovement(io.MouseDelta.x, io.MouseDelta.y);
        }
    }

}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->ProcessMouseScroll(static_cast<float>(yoffset));
}


