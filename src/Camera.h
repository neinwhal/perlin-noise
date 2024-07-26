#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fov, float aspectRatio, float nearPlane, float farPlane)
        : m_position(position), m_target(target), m_up(up), m_fov(fov), m_aspectRatio(aspectRatio), m_nearPlane(nearPlane), m_farPlane(farPlane),
        m_left(0.0f), m_right(0.0f), m_bottom(0.0f), m_top(0.0f)
    {
        updateViewMatrix();
        updateProjectionMatrix();
    }

    void setOrthoParams(float left, float right, float bottom, float top) {
        m_left = left;
        m_right = right;
        m_bottom = bottom;
        m_top = top;
        updateProjectionMatrix();
    }

    void setPosition(const glm::vec3& position) {
        m_position = position;
        updateViewMatrix();
    }

    void setTarget(const glm::vec3& target) {
        m_target = target;
        updateViewMatrix();
    }

    void setUp(const glm::vec3& up) {
        m_up = up;
        updateViewMatrix();
    }

    void setFOV(float fov) {
        m_fov = fov;
        updateProjectionMatrix();
    }

    void setAspectRatio(float aspectRatio) {
        m_aspectRatio = aspectRatio;
        updateProjectionMatrix();
    }

    void setNearPlane(float nearPlane) {
        m_nearPlane = nearPlane;
        updateProjectionMatrix();
    }

    void setFarPlane(float farPlane) {
        m_farPlane = farPlane;
        updateProjectionMatrix();
    }

    const glm::mat4& getViewMatrix() const {
        return m_viewMatrix;
    }

    const glm::mat4& getProjectionMatrix() const {
        return m_projectionMatrix;
    }
	glm::vec3& getPosition() {
		return m_position;
	}

    glm::vec3 getRight() const {
        return glm::normalize(glm::cross(m_target - m_position, m_up));
    }

    const glm::vec3& getTarget() const {
        return m_target;
    }


    glm::vec3 getUp() const {
        return m_up;
    }

    glm::vec3 getDirection() const {
        return glm::normalize(m_target - m_position);
    }

    float getFOV() const { return m_fov; }
    float getAspectRatio() const { return m_aspectRatio; }
    float getNearPlane() const { return m_nearPlane; }
    float getFarPlane() const { return m_farPlane; }
    // Add this helper function to your Camera class
    bool isProjectionMatrixValid(const glm::mat4& matrix) {
        // Check if any value is NaN or Inf
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (std::isnan(matrix[i][j]) || std::isinf(matrix[i][j])) {
                    return false;
                }
            }
        }

        // Check if the scale factors are within a reasonable range
        if (std::abs(matrix[0][0]) < 0.1f || std::abs(matrix[0][0]) > 100.0f ||
            std::abs(matrix[1][1]) < 0.1f || std::abs(matrix[1][1]) > 100.0f) {
            return false;
        }

        // Check if the projection type is as expected (perspective in this case)
        if (matrix[3][3] != 0.0f || matrix[2][3] != -1.0f) {
            return false;
        }

        return true;
    }

private:
    void updateViewMatrix() {
        m_viewMatrix = glm::lookAt(m_position, m_target, m_up);
    }

    void updateProjectionMatrix() {
        if (m_left != 0.0f || m_right != 0.0f || m_bottom != 0.0f || m_top != 0.0f) {
            // Orthographic projection
            m_projectionMatrix = glm::ortho(m_left, m_right, m_bottom, m_top, m_nearPlane, m_farPlane);
        }
        else {
            // Perspective projection
            // First, let's try using glm::perspective
            m_projectionMatrix = glm::perspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);

            // If glm::perspective is still not working, let's implement the projection matrix manually
            if (!isProjectionMatrixValid(m_projectionMatrix)) {
                float tanHalfFovy = tan(m_fov / 2.0f);
                m_projectionMatrix = glm::mat4(0.0f);
                m_projectionMatrix[0][0] = 1.0f / (m_aspectRatio * tanHalfFovy);
                m_projectionMatrix[1][1] = 1.0f / (tanHalfFovy);
                m_projectionMatrix[2][2] = -(m_farPlane + m_nearPlane) / (m_farPlane - m_nearPlane);
                m_projectionMatrix[2][3] = -1.0f;
                m_projectionMatrix[3][2] = -(2.0f * m_farPlane * m_nearPlane) / (m_farPlane - m_nearPlane);
				std::cout << "Manually calculated projection matrix:" << std::endl;
				//std::exit(EXIT_FAILURE);
            }
        }

        //// Add debug output
        //std::cout << "Updating Projection Matrix:" << std::endl;
        //std::cout << "FOV: " << m_fov << ", Aspect Ratio: " << m_aspectRatio << std::endl;
        //std::cout << "Near Plane: " << m_nearPlane << ", Far Plane: " << m_farPlane << std::endl;
        //std::cout << "Resulting Matrix:" << std::endl;
        //for (int i = 0; i < 4; ++i) {
        //    for (int j = 0; j < 4; ++j) {
        //        std::cout << m_projectionMatrix[i][j] << " ";
        //    }
        //    std::cout << std::endl;
        //}

     
    }

    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    float m_yaw;
    float m_pitch;
    float m_lastX;
    float m_lastY;
    bool m_firstMouse;
    float m_sensitivity;


    float m_left;
    float m_right;
    float m_bottom;
    float m_top;
};

#endif // CAMERA_H