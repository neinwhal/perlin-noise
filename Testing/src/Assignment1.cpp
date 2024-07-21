//
//#include <glhelper.h>
//#include <../Input.h>
//#include <glm/gtc/type_ptr.hpp>
//#include <../KeyCodes.h>
//#include "../imgui-1.87/imgui-1.87/imgui.h"
//#include "../imgui-1.87/imgui-1.87/backends/imgui_impl_glfw.h"
//#include "../imgui-1.87/imgui-1.87/backends/imgui_impl_opengl3.h"
//#include "../Assignment1.h"
//
//void Assignment1::load()
//{
//	std::cout << "RUNNING LOAD" << std::endl;
//        const std::string vert_Shader = R"(
//        #version 450 core
//
//        layout (location = 0) in vec3 aVertexPosition;
//        uniform mat4 transformationMatrix;
//        uniform mat4 mvpMatrix;
//        void main(void) {
//          gl_Position = mvpMatrix * vec4(aVertexPosition, 1.0);
//        }
//        )";
//        const std::string frag_Shader = R"(
//        #version 450 core
//
//        out vec4 fFragColor;
//        uniform vec4 uColor;
//        void main() {
//            fFragColor = uColor; // Output red color
//        }
//        )";
//
//
//    shdr_pgm.CompileShaderFromString(GL_VERTEX_SHADER, vert_Shader);
//    shdr_pgm.CompileShaderFromString(GL_FRAGMENT_SHADER, frag_Shader);
//
//    if (!shdr_pgm.Link() || !shdr_pgm.Validate()) {
//        std::cout << "Shaders is not linked and/or not validated!" << std::endl;
//        std::cout << shdr_pgm.GetLog() << std::endl;
//        std::exit(EXIT_FAILURE);
//    }
//    shdr_pgm.PrintActiveAttribs();
//    shdr_pgm.PrintActiveUniforms();
//}
//
//void Assignment1::init() {
//    std::cout << "RUNNING INIT" << std::endl;
//    // Initialize OpenGL state
//    glViewport(0, 0, GLHelper::width, GLHelper::height);
//    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glfwSetInputMode(GLHelper::ptr_window, GLFW_CURSOR, GLFW_CURSOR);
//
//
//    // Initialize buffers for point
//    glGenVertexArrays(1, &pointVAO);
//    glGenBuffers(1, &pointVBO);
//    glBindVertexArray(pointVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
//    glEnableVertexAttribArray(0);
//    glBindVertexArray(0);
//
//    // Initialize buffers for AABB
//    glGenVertexArrays(1, &aabbVAO);
//    glGenBuffers(1, &aabbVBO);
//    glGenBuffers(1, &aabbEBO);
//
//    glBindVertexArray(aabbVAO);
//
//    glBindBuffer(GL_ARRAY_BUFFER, aabbVBO);
//    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(glm::vec3) *2, nullptr, GL_DYNAMIC_DRAW);
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
//    glEnableVertexAttribArray(0);
//
//    unsigned int indices[48] = {
//        // Lines for the first AABB
//        0, 1, 1, 2, 2, 3, 3, 0, // Bottom Face
//        4, 5, 5, 6, 6, 7, 7, 4, // Top Face
//		0, 4, 1, 5, 2, 6, 3, 7, // Vertical Lines
//
//		// Lines for the second AABB
//		8, 9, 9, 10, 10, 11, 11, 8,     // Bottom Face
//		12, 13, 13, 14, 14, 15, 15, 12, // Top Face
//		8, 12, 9, 13, 10, 14, 11, 15    // Vertical Lines
//    };
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aabbEBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//    glBindVertexArray(0);
//
//
//    // Initialize Ray
//	glGenVertexArrays(1, &rayVAO);
//	glGenBuffers(1, &rayVBO);
//	glBindVertexArray(rayVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
//	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
//	glEnableVertexAttribArray(0);
//	glBindVertexArray(0);
//
//    // Initialize Plane
//    const float planeVertices[] = {
//        // Positions          // Normals
//        -0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  // Vertex 0
//         0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  // Vertex 1
//         0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,  // Vertex 2
//        -0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f   // Vertex 3
//    };
//
//    const unsigned int planeIndices[] = {
//        0, 1, 2,
//        2, 3, 0
//    };
//
//    glGenVertexArrays(1, &planeVAO);
//    glGenBuffers(1, &planeVBO);
//    glGenBuffers(1, &planeEBO);
//
//    glBindVertexArray(planeVAO);
//
//    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);
//
//    // Position attribute
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//
//    // Normal attribute
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(1);
//
//    glBindVertexArray(0);
//
//    // Triangle
//    glGenVertexArrays(1, &triangleVAO);
//    glBindVertexArray(triangleVAO);
//
//    // Generate the VBO
//    glGenBuffers(1, &triangleVBO);
//    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 100, nullptr, GL_DYNAMIC_DRAW);
//
//    // Generate the EBO
//    glGenBuffers(1, &triangleEBO);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleEBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 100, nullptr, GL_DYNAMIC_DRAW);
//
//    // Specify the vertex attribute pointers
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
//    glEnableVertexAttribArray(0);
//
//    // Unbind the VAO
//    glBindVertexArray(0);
//
//    // Create a triangle
//    glm::vec3 v0(0.0f, 0.0f, 0.0f);
//    glm::vec3 v1(1.0f, 0.0f, 0.0f);
//    glm::vec3 v2(0.5f, 1.0f, 0.0f);
//
//    //Create Sphere
//    glGenVertexArrays(1, &sphereVAO);
//    glGenBuffers(1, &sphereVBO);
//    glGenBuffers(1, &sphereEBO);
//
//	// TODO: For now the stacks and slices are hardcoded and different from the m_Slices and m_Stacks in the Sphere class
//    // rmb to change.
//    const int stacks = 20;
//    const int slices = 20;
//    int vertexCount = (stacks + 1) * (slices + 1);
//    int indexCount = (stacks * slices + slices) * 6;
//
//    glBindVertexArray(sphereVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
//    glBufferData(GL_ARRAY_BUFFER, 2 * vertexCount * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * indexCount * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
//    glEnableVertexAttribArray(0);
//    glBindVertexArray(0);
//
//
//	gameObjects.push_back(std::make_unique<Point3D>(glm::vec3(0.0f, 0.0f, 0.0f)));
//    gameObjects.push_back(std::make_unique<AABB>(glm::vec3(0.0f), glm::vec3(1.0f)));
//
//}
//
//
//
////void Assignment1::changeTest(int _test)
////{   
////	std::cout << "Changing test to " << _test << std::endl;
////    test = static_cast<Test>(_test);
////    
////    // Clear the existing gameObjects
////    gameObjects.clear();
////
////    // Initialize gameObjects based on the selected test
////    switch (test)
////    {
////    case Test::PointVsAABB:
////        gameObjects.push_back(std::make_unique<Point3D>(glm::vec3(0.0f)));
////        gameObjects.push_back(std::make_unique<AABB>(glm::vec3(0.0f), glm::vec3(1.0f)));
////        break;
////
////    case Test::RayVsAABB:
////        gameObjects.push_back(std::make_unique<Ray>(glm::vec3(0.0f), glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f))));
////        gameObjects.push_back(std::make_unique<AABB>(glm::vec3(0.0f), glm::vec3(1.0f)));
////        break;
////
////    case Test::AABBVsAABB:
////		gameObjects.push_back(std::make_unique<AABB>(glm::vec3(2.0f), glm::vec3(1.0f)));
////		gameObjects.push_back(std::make_unique<AABB>(glm::vec3(1.0f), glm::vec3(1.0f)));
////		break;
////
////    case Test::PlaneVsAABB:
////		gameObjects.push_back(std::make_unique<Plane>(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
////		gameObjects.push_back(std::make_unique<AABB>(glm::vec3(0.0f), glm::vec3(1.0f)));
////		break;
////
////    case Test::PointVsPlane:
////		gameObjects.push_back(std::make_unique<Point3D>(glm::vec3(0.0f, 0.0f,0.0f)));
////		gameObjects.push_back(std::make_unique<Plane>(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
////		break;
////
////    case Test::RayVsPlane:
////        gameObjects.push_back(std::make_unique<Ray>(glm::vec3(1.0f, 0.0f, 0.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f))));
////        gameObjects.push_back(std::make_unique<Plane>(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
////        break;
////    case Test::PointVsTriangle:
////		gameObjects.push_back(std::make_unique<Point3D>(glm::vec3(0.0f)));
////		gameObjects.push_back(std::make_unique<Triangle>(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.0f)));
////		break;
////
////    case Test::RayVsTriangle:
////		gameObjects.push_back(std::make_unique<Ray>(glm::vec3(0.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))));
////		gameObjects.push_back(std::make_unique<Triangle>(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.0f)));
////		break;
////
////    case Test::SphereVsPoint:
////		gameObjects.push_back(std::make_unique<Sphere>(glm::vec3(0.0f), 1.0f, 20, 20));
////		gameObjects.push_back(std::make_unique<Point3D>(glm::vec3(0.0f)));
////        break;
////
////    case Test::SphereVsSphere:
////		gameObjects.push_back(std::make_unique<Sphere>(glm::vec3(0.0f), 1.0f, 20, 20));
////		gameObjects.push_back(std::make_unique<Sphere>(glm::vec3(0.0f), 1.0f, 20, 20));
////		break;
////
////    case Test::SphereVsRay:
////		gameObjects.push_back(std::make_unique<Sphere>(glm::vec3(0.0f), 1.0f, 20, 20));
////		gameObjects.push_back(std::make_unique<Ray>(glm::vec3(0.0f), glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f))));
////		break;
////
////    case Test::SphereVsAABB:
////		gameObjects.push_back(std::make_unique<Sphere>(glm::vec3(0.0f), 1.0f, 20, 20));
////		gameObjects.push_back(std::make_unique<AABB>(glm::vec3(0.0f), glm::vec3(1.0f)));
////        break;
////
////    case Test::PlaneVsSphere:
////        gameObjects.push_back(std::make_unique<Sphere>(glm::vec3(0.0f), 1.0f, 20, 20));
////		gameObjects.push_back(std::make_unique<Plane>(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
////        break;
////    
////	case Test::AABBVSSphere:
////        gameObjects.push_back(std::make_unique<Sphere>(glm::vec3(0.0f), 1.0f, 20, 20));
////		gameObjects.push_back(std::make_unique<AABB>(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f)));
////		break;
////
////	default:
////		break;
////    }
////
////}
//void Assignment1::update(double dt)
//{
//    elapsedTime += dt;
//    glViewport(0, 0, GLHelper::width, GLHelper::height);
//    float aspectRatio = (float)GLHelper::width / GLHelper::height;
//    camera.setAspectRatio(aspectRatio);
//
//    // Camera movement
//    float cameraSpeed = 2.5f * static_cast<float>(dt);
//    glm::vec3 cameraRight = glm::normalize(glm::cross(GLHelper::cameraFront, GLHelper::cameraUp));
//    glm::vec3 cameraForward = glm::normalize(glm::cross(GLHelper::cameraUp, cameraRight));
//
//    if (RE_input::is_key_pressed(RE_W)) {
//        camera.setPosition(camera.getPosition() + cameraSpeed * cameraForward);
//    }
//    if (RE_input::is_key_pressed(RE_S)) {
//        camera.setPosition(camera.getPosition() - cameraSpeed * cameraForward);
//    }
//    if (RE_input::is_key_pressed(RE_A)) {
//        camera.setPosition(camera.getPosition() - cameraSpeed * cameraRight);
//    }
//    if (RE_input::is_key_pressed(RE_D)) {
//        camera.setPosition(camera.getPosition() + cameraSpeed * cameraRight);
//    }
//    if (RE_input::is_key_pressed(RE_SPACE)) {
//        camera.setPosition(glm::vec3(camera.getPosition() + m_cameraSpeed * static_cast<float>(dt) * m_cameraUp));
//    }
//    if (RE_input::is_key_pressed(RE_LEFT_SHIFT)) {
//        camera.setPosition(camera.getPosition() - m_cameraSpeed * static_cast<float>(dt) * m_cameraUp);
//    }
//
//    // Start of ImGui to control the movement of the objects
//    ImGui_ImplOpenGL3_NewFrame();
//    ImGui_ImplGlfw_NewFrame();
//    ImGui::NewFrame();
//
//	ImGui::Begin("Intersection Test");
//    static const char* items[] = { "Point VS AABB", "Ray VS AABB", "AABB VS AABB", "Plane VS AABB", "Point VS Plane", "Ray VS Plane",
//                                   "Point Vs Triangle", "Ray Vs Triangle", "Sphere Vs Point", "Sphere Vs Sphere", "Sphere Vs Ray",
//                                   "Sphere Vs AABB", "Plane Vs Sphere", "AABB Vs Sphere"};
//    static int _test = static_cast<int>(test);
//
//	// Intersection test selection, if selec one test, call changeTest(_test) and update the variable test to update the switch case.
//    if (ImGui::Combo("Intersection Test", &_test, items, IM_ARRAYSIZE(items)))
//    {
//		changeTest(_test);
//    };
//	const char* wireFrame_text = wireFrame ? "Wireframe is OFF" : "Wireframe is ON";
//    if (ImGui::Button(wireFrame_text))
//    {
//        wireFrame = !wireFrame;
//		std::cout << "Wireframe is " << wireFrame << std::endl;
//    }
//    switch (test)
//    {
//    case Test::PointVsAABB:
//
//        if (gameObjects.size() >= 2)
//        {
//            Point3D& point = *dynamic_cast<Point3D*>(gameObjects[0].get());
//            AABB& aabb     = *dynamic_cast<AABB*>(gameObjects[1].get());
//
//            ImGui::SliderFloat3("Point Position", glm::value_ptr(point.position), -10.0f, 10.0f, "%.5f");
//            ImGui::SliderFloat3("AABB Pos", glm::value_ptr(aabb.m_Center), -10.0f, 10.0f);
//            ImGui::SliderFloat3("AABB Half Extent", glm::value_ptr(aabb.m_HalfExtents), 0.0f, 10.0f);
//
//            point.updateBuffers(pointVAO, pointVBO, 0, 0);
//            aabb.updateBuffers(aabbVAO, aabbVBO, aabbEBO, 0);
//
//            intersectionResult = isPointInAABB(point.position, aabb.m_Center, aabb.m_HalfExtents);
//        }
//        break;
//    
//        case Test::RayVsAABB:
//            if (gameObjects.size() >= 2)
//            {
//                Ray& ray = *dynamic_cast<Ray*>(gameObjects[0].get());
//                AABB& aabb = *dynamic_cast<AABB*>(gameObjects[1].get());
//                ImGui::SliderFloat3("AABB Pos", glm::value_ptr(aabb.m_Center), -10.0f, 10.0f);
//                ImGui::SliderFloat3("AABB Half Extent", glm::value_ptr(aabb.m_HalfExtents), 0.0f, 10.0f);
//
//                ImGui::SliderFloat3("Ray Start", glm::value_ptr(ray.m_Start), -10.0f, 10.0f);
//                if (ImGui::SliderFloat3("Ray Direction", glm::value_ptr(ray.m_Direction), -10.0f, 10.0f))
//                {
//                    float length = glm::length(ray.m_Direction);
//                    if (length > 0.0001f) 
//                    {
//                        ray.m_Direction = glm::normalize(ray.m_Direction);
//                    }
//                    else
//                    {
//                        ray.m_Direction = glm::vec3(1.0f, 0.0f, 0.0f); 
//                    }
//                }
//                ray.updateBuffers(rayVAO, rayVBO, 0, 0);
//				aabb.updateBuffers(aabbVAO, aabbVBO, aabbEBO, 0);
//                intersectionResult = isRayIntersectAABB(ray, aabb.m_Center, aabb.m_HalfExtents);
//            }
//        	break;
//
//        case Test::AABBVsAABB:
//            if (gameObjects.size() >= 2)
//            {
//				AABB& aabb1 = *dynamic_cast<AABB*>(gameObjects[0].get());
//				AABB& aabb2 = *dynamic_cast<AABB*>(gameObjects[1].get());
//
//    
//                ImGui::SliderFloat3("AABB1 Pos", glm::value_ptr(aabb1.m_Center), -10.0f, 10.0f);
//                ImGui::SliderFloat3("AABB1 Half Extent", glm::value_ptr(aabb1.m_HalfExtents), 0.0f, 10.0f);
//
//                ImGui::SliderFloat3("AABB2 Pos", glm::value_ptr(aabb2.m_Center), -10.0f, 10.0f);
//                ImGui::SliderFloat3("AABB2 Half Extent", glm::value_ptr(aabb2.m_HalfExtents), 0.0f, 10.0f);
//
//				aabb1.updateBuffers(aabbVAO, aabbVBO, aabbEBO,0);
//				aabb2.updateBuffers(aabbVAO, aabbVBO, aabbEBO, 1);
//
//				intersectionResult = isAABBIntersectAABB(aabb1, aabb2);
//            }
//            break;
//
//        case Test::PlaneVsAABB:
//            if (gameObjects.size() >= 2)
//            {
//				Plane& plane = *dynamic_cast<Plane*>(gameObjects[0].get());
//				AABB& aabb = *dynamic_cast<AABB*>(gameObjects[1].get());
//                ImGui::SliderFloat3("Plane Normal", glm::value_ptr(plane.m_Normal), -1.0f, 1.0f, "%.5f");
//
//                ImGui::SliderFloat3("AABB Pos", glm::value_ptr(aabb.m_Center), -10.0f, 10.0f);
//                ImGui::SliderFloat3("AABB Half Extent", glm::value_ptr(aabb.m_HalfExtents), 0.0f, 10.0f);
//
//				plane.m_Normal = glm::normalize(plane.m_Normal); 
//				plane.updateBuffers(planeVAO, planeVBO, planeEBO, 0);
//				aabb.updateBuffers(aabbVAO, aabbVBO, aabbEBO, 0);
//
//               intersectionResult = isPlaneIntersectAABB(plane, aabb);
//            }
//            break;
//        case Test::PointVsPlane:
//            if (gameObjects.size() >= 2)
//            {
//				Point3D& point = *dynamic_cast<Point3D*>(gameObjects[0].get());
//				Plane& plane = *dynamic_cast<Plane*>(gameObjects[1].get());
//
//                ImGui::SliderFloat3("Point Position", glm::value_ptr(point.position), -1.0f, 1.0f, "%5f");
//         
//                ImGui::SliderFloat3("Plane Normal", glm::value_ptr(plane.m_Normal), -1.0f, 1.0f, "%.4f");
//                plane.m_Normal = glm::normalize(plane.m_Normal);  // Normalize the normal vector after updating it
//
//                if (glm::length(plane.m_Normal) == 0) {
//                    plane.m_Normal = glm::vec4(1.0f, 0.0f, 0.0f,1.0f); // Default normal
//                }
//
//				point.updateBuffers(pointVAO, pointVBO, 0, 0);
//				plane.updateBuffers(planeVAO, planeVBO, planeEBO, 0);
//
//				intersectionResult = isPointInPlane(point.position, plane);
//            }
//            break;
//
//        case Test::RayVsPlane:
//			if (gameObjects.size() >= 2)
//			{
//				Ray& ray = *dynamic_cast<Ray*>(gameObjects[0].get());
//				Plane& plane = *dynamic_cast<Plane*>(gameObjects[1].get());
//
//				ImGui::SliderFloat3("Ray Start", glm::value_ptr(ray.m_Start), -10.0f, 10.0f);
//				ImGui::SliderFloat3("Ray Direction", glm::value_ptr(ray.m_Direction), -1.0f, 1.0f);
//				ImGui::SliderFloat3("Plane Normal", glm::value_ptr(plane.m_Normal), -1.0f, 1.0f);
//				plane.m_Normal = glm::normalize(plane.m_Normal);  // Normalize the normal vector after updating it
//				ray.m_Direction = glm::normalize(ray.m_Direction);  // Normalize the direction vector after updating it
//				ray.updateBuffers(rayVAO, rayVBO, 0, 0);
//				plane.updateBuffers(planeVAO, planeVBO, planeEBO, 0);
//
//				intersectionResult = isRayIntersectPlane(ray, plane);
//			}
//			break;
//		case Test::PointVsTriangle:
//			if (gameObjects.size() >= 2)
//			{
//				Point3D& point = *dynamic_cast<Point3D*>(gameObjects[0].get());
//				Triangle& triangle = *dynamic_cast<Triangle*>(gameObjects[1].get());
//
//				ImGui::SliderFloat3("Point Position", glm::value_ptr(point.position), -10.0f, 10.0f, "%5f");
//				ImGui::SliderFloat3("Triangle Vertex 0", glm::value_ptr(triangle.m_V0), -10.0f, 10.0f);
//				ImGui::SliderFloat3("Triangle Vertex 1", glm::value_ptr(triangle.m_V1), -10.0f, 10.0f);
//				ImGui::SliderFloat3("Triangle Vertex 2", glm::value_ptr(triangle.m_V2), -10.0f, 10.0f);
//
//				point.updateBuffers(pointVAO, pointVBO, 0, 0);
//				triangle.updateBuffers(triangleVAO, triangleVBO, triangleEBO, 0);
//
//				intersectionResult = isPointInTriangle(point.position, triangle.m_V0, triangle.m_V1, triangle.m_V2);
//			}
//            break;
//        case Test::RayVsTriangle:
//            if (gameObjects.size() >= 2)
//            {
//				Ray& ray = *dynamic_cast<Ray*>(gameObjects[0].get());
//				Triangle& triangle = *dynamic_cast<Triangle*>(gameObjects[1].get());
//
//				ImGui::SliderFloat3("Ray Start", glm::value_ptr(ray.m_Start), -10.0f, 10.0f);
//				ImGui::SliderFloat3("Ray Direction", glm::value_ptr(ray.m_Direction), -1.0f, 1.0f);
//				ImGui::SliderFloat3("Triangle Vertex 0", glm::value_ptr(triangle.m_V0), -10.0f, 10.0f);
//				ImGui::SliderFloat3("Triangle Vertex 1", glm::value_ptr(triangle.m_V1), -10.0f, 10.0f);
//				ImGui::SliderFloat3("Triangle Vertex 2", glm::value_ptr(triangle.m_V2), -10.0f, 10.0f);
//
//				ray.m_Direction = glm::normalize(ray.m_Direction);  // Normalize the direction vector after updating it
//				ray.updateBuffers(rayVAO, rayVBO, 0, 0);
//				triangle.updateBuffers(triangleVAO, triangleVBO, triangleEBO, 0);
//
//				intersectionResult = isRayIntersectTriangle(ray, triangle.m_V0, triangle.m_V1, triangle.m_V2);
//
//            }
//            break;
//		case Test::SphereVsPoint:
//			if (gameObjects.size() >= 2)
//			{
//				Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//				Point3D& point = *dynamic_cast<Point3D*>(gameObjects[1].get());
//
//				ImGui::SliderFloat3("Sphere Center", glm::value_ptr(sphere.m_Center), -10.0f, 10.0f);
//				ImGui::SliderFloat("Sphere Radius", &sphere.m_Radius, 0.0f, 10.0f);
//				ImGui::SliderFloat3("Point Position", glm::value_ptr(point.position), -10.0f, 10.0f);
//
//				sphere.updateBuffers(sphereVAO, sphereVBO, sphereEBO, 0);
//                sphere.updateBuffers(sphereVAO, sphereVBO, sphereEBO, 0);
//
//				point.updateBuffers(pointVAO, pointVBO, 0, 0);
//
//				intersectionResult = isSphereIntersectPoint(sphere, point.position);
//			}
//            break;
//        case Test::SphereVsSphere:
//            if (gameObjects.size() >= 2)
//            {
//				Sphere& sphere1 = *dynamic_cast<Sphere*>(gameObjects[0].get());
//				Sphere& sphere2 = *dynamic_cast<Sphere*>(gameObjects[1].get());
//
//				ImGui::SliderFloat3("Sphere 1 Center", glm::value_ptr(sphere1.m_Center), -10.0f, 10.0f);
//				ImGui::SliderFloat("Sphere 1 Radius", &sphere1.m_Radius, 0.0f, 10.0f);
//				ImGui::SliderFloat3("Sphere 2 Center", glm::value_ptr(sphere2.m_Center), -10.0f, 10.0f);
//				ImGui::SliderFloat("Sphere 2 Radius", &sphere2.m_Radius, 0.0f, 10.0f);
//
//				sphere1.updateBuffers(sphereVAO, sphereVBO, sphereEBO, 0);
//				sphere2.updateBuffers(sphereVAO, sphereVBO, sphereEBO, 1);
//
//				intersectionResult = isSphereIntersectSphere(sphere1, sphere2);
//            }
//            break;
//        case Test::SphereVsRay:
//			if (gameObjects.size() >= 2)
//			{
//				Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//				Ray& ray = *dynamic_cast<Ray*>(gameObjects[1].get());
//
//				ImGui::SliderFloat3("Sphere Center", glm::value_ptr(sphere.m_Center), -10.0f, 10.0f);
//				ImGui::SliderFloat("Sphere Radius", &sphere.m_Radius, 0.0f, 10.0f);
//				ImGui::SliderFloat3("Ray Start", glm::value_ptr(ray.m_Start), -10.0f, 10.0f);
//				ImGui::SliderFloat3("Ray Direction", glm::value_ptr(ray.m_Direction), -1.0f, 1.0f);
//				// Normalize Ray Direction
//				ray.m_Direction = glm::normalize(ray.m_Direction);
//				sphere.updateBuffers(sphereVAO, sphereVBO, sphereEBO, 0);
//				ray.updateBuffers(rayVAO, rayVBO, 0, 0);
//
//				intersectionResult = isSphereIntersectRay(sphere, ray);
//			}
//            break;
//		case Test::SphereVsAABB:
//            if (gameObjects.size() >= 2)
//            {
//				Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//				AABB& aabb = *dynamic_cast<AABB*>(gameObjects[1].get());
//
//				ImGui::SliderFloat3("Sphere Center", glm::value_ptr(sphere.m_Center), -10.0f, 10.0f);
//				ImGui::SliderFloat("Sphere Radius", &sphere.m_Radius, 0.0f, 10.0f);
//                ImGui::SliderFloat3("AABB Pos", glm::value_ptr(aabb.m_Center), -10.0f, 10.0f);
//                ImGui::SliderFloat3("AABB Half Extent", glm::value_ptr(aabb.m_HalfExtents), 0.0f, 10.0f);
//
//				sphere.updateBuffers(sphereVAO, sphereVBO, sphereEBO, 0);
//				aabb.updateBuffers(aabbVAO, aabbVBO, aabbEBO, 0);
//
//				intersectionResult = isSphereIntersectAABB(sphere, aabb);
//            }
//            break;
//		case Test::PlaneVsSphere:  
//            if (gameObjects.size() >= 2)
//            {
//				Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//				Plane& plane = *dynamic_cast<Plane*>(gameObjects[1].get());
//
//				ImGui::SliderFloat3("Sphere Center", glm::value_ptr(sphere.m_Center), -10.0f, 10.0f);
//				ImGui::SliderFloat("Sphere Radius", &sphere.m_Radius, 0.0f, 10.0f);
//				ImGui::SliderFloat3("Plane Normal", glm::value_ptr(plane.m_Normal), -1.0f, 1.0f, "%.5f");
//
//				sphere.updateBuffers(sphereVAO, sphereVBO, sphereEBO, 0);
//				plane.updateBuffers(planeVAO, planeVBO, planeEBO, 0);
//
//				intersectionResult = isPlaneIntersectSphere(plane, sphere);
//            }
//            break;
//        case Test::AABBVSSphere:
//			if (gameObjects.size() >= 2)
//			{
//				Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//				AABB& aabb = *dynamic_cast<AABB*>(gameObjects[1].get());
//
//				ImGui::SliderFloat3("Sphere Center", glm::value_ptr(sphere.m_Center), -10.0f, 10.0f);
//				ImGui::SliderFloat("Sphere Radius", &sphere.m_Radius, 0.0f, 10.0f);
//                ImGui::SliderFloat3("AABB Pos", glm::value_ptr(aabb.m_Center), -10.0f, 10.0f);
//                ImGui::SliderFloat3("AABB Half Extent", glm::value_ptr(aabb.m_HalfExtents), 0.0f, 10.0f);
//
//				sphere.updateBuffers(sphereVAO, sphereVBO, sphereEBO, 0);
//				aabb.updateBuffers(aabbVAO, aabbVBO, aabbEBO, 0);
//
//				intersectionResult = isAABBIntersectSphere(aabb, sphere);
//			}
//			break;
//        }
//    
//	ImGui::End();
//    draw();
//}
//
//
//void Assignment1::draw() {
//    shdr_pgm.Use();
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    if(wireFrame){
//        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    }
//    else {
//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    }
//
//    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    // Set up MVP matrix
//    camera.setTarget(camera.getPosition() + GLHelper::cameraFront);
//    camera.setFOV(GLHelper::fov);
//    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
//    glm::mat4 viewMatrix       = camera.getViewMatrix();
//    glm::mat4 modelMatrix      = glm::mat4(1.0f);
//    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
//
//    GLuint mvpLoc = glGetUniformLocation(shdr_pgm.GetHandle(), "mvpMatrix");
//    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
//
//    glm::vec4 color = intersectionResult ? glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) : glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
//    GLuint colorLoc = glGetUniformLocation(shdr_pgm.GetHandle(), "uColor");
//    glUniform4fv(colorLoc, 1, glm::value_ptr(color));
//
//    switch (test) {
//    case Test::PointVsAABB: // Point-AABB
//        if (gameObjects.size() >= 2 && dynamic_cast<Point3D*>(gameObjects[0].get()) && dynamic_cast<AABB*>(gameObjects[1].get()))
//        {
//            Point3D& point = *dynamic_cast<Point3D*>(gameObjects[0].get());
//            AABB& aabb = *dynamic_cast<AABB*>(gameObjects[1].get());
//
//            point.draw(pointVAO, pointVBO, 0, 0);
//            aabb.draw(aabbVAO, aabbVBO, aabbEBO, 0);
//
//        }
//        break;
//    case Test::RayVsAABB: // Ray-AABB
//        if (gameObjects.size() >= 2 && dynamic_cast<Ray*>(gameObjects[0].get()) && dynamic_cast<AABB*>(gameObjects[1].get()))
//        {
//            Ray& ray = *dynamic_cast<Ray*>(gameObjects[0].get());
//            AABB& aabb = *dynamic_cast<AABB*>(gameObjects[1].get());
//
//            ray.draw(rayVAO, rayVBO, 0, 0);
//            aabb.draw(aabbVAO, aabbVBO, aabbEBO, 0);
//        }
//        break;
//
//    case Test::AABBVsAABB: // AABB-AABB
//        if (gameObjects.size() >= 2 && dynamic_cast<AABB*>(gameObjects[0].get()) && dynamic_cast<AABB*>(gameObjects[1].get()))
//        {
//            AABB& aabb1 = *dynamic_cast<AABB*>(gameObjects[0].get());
//            AABB& aabb2 = *dynamic_cast<AABB*>(gameObjects[1].get());
//
//            aabb1.draw(aabbVAO, aabbVBO, aabbEBO, 0);
//            aabb2.draw(aabbVAO, aabbVBO, aabbEBO, 1);
//        }
//        break;
//    case Test::PlaneVsAABB: // Plane-AABB
//        if (gameObjects.size() >= 2 && dynamic_cast<Plane*>(gameObjects[0].get()) && dynamic_cast<AABB*>(gameObjects[1].get()))
//        {
//            Plane& plane = *dynamic_cast<Plane*>(gameObjects[0].get());
//            AABB& aabb = *dynamic_cast<AABB*>(gameObjects[1].get());
//
//            plane.draw(planeVAO, planeVBO, planeEBO, 0);
//            aabb.draw(aabbVAO, aabbVBO, aabbEBO, 0);
//        }
//        break;
//
//    case Test::PointVsPlane:
//        if (gameObjects.size() >= 2 && dynamic_cast<Point3D*>(gameObjects[0].get()) && dynamic_cast<Plane*>(gameObjects[1].get()))
//        {
//            Point3D& point = *dynamic_cast<Point3D*>(gameObjects[0].get());
//            Plane& plane = *dynamic_cast<Plane*>(gameObjects[1].get());
//
//            point.draw(pointVAO, pointVBO, 0, 0);
//            plane.draw(planeVAO, planeVBO, planeEBO, 0);
//        }
//        break;
//    case Test::RayVsPlane:
//        if (gameObjects.size() >= 2 && dynamic_cast<Ray*>(gameObjects[0].get()) && dynamic_cast<Plane*>(gameObjects[1].get()))
//        {
//            Ray& ray = *dynamic_cast<Ray*>(gameObjects[0].get());
//            Plane& plane = *dynamic_cast<Plane*>(gameObjects[1].get());
//
//            ray.draw(rayVAO, rayVBO, 0, 0);
//            plane.draw(planeVAO, planeVBO, planeEBO, 0);
//        }
//        break;
//
//    case Test::PointVsTriangle:
//        if (gameObjects.size() >= 2 && dynamic_cast<Point3D*>(gameObjects[0].get()) && dynamic_cast<Triangle*>(gameObjects[1].get()))
//        {
//            Point3D& point = *dynamic_cast<Point3D*>(gameObjects[0].get());
//            Triangle& triangle = *dynamic_cast<Triangle*>(gameObjects[1].get());
//
//            point.draw(pointVAO, pointVBO, 0, 0);
//            triangle.draw(triangleVAO, triangleVBO, triangleEBO, 0);
//        }
//        break;
//    case Test::RayVsTriangle:
//        if (gameObjects.size() >= 2 && dynamic_cast<Ray*>(gameObjects[0].get()) && dynamic_cast<Triangle*>(gameObjects[1].get()))
//        {
//            Ray& ray = *dynamic_cast<Ray*>(gameObjects[0].get());
//            Triangle& triangle = *dynamic_cast<Triangle*>(gameObjects[1].get());
//
//            ray.draw(rayVAO, rayVBO, 0, 0);
//            triangle.draw(triangleVAO, triangleVBO, triangleEBO, 0);
//        }
//        break;
//    case Test::SphereVsPoint:
//        if (gameObjects.size() >= 2 && dynamic_cast<Sphere*>(gameObjects[0].get()) && dynamic_cast<Point3D*>(gameObjects[1].get()))
//        {
//            Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//            Point3D& point = *dynamic_cast<Point3D*>(gameObjects[1].get());
//
//            sphere.draw(sphereVAO, sphereVBO, sphereEBO, 0);
//            point.draw(pointVAO, pointVBO, 0, 0);
//        }
//        break;
//    case Test::SphereVsSphere:
//        if (gameObjects.size() >= 2 && dynamic_cast<Sphere*>(gameObjects[0].get()) && dynamic_cast<Sphere*>(gameObjects[1].get()))
//        {
//            Sphere& sphere1 = *dynamic_cast<Sphere*>(gameObjects[0].get());
//            Sphere& sphere2 = *dynamic_cast<Sphere*>(gameObjects[1].get());
//
//            sphere1.draw(sphereVAO, sphereVBO, sphereEBO, 0);
//            sphere2.draw(sphereVAO, sphereVBO, sphereEBO, 1);
//        }
//        break;
//    case Test::SphereVsRay:
//        if (gameObjects.size() >= 2 && dynamic_cast<Sphere*>(gameObjects[0].get()) && dynamic_cast<Ray*>(gameObjects[1].get()))
//        {
//            // Ray and Sphere object
//            Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//            Ray& ray = *dynamic_cast<Ray*>(gameObjects[1].get());
//
//            sphere.draw(sphereVAO, sphereVBO, sphereEBO, 0);
//            ray.draw(rayVAO, rayVBO, 0, 0);
//        }
//        break;
//    case Test::SphereVsAABB:
//        if (gameObjects.size() >= 2 && dynamic_cast<Sphere*>(gameObjects[0].get()) && dynamic_cast<AABB*>(gameObjects[1].get()))
//        {
//            Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//            AABB& aabb = *dynamic_cast<AABB*>(gameObjects[1].get());
//
//            sphere.draw(sphereVAO, sphereVBO, sphereEBO, 0);
//            aabb.draw(aabbVAO, aabbVBO, aabbEBO, 0);
//        }
//        break;
//    case Test::PlaneVsSphere:
//        if (gameObjects.size() >= 2 && dynamic_cast<Plane*>(gameObjects[1].get()) && dynamic_cast<Sphere*>(gameObjects[0].get()))
//		{
//            Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//			Plane& plane = *dynamic_cast<Plane*>(gameObjects[1].get());
//
//			plane.draw(planeVAO, planeVBO, planeEBO, 0);
//			sphere.draw(sphereVAO, sphereVBO, sphereEBO, 0);
//		}
//        break;
//    case Test::AABBVSSphere:
//		if (gameObjects.size() >= 2 && dynamic_cast<AABB*>(gameObjects[1].get()) && dynamic_cast<Sphere*>(gameObjects[0].get()))
//		{
//			AABB& aabb = *dynamic_cast<AABB*>(gameObjects[1].get());
//			Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//
//			aabb.draw(aabbVAO, aabbVBO, aabbEBO, 0);
//			sphere.draw(sphereVAO, sphereVBO, sphereEBO, 0);
//		}
//		break;
//
//    default:
//		break;
//    }
//    
//   // PIP CAMERA
//    int pipWidth = GLHelper::width / 5;
//    int pipHeight = GLHelper::height / 5;
//    int pipX = GLHelper::width - pipWidth;
//    int pipY = GLHelper::height - pipHeight;
//
//    float pipAspectRatio = static_cast<float>(pipWidth) / static_cast<float>(pipHeight);
//    float mainAspectRatio = static_cast<float>(GLHelper::width) / static_cast<float>(GLHelper::height);
//
//    float orthoScale = 5.0f;
//    float orthoWidth = orthoScale * mainAspectRatio;
//    float orthoHeight = orthoScale;
//
//    if (pipAspectRatio > mainAspectRatio) {
//        orthoWidth *= (pipAspectRatio / mainAspectRatio);
//    }
//    else {
//        orthoHeight *= (mainAspectRatio / pipAspectRatio);
//    }
//
//    pipCamera.setOrthoParams(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight);
//
//    glEnable(GL_SCISSOR_TEST);
//    glScissor(pipX, pipY, pipWidth, pipHeight);
//    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glDisable(GL_SCISSOR_TEST);
//    glViewport(pipX, pipY, pipWidth, pipHeight);
//
//    glm::mat4 pipProjectionMatrix = pipCamera.getProjectionMatrix();
//    glm::mat4 pipViewMatrix = pipCamera.getViewMatrix();
//    glm::mat4 pipMVPMatrix = pipProjectionMatrix * pipViewMatrix * modelMatrix;
//
//    GLuint pipMVPLoc = glGetUniformLocation(shdr_pgm.GetHandle(), "mvpMatrix");
//    glUniformMatrix4fv(pipMVPLoc, 1, GL_FALSE, glm::value_ptr(pipMVPMatrix));
//
//
//    switch (test) {
//    case Test::PointVsAABB: // Point-AABB
//        if (gameObjects.size() >= 2 && dynamic_cast<Point3D*>(gameObjects[0].get()) && dynamic_cast<AABB*>(gameObjects[1].get()))
//        {
//            Point3D& point = *dynamic_cast<Point3D*>(gameObjects[0].get());
//            AABB& aabb = *dynamic_cast<AABB*>(gameObjects[1].get());
//
//            point.draw(pointVAO, pointVBO, 0, 0);
//            aabb.draw(aabbVAO, aabbVBO, aabbEBO, 0);
//
//        }
//        break;
//    case Test::RayVsAABB: // Ray-AABB
//        if (gameObjects.size() >= 2 && dynamic_cast<Ray*>(gameObjects[0].get()) && dynamic_cast<AABB*>(gameObjects[1].get()))
//        {
//            Ray& ray = *dynamic_cast<Ray*>(gameObjects[0].get());
//            AABB& aabb = *dynamic_cast<AABB*>(gameObjects[1].get());
//
//            ray.draw(rayVAO, rayVBO, 0, 0);
//            aabb.draw(aabbVAO, aabbVBO, aabbEBO, 0);
//        }
//        break;
//
//    case Test::AABBVsAABB: // AABB-AABB
//        if (gameObjects.size() >= 2 && dynamic_cast<AABB*>(gameObjects[0].get()) && dynamic_cast<AABB*>(gameObjects[1].get()))
//        {
//            AABB& aabb1 = *dynamic_cast<AABB*>(gameObjects[0].get());
//            AABB& aabb2 = *dynamic_cast<AABB*>(gameObjects[1].get());
//
//            aabb1.draw(aabbVAO, aabbVBO, aabbEBO, 0);
//            aabb2.draw(aabbVAO, aabbVBO, aabbEBO, 1);
//        }
//        break;
//    case Test::PlaneVsAABB: // Plane-AABB
//        if (gameObjects.size() >= 2 && dynamic_cast<Plane*>(gameObjects[0].get()) && dynamic_cast<AABB*>(gameObjects[1].get()))
//        {
//            Plane& plane = *dynamic_cast<Plane*>(gameObjects[0].get());
//            AABB& aabb = *dynamic_cast<AABB*>(gameObjects[1].get());
//
//            plane.draw(planeVAO, planeVBO, planeEBO, 0);
//            aabb.draw(aabbVAO, aabbVBO, aabbEBO, 0);
//        }
//        break;
//
//    case Test::PointVsPlane:
//        if (gameObjects.size() >= 2 && dynamic_cast<Point3D*>(gameObjects[0].get()) && dynamic_cast<Plane*>(gameObjects[1].get()))
//        {
//            Point3D& point = *dynamic_cast<Point3D*>(gameObjects[0].get());
//            Plane& plane = *dynamic_cast<Plane*>(gameObjects[1].get());
//
//            point.draw(pointVAO, pointVBO, 0, 0);
//            plane.draw(planeVAO, planeVBO, planeEBO, 0);
//        }
//        break;
//    case Test::RayVsPlane:
//        if (gameObjects.size() >= 2 && dynamic_cast<Ray*>(gameObjects[0].get()) && dynamic_cast<Plane*>(gameObjects[1].get()))
//        {
//            Ray& ray = *dynamic_cast<Ray*>(gameObjects[0].get());
//            Plane& plane = *dynamic_cast<Plane*>(gameObjects[1].get());
//
//            ray.draw(rayVAO, rayVBO, 0, 0);
//            plane.draw(planeVAO, planeVBO, planeEBO, 0);
//        }
//        break;
//
//    case Test::PointVsTriangle:
//        if (gameObjects.size() >= 2 && dynamic_cast<Point3D*>(gameObjects[0].get()) && dynamic_cast<Triangle*>(gameObjects[1].get()))
//        {
//            Point3D& point = *dynamic_cast<Point3D*>(gameObjects[0].get());
//            Triangle& triangle = *dynamic_cast<Triangle*>(gameObjects[1].get());
//
//            point.draw(pointVAO, pointVBO, 0, 0);
//            triangle.draw(triangleVAO, triangleVBO, triangleEBO, 0);
//        }
//        break;
//    case Test::RayVsTriangle:
//        if (gameObjects.size() >= 2 && dynamic_cast<Ray*>(gameObjects[0].get()) && dynamic_cast<Triangle*>(gameObjects[1].get()))
//        {
//            Ray& ray = *dynamic_cast<Ray*>(gameObjects[0].get());
//            Triangle& triangle = *dynamic_cast<Triangle*>(gameObjects[1].get());
//
//            ray.draw(rayVAO, rayVBO, 0, 0);
//            triangle.draw(triangleVAO, triangleVBO, triangleEBO, 0);
//        }
//        break;
//    case Test::SphereVsPoint:
//        if (gameObjects.size() >= 2 && dynamic_cast<Sphere*>(gameObjects[0].get()) && dynamic_cast<Point3D*>(gameObjects[1].get()))
//        {
//            Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//            Point3D& point = *dynamic_cast<Point3D*>(gameObjects[1].get());
//
//            sphere.draw(sphereVAO, sphereVBO, sphereEBO, 0);
//            point.draw(pointVAO, pointVBO, 0, 0);
//        }
//        break;
//    case Test::SphereVsSphere:
//        if (gameObjects.size() >= 2 && dynamic_cast<Sphere*>(gameObjects[0].get()) && dynamic_cast<Sphere*>(gameObjects[1].get()))
//        {
//            Sphere& sphere1 = *dynamic_cast<Sphere*>(gameObjects[0].get());
//            Sphere& sphere2 = *dynamic_cast<Sphere*>(gameObjects[1].get());
//
//            sphere1.draw(sphereVAO, sphereVBO, sphereEBO, 0);
//            sphere2.draw(sphereVAO, sphereVBO, sphereEBO, 1);
//        }
//        break;
//    case Test::SphereVsRay:
//        if (gameObjects.size() >= 2 && dynamic_cast<Sphere*>(gameObjects[0].get()) && dynamic_cast<Ray*>(gameObjects[1].get()))
//        {
//            // Ray and Sphere object
//            Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//            Ray& ray = *dynamic_cast<Ray*>(gameObjects[1].get());
//
//            sphere.draw(sphereVAO, sphereVBO, sphereEBO, 0);
//            ray.draw(rayVAO, rayVBO, 0, 0);
//        }
//        break;
//    case Test::SphereVsAABB:
//        if (gameObjects.size() >= 2 && dynamic_cast<Sphere*>(gameObjects[0].get()) && dynamic_cast<AABB*>(gameObjects[1].get()))
//        {
//            Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//            AABB& aabb = *dynamic_cast<AABB*>(gameObjects[1].get());
//
//            sphere.draw(sphereVAO, sphereVBO, sphereEBO, 0);
//            aabb.draw(aabbVAO, aabbVBO, aabbEBO, 0);
//        }
//        break;
//    case Test::PlaneVsSphere:
//        if (gameObjects.size() >= 2 && dynamic_cast<Plane*>(gameObjects[1].get()) && dynamic_cast<Sphere*>(gameObjects[0].get()))
//        {
//            Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//            Plane& plane = *dynamic_cast<Plane*>(gameObjects[1].get());
//
//            plane.draw(planeVAO, planeVBO, planeEBO, 0);
//            sphere.draw(sphereVAO, sphereVBO, sphereEBO, 0);
//        }
//        break;
//    case Test::AABBVSSphere:
//        if (gameObjects.size() >= 2 && dynamic_cast<AABB*>(gameObjects[1].get()) && dynamic_cast<Sphere*>(gameObjects[0].get()))
//        {
//            AABB& aabb = *dynamic_cast<AABB*>(gameObjects[1].get());
//            Sphere& sphere = *dynamic_cast<Sphere*>(gameObjects[0].get());
//
//            aabb.draw(aabbVAO, aabbVBO, aabbEBO, 0);
//            sphere.draw(sphereVAO, sphereVBO, sphereEBO, 0);
//        }
//        break;
//
//        break;
//
//    default:
//        break;
//    }
//
//    
//    glViewport(0, 0, GLHelper::width, GLHelper::height);
//
//
//    shdr_pgm.UnUse();
//
//    // Render ImGui
//    ImGui::Render();
//    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//}
//
//void Assignment1::cleanup()
//{
//    // IMGUI CLEAN UP
//    ImGui_ImplOpenGL3_Shutdown();
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();
//
//    // FREE THE VAO, VBO, EBO
//
//    // Point
//    glDeleteVertexArrays(1, &pointVAO);
//    glDeleteBuffers(1, &pointVBO);
//
//    // AABB
//    glDeleteVertexArrays(1, &aabbVAO);
//    glDeleteBuffers(1, &aabbVBO);
//    glDeleteBuffers(1, &aabbEBO);
//
//    // Ray
//    glDeleteVertexArrays(1, &rayVAO);
//    glDeleteBuffers(1, &rayVBO);
//
//    // Plane
//    glDeleteVertexArrays(1, &planeVAO);
//    glDeleteBuffers(1, &planeVBO);
//    glDeleteBuffers(1, &planeEBO);
//
//    // Triangle
//    glDeleteVertexArrays(1, &triangleVAO);
//    glDeleteBuffers(1, &triangleVBO);
//    glDeleteBuffers(1, &triangleEBO);
//
//    // Sphere
//    glDeleteVertexArrays(1, &sphereVAO);
//    glDeleteBuffers(1, &sphereVBO);
//    glDeleteBuffers(1, &sphereEBO);
//}
