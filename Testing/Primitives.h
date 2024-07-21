#pragma once
#include "glapp.h"

class GameObject
{
public:
	virtual ~GameObject() {}
	virtual void updateBuffers(GLuint& vao, GLuint& vbo, GLuint& ebo, size_t offset) = 0;
	virtual void draw(GLuint &vao, GLuint& vbo, GLuint& ebo, size_t offset) = 0;
};


//class Point3D : public GameObject
//{
//public:
//	Point3D(const glm::vec3& position) : position(position) {}
//	virtual void updateBuffers(GLuint vao, GLuint vbo, GLuint ebo, size_t offset) override;
//	virtual void draw(GLuint vao, GLuint vbo, GLuint ebo, size_t offset) override;
//
//	glm::vec3 position;
//};
class AABB : public GameObject
{
public:
    // Default constructor
    AABB() : m_Center(0.0f), m_HalfExtents(0.0f), aabbVAO(0), aabbVBO(0), aabbEBO(0) {}

    // Constructor with parameters
    AABB(const glm::vec3& center, const glm::vec3& halfExtent)
        : m_Center(center), m_HalfExtents(halfExtent), aabbVAO(0), aabbVBO(0), aabbEBO(0)
    {
        initializeBuffers();
    }

    // Copy constructor
    AABB(const AABB& other)
        : m_Center(other.m_Center), m_HalfExtents(other.m_HalfExtents),
        aabbVAO(0), aabbVBO(0), aabbEBO(0)
    {
        initializeBuffers();
    }

    // Move constructor
    AABB(AABB&& other) noexcept
        : m_Center(std::move(other.m_Center)),
        m_HalfExtents(std::move(other.m_HalfExtents)),
        aabbVAO(other.aabbVAO), aabbVBO(other.aabbVBO), aabbEBO(other.aabbEBO)
    {
        other.aabbVAO = other.aabbVBO = other.aabbEBO = 0;
    }

    // Copy assignment operator
    AABB& operator=(const AABB& other)
    {
        if (this != &other)
        {
            m_Center = other.m_Center;
            m_HalfExtents = other.m_HalfExtents;
            deleteBuffers();
            initializeBuffers();
        }
        return *this;
    }

    // Move assignment operator
    AABB& operator=(AABB&& other) noexcept
    {
        if (this != &other)
        {
            deleteBuffers();
            m_Center = std::move(other.m_Center);
            m_HalfExtents = std::move(other.m_HalfExtents);
            aabbVAO = other.aabbVAO;
            aabbVBO = other.aabbVBO;
            aabbEBO = other.aabbEBO;
            other.aabbVAO = other.aabbVBO = other.aabbEBO = 0;
        }
        return *this;
    }

    // Destructor
    ~AABB() {
        deleteBuffers();
    }

    void setCenter(const glm::vec3& center) {
        m_Center = center;
        updateVertexBuffer();
    }

    void setHalfExtents(const glm::vec3& halfExtents) {
        m_HalfExtents = halfExtents;
        updateVertexBuffer();
    }

    std::vector<glm::vec3> getVertices() const {
        std::vector<glm::vec3> vertices(8);
        vertices[0] = m_Center + glm::vec3(-m_HalfExtents.x, -m_HalfExtents.y, -m_HalfExtents.z);
        vertices[1] = m_Center + glm::vec3(m_HalfExtents.x, -m_HalfExtents.y, -m_HalfExtents.z);
        vertices[2] = m_Center + glm::vec3(m_HalfExtents.x, -m_HalfExtents.y, m_HalfExtents.z);
        vertices[3] = m_Center + glm::vec3(-m_HalfExtents.x, -m_HalfExtents.y, m_HalfExtents.z);
        vertices[4] = m_Center + glm::vec3(-m_HalfExtents.x, m_HalfExtents.y, -m_HalfExtents.z);
        vertices[5] = m_Center + glm::vec3(m_HalfExtents.x, m_HalfExtents.y, -m_HalfExtents.z);
        vertices[6] = m_Center + glm::vec3(m_HalfExtents.x, m_HalfExtents.y, m_HalfExtents.z);
        vertices[7] = m_Center + glm::vec3(-m_HalfExtents.x, m_HalfExtents.y, m_HalfExtents.z);
        return vertices;
    }

    virtual void updateBuffers(GLuint& vao, GLuint& vbo, GLuint& ebo, size_t offset) override {
        updateVertexBuffer();
    }

