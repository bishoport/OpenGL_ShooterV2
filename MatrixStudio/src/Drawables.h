#pragma once

#include <libCore.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <cmath>



class DrawableBase {
protected:
    GLuint VAO = 0, VBO = 0;
    float speedAnimation = 1.0f;
    glm::vec3 color = { 1.0f, 1.0f, 1.0f };
    bool initialized = false;

public:
    DrawableBase(const glm::vec3& col) : color(col) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }

    virtual ~DrawableBase() {
        if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
    }

    virtual void Draw(float deltaTime) = 0;
    virtual void initializeGraphics() = 0;
};






class DrawableVector3 : public DrawableBase {
public:
    glm::vec3 vector;
    glm::vec3 origin;
    glm::vec3 vectorTarget;
    glm::vec3 originTarget;
    glm::vec3 axisDirectionFactor = { 1.0f, 1.0f, 1.0f };

    std::vector<float> sphereVertices;
    std::vector<float> lineVertices;
    std::vector<float> arrowVertices;

    bool drawSphere = true;
    bool drawArrow = true;


    DrawableVector3(const glm::vec3& vec, const glm::vec3& col) : DrawableBase(col), vector(vec), origin(glm::vec3(0.0f, 0.0f, 0.0f)) {
        vectorTarget = vec;
        originTarget = origin;

        sphereVertices = DrawSphere(0.05f, 8, 8, origin);
        lineVertices = { origin.x, origin.y, origin.z, vec.x, vec.y, vec.z };
        arrowVertices = DrawArrow3D(origin, vec, 0.1f);

        initializeGraphics();
    }

    void Draw(float deltaTime) override {
        updateOrigin(originTarget, speedAnimation, deltaTime);
        updateVector(vectorTarget, speedAnimation, deltaTime);

        libCore::ShaderManager::Get("basic")->use();
        libCore::ShaderManager::Get("basic")->setVec3("inputColor", color);

        glBindVertexArray(VAO);

        if (drawSphere) {
            glDrawArrays(GL_TRIANGLE_FAN, 0, sphereVertices.size() / 3);
        }

        glLineWidth(5.0f);
        int lineStartIndex = sphereVertices.size() / 3;
        glDrawArrays(GL_LINES, lineStartIndex, lineVertices.size() / 3);
        glLineWidth(1.0f);

        if (drawArrow) {
            int arrowStartIndex = lineStartIndex + lineVertices.size() / 3;
            glDrawArrays(GL_TRIANGLES, arrowStartIndex, arrowVertices.size() / 3);
        }

        glBindVertexArray(0);
    }

    void initializeGraphics() override {
        if (!initialized) {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            size_t totalSize = sphereVertices.size() + lineVertices.size() + arrowVertices.size();
            glBufferData(GL_ARRAY_BUFFER, totalSize * sizeof(float), nullptr, GL_STATIC_DRAW);

            size_t sphereOffset = 0;
            glBufferSubData(GL_ARRAY_BUFFER, sphereOffset, sphereVertices.size() * sizeof(float), sphereVertices.data());

            size_t lineOffset = sphereVertices.size() * sizeof(float);
            glBufferSubData(GL_ARRAY_BUFFER, lineOffset, lineVertices.size() * sizeof(float), lineVertices.data());

            size_t arrowOffset = lineOffset + lineVertices.size() * sizeof(float);
            glBufferSubData(GL_ARRAY_BUFFER, arrowOffset, arrowVertices.size() * sizeof(float), arrowVertices.data());

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            initialized = true;
        }
    }

    std::vector<float> DrawSphere(float radius, int latSegments, int longSegments, const glm::vec3& origin) {
        std::vector<float> vertices;
        // Simplified sphere drawing algorithm (for demonstration purposes)
        for (int lat = 0; lat <= latSegments; ++lat) {
            for (int lon = 0; lon <= longSegments; ++lon) {
                float theta = lat * M_PI / latSegments;
                float phi = lon * 2 * M_PI / longSegments;
                float x = origin.x + radius * sinf(theta) * cosf(phi);
                float y = origin.y + radius * sinf(theta) * sinf(phi);
                float z = origin.z + radius * cosf(theta);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }
        return vertices;
    }

    std::vector<float> DrawArrow3D(const glm::vec3& origin, const glm::vec3& end, float arrowHeadLength) {
        std::vector<float> vertices;
        // Simplified arrow drawing algorithm
        glm::vec3 direction = glm::normalize(end - origin);
        glm::vec3 up(0, 1, 0);
        glm::vec3 right = glm::cross(direction, up) * (arrowHeadLength * 0.5f);
        glm::vec3 arrowBase = end - (direction * arrowHeadLength);

        vertices.push_back(arrowBase.x - right.x);
        vertices.push_back(arrowBase.y - right.y);
        vertices.push_back(arrowBase.z - right.z);

        vertices.push_back(end.x);
        vertices.push_back(end.y);
        vertices.push_back(end.z);

        vertices.push_back(arrowBase.x + right.x);
        vertices.push_back(arrowBase.y + right.y);
        vertices.push_back(arrowBase.z + right.z);

        return vertices;
    }

    void updateOrigin(const glm::vec3& newOrigin, float speed, float deltaTime) {
        glm::vec3 direction = newOrigin - origin;
        float distance = glm::length(direction);
        if (distance > 0) {
            direction /= distance;
            float movement = speed * deltaTime;
            if (movement > distance) movement = distance;
            origin += direction * movement;
        }
        updateVertices();
    }

    void updateVector(const glm::vec3& newVector, float speed, float deltaTime) {
        glm::vec3 direction = newVector - vector;
        float distance = glm::length(direction);
        if (distance > 0) {
            direction /= distance;
            float movement = speed * deltaTime;
            if (movement > distance) movement = distance;
            vector += direction * movement;
        }
        updateVertices();
    }

    void updateVertices() {
        sphereVertices = DrawSphere(0.05f, 8, 8, origin * axisDirectionFactor);
        lineVertices = { (origin * axisDirectionFactor).x, (origin * axisDirectionFactor).y, (origin * axisDirectionFactor).z,
                         (vector * axisDirectionFactor).x, (vector * axisDirectionFactor).y, (vector * axisDirectionFactor).z };
        arrowVertices = DrawArrow3D(origin * axisDirectionFactor, vector * axisDirectionFactor, 0.1f);
        loadVertexInBuffer();
    }

    void loadVertexInBuffer() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sphereVertices.size() * sizeof(float), sphereVertices.data());
        size_t lineOffset = sphereVertices.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, lineOffset, lineVertices.size() * sizeof(float), lineVertices.data());
        size_t arrowOffset = lineOffset + lineVertices.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, arrowOffset, arrowVertices.size() * sizeof(float), arrowVertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Setters for properties
    void setOriginTarget(const glm::vec3& newOrigin) {
        originTarget = newOrigin;
    }

    void setVectorTarget(const glm::vec3& newVector) {
        vectorTarget = newVector;
    }

    void setDrawSphere(bool value) {
        drawSphere = value;
    }

    void setDrawArrow(bool value) {
        drawArrow = value;
    }

    void SetAxisDirectionFactor(const glm::vec3& directionFactor) {
        axisDirectionFactor = directionFactor;
    }
};