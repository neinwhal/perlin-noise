#include "./Primitives.h"
#include <glm/gtc/type_ptr.hpp>
#include "../Assignment2.h"
//void Point3D::updateBuffers(GLuint vao, GLuint vbo, GLuint ebo, size_t offset)
//{
//    static_cast<void>(ebo);
//	static_cast<void>(offset);
//	static_cast<void>(vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, vao);
//    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3), glm::value_ptr(position));
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//}
//
//void Point3D::draw(GLuint vao, GLuint vbo, GLuint ebo, size_t offset)
//{
//	static_cast<void>(vbo);
//	static_cast<void>(ebo);
//	static_cast<void>(offset);
//    // Render point
//    glPointSize(20.0f);
//    glBindVertexArray(vao);
//    glDrawArrays(GL_POINTS, 0, 1);
//    glBindVertexArray(0);
//}

//void AABB::updateBuffers(GLuint& vao, GLuint& vbo, GLuint& ebo, size_t offset)
//{
//    static_cast<void>(ebo);
//
//    glm::vec3 min = m_Center - m_HalfExtents;
//    glm::vec3 max = m_Center + m_HalfExtents;
//
//    glm::vec3 vertices[] = {
//        glm::vec3(min.x, min.y, min.z), // 0
//        glm::vec3(max.x, min.y, min.z), // 1
//        glm::vec3(max.x, max.y, min.z), // 2
//        glm::vec3(min.x, max.y, min.z), // 3
//        glm::vec3(min.x, min.y, max.z), // 4
//        glm::vec3(max.x, min.y, max.z), // 5
//        glm::vec3(max.x, max.y, max.z), // 6
//        glm::vec3(min.x, max.y, max.z)  // 7
//    };
//
//    glBindVertexArray(vao);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(vertices), sizeof(vertices), vertices);
//    glBindVertexArray(0);
//}
//
//
//void AABB::draw(GLuint& vao, GLuint& vbo, GLuint& ebo, size_t offset)
//{
//    static_cast<void>(vbo);
//    glBindVertexArray(vao);
//
//    // Bind the element buffer
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//
//    // Draw the elements for this AABB
//    glDrawElementsBaseVertex(GL_LINES, 24, GL_UNSIGNED_INT, nullptr, offset * 8);
//
//    glBindVertexArray(0);
//}


//void Ray::updateBuffers(GLuint vao, GLuint vbo, GLuint ebo, size_t offset)
//{
//    static_cast<void>(ebo);
//	static_cast<void>(offset);
//    glm::vec3 rayEnd = m_Start + m_Direction * 5.0f;
//
//    glBindVertexArray(vao);
//
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glm::vec3 vertices[] = { m_Start, rayEnd };
//    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
//
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
//}
//
//
//void Ray::draw(GLuint vao, GLuint vbo, GLuint ebo, size_t offset)
//{
//    static_cast<void>(offset);
//    static_cast<void>(vbo);
//	static_cast<void>(ebo);
//    // Render Ray
//    glBindVertexArray(vao);
//    glDrawArrays(GL_LINES, 0, 2);
//    glBindVertexArray(0);
//}
//
//void Plane::updateBuffers(GLuint vao, GLuint vbo, GLuint ebo, size_t offset)
//{
//    glm::vec3 normal = glm::normalize(glm::vec3(m_Normal));
//
//    glm::vec3 up = normal;
//
//    glm::vec3 right = glm::normalize(glm::cross(up, glm::vec3(0.0f, 0.0f, 1.0f)));
//    glm::vec3 forward = glm::normalize(glm::cross(right, up));
//
//    float halfSize = 50.f;
//
//    glm::vec3 vertices[4] = {
//        -halfSize * right - halfSize * forward,
//        halfSize * right - halfSize * forward,
//        halfSize * right + halfSize * forward,
//        -halfSize * right + halfSize * forward
//    };
//
//    float planeVertices[24];
//
//    for (int i = 0; i < 4; ++i) {
//        planeVertices[i * 6 + 0] = vertices[i].x;
//        planeVertices[i * 6 + 1] = vertices[i].y;
//        planeVertices[i * 6 + 2] = vertices[i].z;
//        planeVertices[i * 6 + 3] = normal.x;
//        planeVertices[i * 6 + 4] = normal.y;
//        planeVertices[i * 6 + 5] = normal.z;
//    }
//
//    unsigned int planeIndices[] = {
//        0, 1, 2,
//        2, 3, 0
//    };
//
//    glBindVertexArray(vao);
//
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(planeVertices), sizeof(planeVertices), planeVertices);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(planeIndices), sizeof(planeIndices), planeIndices);
//
//    glBindVertexArray(0);
//}
//
//void Plane::draw(GLuint vao, GLuint vbo, GLuint ebo, size_t offset)
//{
//    static_cast<void>(vbo);
//
//    glBindVertexArray(vao);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//    glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, offset * 4);
//    glBindVertexArray(0);
//}
//
//void Triangle::updateBuffers(GLuint vao, GLuint vbo, GLuint ebo, size_t offset)
//{
//    static_cast<void>(vbo);
//
//    // Create an array to store the triangle vertices
//    glm::vec3 vertices[3] = { m_V0, m_V1, m_V2 };
//
//    // Create an array to store the triangle indices
//    unsigned int indices[3] = { 0, 1, 2 };
//
//    // Bind the VAO
//    glBindVertexArray(vao);
//
//    // Update the vertex buffer with the triangle vertices
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(vertices), sizeof(vertices), vertices);
//
//    // Update the element buffer with the triangle indices
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(indices), sizeof(indices), indices);
//
//    // Unbind the VAO
//    glBindVertexArray(0);
//}
//
//void Triangle::draw(GLuint vao, GLuint vbo, GLuint ebo, size_t offset)
//{
//    static_cast<void>(ebo);
//    static_cast<void>(vbo);
//    // Bind the VAO
//    glBindVertexArray(vao);
//
//    // Draw the triangle
//    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned int)));
//
//    // Unbind the VAO
//    glBindVertexArray(0);
//}


void Sphere::updateBuffers(GLuint& vao, GLuint& vbo, GLuint& ebo, size_t offset) {
    // Delete old buffers if they exist
    if (sphereVAO != 0) glDeleteVertexArrays(1, &sphereVAO);
    if (sphereVBO != 0) glDeleteBuffers(1, &sphereVBO);
    if (sphereEBO != 0) glDeleteBuffers(1, &sphereEBO);

    // Recreate sphere data
    createSphereData();

    //std::cout << "Sphere buffers updated. Vertices: " << vertices.size()
    //    << ", Indices: " << indices.size()
    //    << ", VAO: " << sphereVAO
    //    << ", VBO: " << sphereVBO
    //    << ", EBO: " << sphereEBO << std::endl;
}
void Sphere::draw(GLuint& vao, GLuint& vbo, GLuint& ebo, size_t offset) {
    if (!m_isVisible) return;

    if (indexCount == 0) {
        std::cerr << "Error: No indices to draw (indexCount is 0)" << std::endl;
        return;
    }

    glBindVertexArray(sphereVAO);

    if (glIsBuffer(sphereEBO) == GL_FALSE) {
        std::cerr << "Error: Invalid EBO in Sphere::draw" << std::endl;
        return;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);

    GLint bufferSize;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

    if (bufferSize < indexCount * sizeof(unsigned int)) {
        std::cerr << "Error: Buffer size (" << bufferSize << ") is smaller than expected ("
            << (indexCount * sizeof(unsigned int)) << ") in Sphere::draw" << std::endl;
        return;
    }

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned int)));

    glBindVertexArray(0);
}