    virtual void draw(GLuint& vao, GLuint& vbo, GLuint& ebo, size_t offset) override {
        glBindVertexArray(aabbVAO);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    //void initializeIfNeeded() {
    //    if (aabbVAO == 0) {
    //        initializeBuffers();
    //    }
    //}

    GLuint aabbVAO;
    GLuint aabbVBO;
    GLuint aabbEBO;
    glm::vec3 m_Center;
    glm::vec3 m_HalfExtents;

private:
    void initializeBuffers() {
        glGenVertexArrays(1, &aabbVAO);
        glGenBuffers(1, &aabbVBO);
        glGenBuffers(1, &aabbEBO);
        glBindVertexArray(aabbVAO);
        glBindBuffer(GL_ARRAY_BUFFER, aabbVBO);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        unsigned int indices[24] = {
            0, 1, 1, 2, 2, 3, 3, 0,  // Bottom face
            4, 5, 5, 6, 6, 7, 7, 4,  // Top face
            0, 4, 1, 5, 2, 6, 3, 7   // Vertical edges
        };
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aabbEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glBindVertexArray(0);
        updateVertexBuffer();
    }

    void updateVertexBuffer() {
        std::vector<glm::vec3> vertices = getVertices();
        glBindBuffer(GL_ARRAY_BUFFER, aabbVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
    }

    void deleteBuffers() {
        if (aabbVAO != 0) glDeleteVertexArrays(1, &aabbVAO);
        if (aabbVBO != 0) glDeleteBuffers(1, &aabbVBO);
        if (aabbEBO != 0) glDeleteBuffers(1, &aabbEBO);
        aabbVAO = aabbVBO = aabbEBO = 0;
    }
};
//class Ray : public GameObject
//{
//public:
//	Ray(const glm::vec3& start, const glm::vec3& direction) : m_Start(start), m_Direction(direction) {}
//	virtual void updateBuffers(GLuint vao, GLuint vbo, GLuint ebo, size_t offset) override;
//	virtual void draw(GLuint vao, GLuint vbo, GLuint ebo, size_t offset) override;
//
//	glm::vec3 m_Start;
//	glm::vec3 m_Direction;
//
//};

class Sphere : public GameObject
{
public:
  
    // Default constructor
    Sphere()
        : m_Center(0.0f), m_Radius(1.0f), m_Slices(20), m_Stacks(20), m_Color(1.0f),
        sphereVAO(0), sphereVBO(0), sphereEBO(0), indexCount(0)
    {
        createSphereData();
    }

    // Parameterized constructor
    Sphere(const glm::vec3& center, float radius, int slices = 20, int stacks = 20, const glm::vec3& color = glm::vec3(1.0f))
        : m_Center(center), m_Radius(radius), m_Slices(slices), m_Stacks(stacks), m_Color(color),
        sphereVAO(0), sphereVBO(0), sphereEBO(0), indexCount(0)
    {
        createSphereData();
    }

    // Copy constructor
    Sphere(const Sphere& other)
        : m_Center(other.m_Center), m_Radius(other.m_Radius), m_Slices(other.m_Slices), m_Stacks(other.m_Stacks),
        m_Color(other.m_Color), sphereVAO(0), sphereVBO(0), sphereEBO(0), indexCount(0)
    {
        createSphereData();
    }

    // Copy assignment operator
    Sphere& operator=(const Sphere& other)
    {
        if (this != &other)
        {
            m_Center = other.m_Center;
            m_Radius = other.m_Radius;
            m_Color = other.m_Color;
            m_Slices = other.m_Slices;
            m_Stacks = other.m_Stacks;
            createSphereData();
        }
        return *this;
    }

    ~Sphere() {
        if (sphereVAO != 0) glDeleteVertexArrays(1, &sphereVAO);
        if (sphereVBO != 0) glDeleteBuffers(1, &sphereVBO);
        if (sphereEBO != 0) glDeleteBuffers(1, &sphereEBO);
    }

    virtual void updateBuffers(GLuint& vao, GLuint& vbo, GLuint& ebo, size_t offset) override;
    virtual void draw(GLuint& vao, GLuint& vbo, GLuint& ebo, size_t offset) override;

    void createSphereData() {
        std::vector<glm::vec3> vertices;
        std::vector<unsigned int> indices;
        generateSphereData(vertices, indices);

        // Create and bind VAO
        glGenVertexArrays(1, &sphereVAO);
        glBindVertexArray(sphereVAO);

        // Create and bind VBO
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        // Create and bind EBO
        glGenBuffers(1, &sphereEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // Unbind VAO
        glBindVertexArray(0);

        indexCount = static_cast<GLsizei>(indices.size());
    }

    void generateSphereData(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices) const {
        vertices.clear();
        indices.clear();

        for (int i = 0; i <= m_Stacks; ++i) {
            float phi = static_cast<float>(i) * glm::pi<float>() / m_Stacks;
            for (int j = 0; j <= m_Slices; ++j) {
                float theta = static_cast<float>(j) * 2.0f * glm::pi<float>() / m_Slices;
                float x = std::sin(phi) * std::cos(theta);
                float y = std::cos(phi);
                float z = std::sin(phi) * std::sin(theta);
                vertices.push_back(glm::vec3(x, y, z) * m_Radius + m_Center);
            }
        }

        for (int i = 0; i < m_Stacks; ++i) {
            for (int j = 0; j < m_Slices; ++j) {
                int first = i * (m_Slices + 1) + j;
                int second = first + m_Slices + 1;
                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);
                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }
    }

    glm::vec3 m_Center;
    float m_Radius;
     int m_Slices;
     int m_Stacks;
    glm::vec3 m_Color;
    GLuint sphereVAO;
    GLuint sphereVBO;
    GLuint sphereEBO;
    bool m_isVisible = true;
    GLsizei indexCount;
};

class OBB : public GameObject
{
public:
    // Default constructor
    OBB() : m_Center(0.0f), m_HalfExtents(1.0f), m_RotationMatrix(1.0f), obbVAO(0), obbVBO(0), obbEBO(0) {
        initializeBuffers();
    }

    // Parameterized constructor
    OBB(const glm::vec3& center, const glm::vec3& halfExtents, const glm::mat3& rotationMatrix)
        : m_Center(center), m_HalfExtents(halfExtents), m_RotationMatrix(rotationMatrix), obbVAO(0), obbVBO(0), obbEBO(0) {
        initializeBuffers();
    }

    // Copy constructor
    OBB(const OBB& other)
        : m_Center(other.m_Center), m_HalfExtents(other.m_HalfExtents), m_RotationMatrix(other.m_RotationMatrix),
        obbVAO(0), obbVBO(0), obbEBO(0) {
        initializeBuffers();
    }

    // Move constructor
    OBB(OBB&& other) noexcept
        : m_Center(std::move(other.m_Center)),
        m_HalfExtents(std::move(other.m_HalfExtents)),
        m_RotationMatrix(std::move(other.m_RotationMatrix)),
        obbVAO(other.obbVAO), obbVBO(other.obbVBO), obbEBO(other.obbEBO) {
        other.obbVAO = other.obbVBO = other.obbEBO = 0;
    }

    // Copy assignment operator
    OBB& operator=(const OBB& other) {
        if (this != &other) {
            m_Center = other.m_Center;
            m_HalfExtents = other.m_HalfExtents;
            m_RotationMatrix = other.m_RotationMatrix;
            deleteBuffers();
            initializeBuffers();
        }
        return *this;
    }

    // Move assignment operator
    OBB& operator=(OBB&& other) noexcept {
        if (this != &other) {
            deleteBuffers();
            m_Center = std::move(other.m_Center);
            m_HalfExtents = std::move(other.m_HalfExtents);
            m_RotationMatrix = std::move(other.m_RotationMatrix);
            obbVAO = other.obbVAO;
            obbVBO = other.obbVBO;
            obbEBO = other.obbEBO;
            other.obbVAO = other.obbVBO = other.obbEBO = 0;
        }
        return *this;
    }

    // Destructor
    ~OBB() {
        deleteBuffers();
    }

    void setCenter(const glm::vec3& center) {
        m_Center = center;
        updateVertexBuffer();
    }

    void setHalfExtents(const glm::vec3& halfExtents) {
        m_HalfExtents = halfExtents;
        updateVertexBuffer();
    }

    void setRotationMatrix(const glm::mat3& rotationMatrix) {
        m_RotationMatrix = rotationMatrix;
        updateVertexBuffer();
    }

    std::vector<glm::vec3> getVertices() const {
        std::vector<glm::vec3> vertices(8);
        for (int i = 0; i < 8; ++i) {
            glm::vec3 corner = glm::vec3(
                (i & 1) ? m_HalfExtents.x : -m_HalfExtents.x,
                (i & 2) ? m_HalfExtents.y : -m_HalfExtents.y,
                (i & 4) ? m_HalfExtents.z : -m_HalfExtents.z
            );
            vertices[i] = m_Center + m_RotationMatrix * corner;
        }
        return vertices;
    }

    virtual void updateBuffers(GLuint& vao, GLuint& vbo, GLuint& ebo, size_t offset) override {
        updateVertexBuffer();
    }

    virtual void draw(GLuint& vao, GLuint& vbo, GLuint& ebo, size_t offset) override {
        glBindVertexArray(obbVAO);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    GLuint obbVAO;
    GLuint obbVBO;
    GLuint obbEBO;
    glm::vec3 m_Center;
    glm::vec3 m_HalfExtents;
    glm::mat3 m_RotationMatrix;

private:
    void initializeBuffers() {
        glGenVertexArrays(1, &obbVAO);
        glGenBuffers(1, &obbVBO);
        glGenBuffers(1, &obbEBO);
        glBindVertexArray(obbVAO);
        glBindBuffer(GL_ARRAY_BUFFER, obbVBO);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        unsigned int indices[24] = {
            0, 1, 1, 3, 3, 2, 2, 0,  // Bottom face
            4, 5, 5, 7, 7, 6, 6, 4,  // Top face
            0, 4, 1, 5, 2, 6, 3, 7   // Vertical edges
        };
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obbEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glBindVertexArray(0);
        updateVertexBuffer();
    }

    void updateVertexBuffer() {
        std::vector<glm::vec3> vertices = getVertices();
        glBindBuffer(GL_ARRAY_BUFFER, obbVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
    }

    void deleteBuffers() {
        if (obbVAO != 0) glDeleteVertexArrays(1, &obbVAO);
        if (obbVBO != 0) glDeleteBuffers(1, &obbVBO);
        if (obbEBO != 0) glDeleteBuffers(1, &obbEBO);
        obbVAO = obbVBO = obbEBO = 0;
    }

 
};

class BoundingEllipse : public GameObject
{
public:
    // Default constructor
    BoundingEllipse()
        : m_Position(0.0f), m_HalfAxes(1.0f), m_RotationMatrix(1.0f), m_Radius(1.0f),
        ellipseVAO(0), ellipseVBO(0), m_Segments(64) {
        initializeBuffers();
    }

    // Parameterized constructor
    BoundingEllipse(const glm::vec3& position, const glm::vec3& halfAxes, const glm::mat3& rotationMatrix, float radius, int segments = 64)
        : m_Position(position), m_HalfAxes(halfAxes), m_RotationMatrix(rotationMatrix), m_Radius(radius),
        ellipseVAO(0), ellipseVBO(0), m_Segments(segments) {
        initializeBuffers();
    }

    // Copy constructor
    BoundingEllipse(const BoundingEllipse& other)
        : m_Position(other.m_Position), m_HalfAxes(other.m_HalfAxes), m_RotationMatrix(other.m_RotationMatrix),
        m_Radius(other.m_Radius), ellipseVAO(0), ellipseVBO(0), m_Segments(other.m_Segments) {
        initializeBuffers();
    }

    // Move constructor
    BoundingEllipse(BoundingEllipse&& other) noexcept
        : m_Position(std::move(other.m_Position)),
        m_HalfAxes(std::move(other.m_HalfAxes)),
        m_RotationMatrix(std::move(other.m_RotationMatrix)),
        m_Radius(other.m_Radius),
        ellipseVAO(other.ellipseVAO), ellipseVBO(other.ellipseVBO),
        m_Segments(other.m_Segments) {
        other.ellipseVAO = other.ellipseVBO = 0;
    }

    // Copy assignment operator
    BoundingEllipse& operator=(const BoundingEllipse& other) {
        if (this != &other) {
            m_Position = other.m_Position;
            m_HalfAxes = other.m_HalfAxes;
            m_RotationMatrix = other.m_RotationMatrix;
            m_Radius = other.m_Radius;
            m_Segments = other.m_Segments;
            deleteBuffers();
            initializeBuffers();
        }
        return *this;
    }

    // Move assignment operator
    BoundingEllipse& operator=(BoundingEllipse&& other) noexcept {
        if (this != &other) {
            deleteBuffers();
            m_Position = std::move(other.m_Position);
            m_HalfAxes = std::move(other.m_HalfAxes);
            m_RotationMatrix = std::move(other.m_RotationMatrix);
            m_Radius = other.m_Radius;
            ellipseVAO = other.ellipseVAO;
            ellipseVBO = other.ellipseVBO;
            m_Segments = other.m_Segments;
            other.ellipseVAO = other.ellipseVBO = 0;
        }
        return *this;
    }

    // Destructor
    ~BoundingEllipse() {
        deleteBuffers();
    }

    void setPosition(const glm::vec3& position) {
        m_Position = position;
        updateVertexBuffer();
    }

    void setHalfAxes(const glm::vec3& halfAxes) {
        m_HalfAxes = halfAxes;
        updateVertexBuffer();
    }

    void setRotationMatrix(const glm::mat3& rotationMatrix) {
        m_RotationMatrix = rotationMatrix;
        updateVertexBuffer();
    }

    void setRadius(float radius) {
        m_Radius = radius;
        updateVertexBuffer();
    }

    std::vector<glm::vec3> getVertices() const {
        std::vector<glm::vec3> vertices;
        vertices.reserve(m_Segments * 3);  // 3 circles (XY, YZ, XZ planes)

        for (int i = 0; i < m_Segments; ++i) {
            float angle = 2.0f * glm::pi<float>() * i / m_Segments;
            float cosA = std::cos(angle);
            float sinA = std::sin(angle);

            // XY plane
            vertices.push_back(m_Position + m_RotationMatrix * glm::vec3(m_HalfAxes.x * cosA, m_HalfAxes.y * sinA, 0.0f));

            // YZ plane
            vertices.push_back(m_Position + m_RotationMatrix * glm::vec3(0.0f, m_HalfAxes.y * cosA, m_HalfAxes.z * sinA));

            // XZ plane
            vertices.push_back(m_Position + m_RotationMatrix * glm::vec3(m_HalfAxes.x * cosA, 0.0f, m_HalfAxes.z * sinA));
        }

        return vertices;
    }

    virtual void updateBuffers(GLuint& vao, GLuint& vbo, GLuint& ebo, size_t offset) override {
        updateVertexBuffer();
    }

    virtual void draw(GLuint& vao, GLuint& vbo, GLuint& ebo, size_t offset) override {
        glBindVertexArray(ellipseVAO);
        glDrawArrays(GL_LINE_LOOP, 0, m_Segments);
        glDrawArrays(GL_LINE_LOOP, m_Segments, m_Segments);
        glDrawArrays(GL_LINE_LOOP, 2 * m_Segments, m_Segments);
        glBindVertexArray(0);
    }
    GLuint ellipseVAO;
    GLuint ellipseVBO;

    glm::vec3 m_Position;
    glm::vec3 m_HalfAxes;
    glm::mat3 m_RotationMatrix;
    float m_Radius;
    int m_Segments;
private:
    void initializeBuffers() {
        glGenVertexArrays(1, &ellipseVAO);
        glGenBuffers(1, &ellipseVBO);
        glBindVertexArray(ellipseVAO);
        glBindBuffer(GL_ARRAY_BUFFER, ellipseVBO);

        std::vector<glm::vec3> vertices = getVertices();
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void updateVertexBuffer() {
        std::vector<glm::vec3> vertices = getVertices();
        glBindBuffer(GL_ARRAY_BUFFER, ellipseVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
    }

    void deleteBuffers() {
        if (ellipseVAO != 0) glDeleteVertexArrays(1, &ellipseVAO);
        if (ellipseVBO != 0) glDeleteBuffers(1, &ellipseVBO);
        ellipseVAO = ellipseVBO = 0;
    }

};
//class Plane : public GameObject
//{
//public:
//	Plane(const glm::vec4& normal) : m_Normal(normal) {}
//	virtual void updateBuffers(GLuint vao, GLuint vbo, GLuint ebo, size_t offset) override;
//	virtual void draw(GLuint vao, GLuint vbo, GLuint ebo, size_t offset) override;
//
//	glm::vec4 m_Normal;
//
//};

//class Triangle : public GameObject
//{
//public:
//	Triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) : m_V0(v0), m_V1(v1), m_V2(v2) {}
//	virtual void updateBuffers(GLuint vao, GLuint vbo, GLuint ebo, size_t offset) override;
//	virtual void draw(GLuint vao, GLuint vbo, GLuint ebo, size_t offset) override;
//
//	glm::vec3 m_V0;
//	glm::vec3 m_V1;
//	glm::vec3 m_V2;
//
//};