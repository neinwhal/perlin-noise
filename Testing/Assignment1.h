//#pragma once
//#include "glapp.h"
//#include "./Camera.h"
//#include "./Primitives.h"
//#include "./IntersectionTest.h"
//
//
//class Assignment1 : public Scene_Base
//{
//public:
//	Assignment1(Scene_Manager& sceneManager, const std::string& name)
//		: _sceneManager(sceneManager), _name(name), elapsedTime(0.0f),
//		m_cameraSpeed(2.5f), m_cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), m_cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
//		camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 45.0f, (float)GLHelper::width / (float)GLHelper::height, 0.1f, 100.0f),
//		pipCamera(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f, 0.1f, 100.0f)
//	{
//	/*	int pipWidth = GLHelper::width / 5;
//		int pipHeight = GLHelper::height / 5;
//		float pipAspectRatio = static_cast<float>(pipWidth) / static_cast<float>(pipHeight);
//		float mainAspectRatio = static_cast<float>(GLHelper::width) / static_cast<float>(GLHelper::height);
//
//		float orthoScale = 5.0f;
//		float orthoWidth = orthoScale * mainAspectRatio;
//		float orthoHeight = orthoScale;
//
//		if (pipAspectRatio > mainAspectRatio) {
//			orthoWidth *= (pipAspectRatio / mainAspectRatio);
//		}
//		else {
//			orthoHeight *= (mainAspectRatio / pipAspectRatio);
//		}
//
//		pipCamera.setOrthoParams(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight);*/
//	}
//
//	virtual void load() override;
//	virtual void init() override;
//	virtual void update(double dt) override;
//	virtual void draw() override;
//	virtual void cleanup() override;
//	
//	//Change Test
//	void changeTest(int _test);
//	
//
//private:
//
//    // Scene Manager
//    Scene_Manager& _sceneManager;
//    double elapsedTime;
//	// Name of this assignemtn for Scene Manager
//    std::string _name;
//    
//	//Point
//    GLuint pointVAO;
//    GLuint pointVBO;
//
//    //AABB
//    GLuint aabbVAO;
//    GLuint aabbVBO;
//    GLuint aabbEBO;
//
//	//Ray
//	GLuint rayVAO;
//	GLuint rayVBO;
//    
//	// Plane
//	GLuint planeVAO;
//	GLuint planeVBO;
//	GLuint planeEBO;
//
//	// Triangle
//	GLuint triangleVAO;
//	GLuint triangleVBO;
//	GLuint triangleEBO;
//	
//	// Sphere
//	GLuint sphereVAO;
//	GLuint sphereVBO;
//	GLuint sphereEBO;
//
//    // Shader
//    GLSLShader shdr_pgm; 
//
//    // Camera
//	Camera camera;
//	Camera pipCamera;
//
//    float m_cameraSpeed;
//    glm::vec3 m_cameraFront;
//    glm::vec3 m_cameraUp;   
//
//    // Test for intersection
//    enum class Test
//    {
//		PointVsAABB, //done
//		RayVsAABB, //done
//		AABBVsAABB, //done
//		PlaneVsAABB, //done
//		PointVsPlane, // done
//		RayVsPlane, // done
//		PointVsTriangle, // done
//		RayVsTriangle, // done
//		SphereVsPoint, // done
//		SphereVsSphere, // done
//		SphereVsRay, // done
//		SphereVsAABB, // done
//		PlaneVsSphere, // done
//		AABBVSSphere // done
//    };
//    Test test = Test::PointVsAABB;
//	bool intersectionResult;
//	bool wireFrame = false;
//
//	std::vector<std::unique_ptr<GameObject>> gameObjects;
//};