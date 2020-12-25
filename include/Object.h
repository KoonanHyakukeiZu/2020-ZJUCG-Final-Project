#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.h>
#include <model.h>

#include <string>

namespace GLObj
{
    class GLObject
	{
    private:
    glm::mat4 modelMat = glm::mat4(1.0f);
    Shader ObjShader;
    Model ObjModel;
    bool Objselected;
	public:
    GLObject(const char* vertexPath = nullptr, const char* fragmentPath = nullptr, const char* modelPath = nullptr):ObjShader(vertexPath, fragmentPath), ObjModel(modelPath);
    void ObjTranslatef(GLfloat x, GLfloat y, GLfloat z);
    void ObjRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
    void ObjScalef(GLfloat x, GLfloat y, GLfloat z);
    void ObjLoadIdentity();
    void ObjSetSelected(bool selected);


    }

    void GLObject::ObjTranslatef(GLfloat x, GLfloat y, GLfloat z)
    {
        modelMat = glm::translate(modelMat, glm::vec3(x, y, z));
        ObjShader.setMat4("model", modelMat); //uniform mat4 about model transformation should be named as "model"
    }

    void GLObject::ObjRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
    {
        modelMat = glm::rotate(modelMat, (float)angle, glm::vec3(x, y, z));
        ObjShader.setMat4("model", modelMat); //uniform mat4 about model transformation should be named as "model"
    }

    void GLObject::Scalef(GLfloat x, GLfloat y, GLfloat z)
    {
        modelMat = glm::scale(modelMat, glm::vec3(x, y, z));
        ObjShader.setMat4("model", modelMat); //uniform mat4 about model transformation should be named as "model"
    }

    void GLObject::ObjLoadIdentity()
    {
        modelMat = glm::mat4(1.0f);
        ObjShader.setMat4("model", modelMat); //uniform mat4 about model transformation should be named as "model"
    }

    void GLObject::ObjSetSelected(bool selected)
    {
        Objselected = selected;
    }
}

#endif
