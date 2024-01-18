#ifndef AR_H
#define  AR_H
#include<opencv2/opencv.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include"GLFunc.h"
class AR
{
public:
    AR();
    ~AR();
    void InitCam();
    void draw();
    bool ARmode=false;
    bool IsCamInit = false;
private:
    void cvmatToTexture(GLuint& textureId, const cv::Mat& mat);
    void GetFrame();
    std::mutex g_mutex;
    std::queue<cv::Mat> frames;//�Ƚ��ȳ�����
    Shader shader;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    GLuint textureID;
    cv::VideoCapture cap;
};

AR::AR()
{
    //����plan
    VAO=CreatTexPlane(VBO, EBO);

    // �����������
    textureID = createTexture();

    //����shader
    shader.load("data/shader/qua.vert", "data/shader/qua.frag");
};
AR::~AR()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
void AR::InitCam()
{
    cap.open(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open camera." << std::endl;
        return;
    }
    else
    {
        IsCamInit = true;
    }
    int width = 1920;
    int height = 1080;
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    // ��������ͷ��֡��
    double frameRate = 60.0;  // 30֡ÿ��
    cap.set(cv::CAP_PROP_FPS, frameRate);
    
    //��������ͷ�߳�
    std::thread t1(&AR::GetFrame,this);
    t1.detach();
}
void AR::GetFrame()
{
    while (ARmode)
    {
        cv::Mat frame;
        if (!cap.read(frame))
        {
            std::cerr << "Error: Could not read frame from camera." << std::endl;
            break;  // Break the loop on error
        }
        if (frames.size() > 5)
        {
            g_mutex.lock();
            frames.pop();
            g_mutex.unlock();
        }
        g_mutex.lock();
        frames.push(frame);
        g_mutex.unlock();
    }

    std::cout << "���ڽ������ͷռ��" << std::endl;
    cap.release();
    //��ն���
    std::queue<cv::Mat> empty;
    std::swap(frames, empty);
    std::cout << "�ɹ��������ͷռ��" << std::endl;
    IsCamInit = false;
}
inline void AR::draw()
{
    cv::Mat frame;
    if (!frames.empty())//����������֡
    {
        g_mutex.lock();
        frame = frames.front();
        frames.pop();
        g_mutex.unlock();
    }
    if (!frame.empty())
    {
        cvmatToTexture(textureID, frame);
    }
    // ��������
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader.use();
    shader.setInt("screenTexture", 0);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
 void AR::cvmatToTexture(GLuint& textureId, const cv::Mat& mat)
{
    // ��textureID��ʶ������֮������в�����������ڸ������
    glBindTexture(GL_TEXTURE_2D, textureId);
    // ע��OpenCV��ͼ��ͨ����˳����BGR
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mat.cols, mat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, mat.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // ���
    glBindTexture(GL_TEXTURE_2D, 0);
}
#endif
