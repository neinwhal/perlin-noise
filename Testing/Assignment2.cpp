#include "Assignment2.h"
#include <glhelper.h>
#include <../Input.h>
#include <glm/gtc/type_ptr.hpp>
#include <../KeyCodes.h>
#include "../imgui-1.87/imgui-1.87/imgui.h"
#include "../imgui-1.87/imgui-1.87/backends/imgui_impl_glfw.h"
#include "../imgui-1.87/imgui-1.87/backends/imgui_impl_opengl3.h"

void Assignment2::load()
{
    std::cout << "RUNNING LOAD" << std::endl;
    const std::string vert_Shader = R"(
#version 450 core
layout (location = 0) in vec3 aVertexPosition;
layout (location = 1) in vec3 aVertexNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

void main()
{
    FragPos = vec3(modelMatrix * vec4(aVertexPosition, 1.0));
    Normal = normalMatrix * aVertexNormal;
    TexCoords = aTexCoords;
    
    gl_Position = mvpMatrix * vec4(aVertexPosition, 1.0);
}
        )";
    const std::string frag_Shader = R"(
#version 450 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
//in vec3 VertexColor;  // Add this line

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform bool useTexture;
uniform sampler2D textureSampler;


void main()
{

// Ensure we have some color even without proper lighting
    vec3 result = material.diffuse;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);

    // Ambient
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // Specular
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);

} 

)";


    shdr_pgm.CompileShaderFromString(GL_VERTEX_SHADER, vert_Shader);
    shdr_pgm.CompileShaderFromString(GL_FRAGMENT_SHADER, frag_Shader);

    if (!shdr_pgm.Link() || !shdr_pgm.Validate()) {
        std::cout << "Shaders is not linked and/or not validated!" << std::endl;
        std::cout << shdr_pgm.GetLog() << std::endl;
        std::exit(EXIT_FAILURE);
    }


    const std::string bounding_sphere_vert_shader = R"(
     #version 450 core
layout (location = 0) in vec3 aPos;
uniform mat4 mvpMatrix;
uniform int sphereType;
flat out int vSphereType;

void main()
{
    gl_Position = mvpMatrix * vec4(aPos, 1.0);
    vSphereType = sphereType;
}
    )";

    const std::string bounding_sphere_frag_shader = R"(
       #version 450 core
flat in int vSphereType;
out vec4 FragColor;
uniform vec3 color;
void main()
{
    if (vSphereType == 0) {
        FragColor = vec4(1.0, 1.0, 0.0, 1.0); // Yellow Ritter
    } else if (vSphereType == 1) {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Green Larson
    } else if (vSphereType == 2) {
        FragColor = vec4(0.0, 0.0, 1.0, 1.0); // Blue Eigen
    } else if (vSphereType == 3){
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red aabb
    } else if (vSphereType == 4)
    {
	    FragColor = vec4(color, 1.0); // Top Down AABB/Sphere
    }
}
    )";

    bounding_sphere_shader.CompileShaderFromString(GL_VERTEX_SHADER, bounding_sphere_vert_shader);
    bounding_sphere_shader.CompileShaderFromString(GL_FRAGMENT_SHADER, bounding_sphere_frag_shader);
   
    if (!bounding_sphere_shader.Link() || !bounding_sphere_shader.Validate()) {
        std::cout << "Bounding sphere shader is not linked and/or not validated!" << std::endl;
        std::cout << bounding_sphere_shader.GetLog() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    else {
		std::cout << "Bounding sphere shader is linked and validated!" << std::endl;
    }
    // After compiling shaders
     bounding_sphere_shader.PrintActiveAttribs();
     bounding_sphere_shader.PrintActiveUniforms();
    shdr_pgm.PrintActiveAttribs();
    shdr_pgm.PrintActiveUniforms();
    checkGLError("Shader compilation and linking");

    // Section 4
    sections[4].parts.push_back(std::make_unique<Model>("../Models/ppsection4/part_a/g0.obj"));
    sections[4].parts.push_back(std::make_unique<Model>("../Models/ppsection4/part_a/g1.obj"));
    sections[4].parts.push_back(std::make_unique<Model>("../Models/ppsection4/part_a/g2.obj"));
    sections[4].parts.push_back(std::make_unique<Model>("../Models/ppsection4/part_b/g0.obj"));
    sections[4].parts.push_back(std::make_unique<Model>("../Models/ppsection4/part_b/g1.obj"));

    // Section 5 Part A
	for (int i = 0; i <= 35; ++i) { 
        sections[5].parts.push_back(std::make_unique<Model>("../Models/ppsection5/part_a/g" + std::to_string(i) + ".obj"));
    }
    // Section 5 Part B
	for (int i = 0; i <= 1; ++i) {
        sections[5].parts.push_back(std::make_unique<Model>("../Models/ppsection5/part_b/g" + std::to_string(i) + ".obj"));
    }
	// Section 5 Part C
	for (int i = 0; i <= 1; ++i) { 
        sections[5].parts.push_back(std::make_unique<Model>("../Models/ppsection5/part_c/g" + std::to_string(i) + ".obj"));
    }
    // Section 6 Part A
	for (int i = 0; i <= 12; ++i) { 
        sections[6].parts.push_back(std::make_unique<Model>("../Models/ppsection6/part_a/g" + std::to_string(i) + ".obj"));
    }
    // Section 6 Part B
	for (int i = 0; i <= 22; ++i) { 
        sections[6].parts.push_back(std::make_unique<Model>("../Models/ppsection6/part_b/g" + std::to_string(i) + ".obj"));
    }
    buildBVH();
    buildBottomUpBVH();
    debugPrintBVHStructure(bottomUpBVHRootAABB.get());
}

void Assignment2::init()
{
    light.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    light.ambient = glm::vec3(0.3f, 0.3f, 0.3f);  
    light.diffuse = glm::vec3(0.7f, 0.7f, 0.7f);  
    light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	std::cout << "Assignment 2 initialized" << std::endl;
    glEnable(GL_DEPTH_TEST);
}

void Assignment2::update(double dt)
{
	//std::cout << "Assignment 2 updated" << std::endl;
	
    glViewport(0, 0, GLHelper::width, GLHelper::height);
    float aspectRatio = (float)GLHelper::width / GLHelper::height;
    camera.setAspectRatio(aspectRatio);

    // Camera movement
    float cameraSpeed = 10.5f * static_cast<float>(dt);
    glm::vec3 cameraRight = glm::normalize(glm::cross(GLHelper::cameraFront, GLHelper::cameraUp));
    glm::vec3 cameraForward = glm::normalize(glm::cross(GLHelper::cameraUp, cameraRight));

    if (RE_input::is_key_pressed(RE_W)) {
        camera.setPosition(camera.getPosition() + cameraSpeed * cameraForward);
    }
    if (RE_input::is_key_pressed(RE_S)) {
        camera.setPosition(camera.getPosition() - cameraSpeed * cameraForward);
    }
    if (RE_input::is_key_pressed(RE_A)) {
        camera.setPosition(camera.getPosition() - cameraSpeed * cameraRight);
    }
    if (RE_input::is_key_pressed(RE_D)) {
        camera.setPosition(camera.getPosition() + cameraSpeed * cameraRight);
    }
    if (RE_input::is_key_pressed(RE_SPACE)) {
        camera.setPosition(glm::vec3(camera.getPosition() + m_cameraSpeed * static_cast<float>(dt) * m_cameraUp));
    }
    if (RE_input::is_key_pressed(RE_LEFT_SHIFT)) {
        camera.setPosition(camera.getPosition() - m_cameraSpeed * static_cast<float>(dt) * m_cameraUp);
    }

    // Light movement
    float lightSpeed = 1.0f * static_cast<float>(dt);
    if (RE_input::is_key_pressed(RE_I)) {
        moveLight(0.0f, lightSpeed, 0.0f);  // Move light up
    }
    if (RE_input::is_key_pressed(RE_K)) {
        moveLight(0.0f, -lightSpeed, 0.0f);  // Move light down
    }
    if (RE_input::is_key_pressed(RE_J)) {
        moveLight(-lightSpeed, 0.0f, 0.0f);  // Move light left
    }
    if (RE_input::is_key_pressed(RE_L)) {
        moveLight(lightSpeed, 0.0f, 0.0f);  // Move light right
    }
    if (RE_input::is_key_pressed(RE_U)) {
        moveLight(0.0f, 0.0f, -lightSpeed);  // Move light forward
    }
    if (RE_input::is_key_pressed(RE_O)) {
        moveLight(0.0f, 0.0f, lightSpeed);  // Move light backward
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Model Controls");
    // Section selection
    if (ImGui::BeginCombo("Select Section", ("Section " + std::to_string(currentSection)).c_str())) {
        for (const auto& [sectionNumber, section] : sections) {
            bool isSelected = (currentSection == sectionNumber);
            if (ImGui::Selectable(("Section " + std::to_string(sectionNumber)).c_str(), isSelected)) {
                currentSection = sectionNumber;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
	ImGui::Separator();
    // Model selection within the current section
    static int selectedModelIndex = 0;
    if (ImGui::BeginCombo("Select Model", ("Model " + std::to_string(selectedModelIndex + 1)).c_str())) {
        for (size_t i = 0; i < sections[currentSection].parts.size(); ++i) {
            bool isSelected = (selectedModelIndex == i);
            if (ImGui::Selectable(("Model " + std::to_string(i + 1)).c_str(), isSelected)) {
                selectedModelIndex = i;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    // Selected model controls
    if (selectedModelIndex >= 0 && selectedModelIndex < sections[currentSection].parts.size()) {
        auto& model = sections[currentSection].parts[selectedModelIndex];

        if (ImGui::TreeNode("Transform")) {
            float currentScale = model->getScale();
            if (ImGui::SliderFloat("Scale", &currentScale, 0.1f, 10.0f)) {
                model->setScale(currentScale);
            }

            glm::vec3 currentTranslation = model->getTranslation();
            if (ImGui::SliderFloat3("Translation", &currentTranslation[0], -60000.0f, 60000.0f)) {
                model->setTranslation(currentTranslation);
            }

            glm::vec3 currentRotation = model->getRotation();
            if (ImGui::SliderFloat3("Rotation", &currentRotation[0], 0.0f, 360.0f)) {
                model->setRotation(currentRotation);
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Bounding Volumes")) {
            bool showRitter = model->getShowRitterSphere();
            bool showLarson = model->getShowLarsonSphere();
            bool showEigen = model->getShowEigenSphere();
            bool showAABB = model->getBoundingVolumeType() == Model::BVType::AABB;
            bool showOBB = model->getBoundingVolumeType() == Model::BVType::OBB;
            bool showEllipse = model->getBoundingVolumeType() == Model::BVType::Ellipse;

            if (ImGui::Checkbox("Show Ritter Sphere", &showRitter)) {
                model->setShowRitterSphere(showRitter);
                model->recalculateBoundingVolume();
            }
            ImGui::SameLine();
            ImGui::ColorButton("##RitterColor", ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(20, 20));

           if (ImGui::Checkbox("Show Larson Sphere", &showLarson)) {
               model->setShowLarsonSphere(showLarson);
               model->recalculateBoundingVolume();
           }
           ImGui::SameLine();
           ImGui::ColorButton("##LarsonColor", ImVec4(0.0f, 1.0f, 0.0f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(20, 20));

           if (ImGui::Checkbox("Show Eigen Sphere", &showEigen)) {
               model->setShowEigenSphere(showEigen);
               model->recalculateBoundingVolume();
           }   
           ImGui::SameLine();
           ImGui::ColorButton("##EigenColor", ImVec4(0.0f, 0.0f, 1.0f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(20, 20));


           //  Radio buttons for AABB, OBB, and Sphere
           int bvType = static_cast<int>(model->getBoundingVolumeType());
           if (ImGui::RadioButton("AABB", &bvType, static_cast<int>(Model::BVType::AABB))) {
               model->setBoundingVolumeType(Model::BVType::AABB);
               model->recalculateBoundingVolume();
           }
           ImGui::SameLine();
           if (ImGui::RadioButton("OBB", &bvType, static_cast<int>(Model::BVType::OBB))) {
               model->setBoundingVolumeType(Model::BVType::OBB);
               model->recalculateBoundingVolume();
           }
           ImGui::SameLine();
   		    if (ImGui::RadioButton("Ellipse Sphere", &bvType, static_cast<int>(Model::BVType::Ellipse))) {
   			    model->setBoundingVolumeType(Model::BVType::Ellipse);
                model->recalculateBoundingVolume();
   		    }
           ImGui::SameLine();
           if (ImGui::RadioButton("Sphere", &bvType, static_cast<int>(Model::BVType::Sphere))) {
               model->setBoundingVolumeType(Model::BVType::Sphere);
               model->recalculateBoundingVolume();
           }
           ImGui::SameLine();
           if (ImGui::RadioButton("None", &bvType, static_cast<int>(Model::BVType::None))) {
               model->setBoundingVolumeType(Model::BVType::None);
               model->recalculateBoundingVolume();
           }

          

           static const char* eposOptions[] = { "EPOS3", "EPOS4", "EPOS6", "EPOS12" };
           static int currentEPOSIndex = static_cast<int>(model->getCurrentEPOS());
           if (ImGui::Combo("Select EPOS", &currentEPOSIndex, eposOptions, IM_ARRAYSIZE(eposOptions))) {
               model->setEPOS(static_cast<Model::EPOS>(currentEPOSIndex));
               model->recalculateBoundingVolume();
           }
            ImGui::TreePop();
        }

        if (ImGui::Button("Recalculate Bounding Volume")) {
            model->recalculateBoundingVolume();
        }
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please Click On The Recalculate Bounding Volume Button If ");
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "The Bounding Volume Is Not Encapsulating The Model");
    }

    ImGui::Separator();
    // Global controls
    if (ImGui::TreeNode("Global Bounding Volume Controls")) {
        static int currentBVType = static_cast<int>(Model::BVType::Sphere);
        const char* bvTypeLabels[] = { "None", "Sphere", "AABB", "OBB", "Ellipse"};

        if (ImGui::Combo("Global Bounding Volume Type", &currentBVType, bvTypeLabels, IM_ARRAYSIZE(bvTypeLabels))) {
            for (auto& [sectionNumber, section] : sections) {
                for (auto& model : section.parts) {
                    model->setBoundingVolumeType(static_cast<Model::BVType>(currentBVType));
                    model->recalculateBoundingVolume();
                }
            }
        }

        // Sphere-specific controls
        if (currentBVType == static_cast<int>(Model::BVType::Sphere)) {
            static bool showRitter = false;
            static bool showLarson = false;
            static bool showEigen = false;

            if (ImGui::Checkbox("Show Ritter Spheres", &showRitter)) {
                for (auto& [sectionNumber, section] : sections) {
                    for (auto& model : section.parts) {
                        model->setShowRitterSphere(showRitter);
                    }
                }
            }
            ImGui::SameLine();
            ImGui::ColorButton("##RitterColor", ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(20, 20));


            if (ImGui::Checkbox("Show Larson Spheres", &showLarson)) {
                for (auto& [sectionNumber, section] : sections) {
                    for (auto& model : section.parts) {
                        model->setShowLarsonSphere(showLarson);
                    }
                }
            }
            ImGui::SameLine();
            ImGui::ColorButton("##LarsonColor", ImVec4(0.0f, 1.0f, 0.0f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(20, 20));

            if (ImGui::Checkbox("Show Eigen Spheres", &showEigen)) {
                for (auto& [sectionNumber, section] : sections) {
                    for (auto& model : section.parts) {
                        model->setShowEigenSphere(showEigen);
                    }
                }
            }
            ImGui::SameLine();
            ImGui::ColorButton("##EigenColor", ImVec4(0.0f, 0.0f, 1.0f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(20, 20));

            if (ImGui::Button("Show All Sphere Types")) {
                showRitter = showLarson = showEigen = true;
                for (auto& [sectionNumber, section] : sections) {
                    for (auto& model : section.parts) {
                        model->setShowLarsonSphere(showLarson);
						model->setShowRitterSphere(showRitter);
                        model->setShowEigenSphere(showRitter);
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Hide All Sphere Types")) {
                showRitter = showLarson = showEigen = false;
                for (auto& [sectionNumber, section] : sections) {
                    for (auto& model : section.parts) {
                        model->setShowLarsonSphere(false);
                        model->setShowRitterSphere(false);
                        model->setShowEigenSphere(false);
                    }
                }
            }
        }

        if (ImGui::Button("Recalculate All Bounding Volumes")) {
            for (auto& [sectionNumber, section] : sections) {
                for (auto& model : section.parts) {
                    model->recalculateBoundingVolume();
                }
            }
        }
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please Click On The Recalculate Bounding Volume Button If");
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "The Bounding Volume Is Not Encapsulating The Model");

        ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::TreeNode("BVH Visualization")) {
        if (ImGui::Button("Recalculate BVHs"))
        {
            for (auto& [sectionNumber, section] : sections) {
                for (auto& model : section.parts) {
                    model->recalculateBoundingVolume();
                    model->updateBoundingVoluesBuffers();
                }
            }
            updateBVH();
            buildBVH();
        }
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please Click On The Recalculate Bounding Volume Button If");
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "The Bounding Volume Is Not Encapsulating The Model");

        ImGui::Checkbox("Show Top Down AABB BVH", &showTopDownAABB);
        ImGui::Checkbox("Show Top Down Sphere BVH", &showTopDownSphere);
        ImGui::Checkbox("Show Bottom Up AABB BVH", &showBottomUpAABB);
        ImGui::Checkbox("Show Bottom Up Sphere BVH", &showBottomUpSphere);

		MAX_TREE_DEPTH = (showTopDownAABB || showTopDownSphere) == true ? 7 : 30;
		ImGui::SliderInt("BVH Depth", &currentBVHDepth, 0, MAX_TREE_DEPTH); 
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Top Down BVH: Max Height of Tree = 7");

      

        ImGui::TreePop();
    }
    ImGui::End();

    for (auto& [sectionNumber, section] : sections) {
        for (auto& model : section.parts) {
            model->updateBoundingVoluesBuffers();
        }
    }
    draw();
 //   ImGui_ImplOpenGL3_NewFrame();
 //   ImGui_ImplGlfw_NewFrame();
 //   ImGui::NewFrame();


 //   // Model selection
 //   static int selectedModelIndex = 0;
 //   if (ImGui::BeginCombo("Select Model", ("Model " + std::to_string(selectedModelIndex + 1)).c_str())) {
 //       for (size_t i = 0; i < models.size(); ++i) {
 //           bool isSelected = (selectedModelIndex == i);
 //           if (ImGui::Selectable(("Model " + std::to_string(i + 1)).c_str(), isSelected)) {
 //               selectedModelIndex = i;
 //           }
 //           if (isSelected) {
 //               ImGui::SetItemDefaultFocus();
 //           }
 //       }
 //       ImGui::EndCombo();
 //   }

 //   // Selected model controls
 //   if (selectedModelIndex >= 0 && selectedModelIndex < models.size()) {
 //       auto& model = models[selectedModelIndex];

 //       if (ImGui::TreeNode("Transform")) {
 //           float currentScale = model->getScale();
 //           if (ImGui::SliderFloat("Scale", &currentScale, 0.1f, 10.0f)) {
 //               model->setScale(currentScale);
 //           }

 //           glm::vec3 currentTranslation = model->getTranslation();
 //           if (ImGui::SliderFloat3("Translation", &currentTranslation[0], -60000.0f, 60000.0f)) {
 //               model->setTranslation(currentTranslation);
 //           }

 //           glm::vec3 currentRotation = model->getRotation();
 //           if (ImGui::SliderFloat3("Rotation", &currentRotation[0], 0.0f, 360.0f)) {
 //               model->setRotation(currentRotation);
 //           }
 //           ImGui::TreePop();
 //       }

 //       if (ImGui::TreeNode("Bounding Volumes")) {
 //           bool showRitter = model->getShowRitterSphere();
 //           bool showLarson = model->getShowLarsonSphere();
 //           bool showEigen = model->getShowEigenSphere();
 //           bool showAABB = model->getBoundingVolumeType() == Model::BVType::AABB;
 //           bool showOBB = model->getBoundingVolumeType() == Model::BVType::OBB;
	//		bool showEllipse = model->getBoundingVolumeType() == Model::BVType::Ellipse;

 //           if (ImGui::Checkbox("Show Ritter Sphere", &showRitter)) {
 //               model->setShowRitterSphere(showRitter);
 //               model->recalculateBoundingVolume();
 //           }   
 //           ImGui::SameLine();
 //           ImGui::ColorButton("##RitterColor", ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(20, 20));

 //           if (ImGui::Checkbox("Show Larson Sphere", &showLarson)) {
 //               model->setShowLarsonSphere(showLarson);
 //               model->recalculateBoundingVolume();
 //           }
 //           ImGui::SameLine();
 //           ImGui::ColorButton("##LarsonColor", ImVec4(0.0f, 1.0f, 0.0f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(20, 20));

 //           if (ImGui::Checkbox("Show Eigen Sphere", &showEigen)) {
 //               model->setShowEigenSphere(showEigen);
 //               model->recalculateBoundingVolume();
 //           }   
 //           ImGui::SameLine();
 //           ImGui::ColorButton("##EigenColor", ImVec4(0.0f, 0.0f, 1.0f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(20, 20));


 //           //  Radio buttons for AABB, OBB, and Sphere
 //           int bvType = static_cast<int>(model->getBoundingVolumeType());
 //           if (ImGui::RadioButton("AABB", &bvType, static_cast<int>(Model::BVType::AABB))) {
 //               model->setBoundingVolumeType(Model::BVType::AABB);
 //               model->recalculateBoundingVolume();
 //           }
 //           ImGui::SameLine();
 //           if (ImGui::RadioButton("OBB", &bvType, static_cast<int>(Model::BVType::OBB))) {
 //               model->setBoundingVolumeType(Model::BVType::OBB);
 //               model->recalculateBoundingVolume();
 //           }
 //           ImGui::SameLine();
	//		if (ImGui::RadioButton("Ellipse Sphere", &bvType, static_cast<int>(Model::BVType::Ellipse))) {
	//			model->setBoundingVolumeType(Model::BVType::Ellipse);
 //               model->recalculateBoundingVolume();
	//		}
 //           ImGui::SameLine();
 //           if (ImGui::RadioButton("Sphere", &bvType, static_cast<int>(Model::BVType::Sphere))) {
 //               model->setBoundingVolumeType(Model::BVType::Sphere);
 //               model->recalculateBoundingVolume();
 //           }

 //           static const char* eposOptions[] = { "EPOS3", "EPOS4", "EPOS6", "EPOS12" };
 //           static int currentEPOSIndex = static_cast<int>(model->getCurrentEPOS());
 //           if (ImGui::Combo("Select EPOS", &currentEPOSIndex, eposOptions, IM_ARRAYSIZE(eposOptions))) {
 //               model->setEPOS(static_cast<Model::EPOS>(currentEPOSIndex));
 //               model->recalculateBoundingVolume();
 //           }

 //           ImGui::TreePop();
 //       }

 //       if (ImGui::Button("Recalculate Bounding Volume")) {
 //           model->recalculateBoundingVolume();
 //       }
 //   }


 //   // Global controls
 //   if (ImGui::TreeNode("Global Bounding Volume Controls")) {
 //       static int currentBVType = static_cast<int>(Model::BVType::Sphere);
 //       const char* bvTypeLabels[] = { "Sphere", "AABB", "OBB", "Ellipse" };

 //       if (ImGui::Combo("Global Bounding Volume Type", &currentBVType, bvTypeLabels, IM_ARRAYSIZE(bvTypeLabels))) {
 //           for (auto& model : models) {
 //               model->setBoundingVolumeType(static_cast<Model::BVType>(currentBVType));
 //               model->recalculateBoundingVolume();
 //           }
 //       }

 //       // Sphere-specific controls
 //       if (currentBVType == static_cast<int>(Model::BVType::Sphere)) {
 //           static bool showRitter = false;
 //           static bool showLarson = false;
 //           static bool showEigen = false;

 //           if (ImGui::Checkbox("Show Ritter Spheres", &showRitter)) {
 //               for (auto& model : models) {
 //                   model->setShowRitterSphere(showRitter);
 //               }
 //           }         
 //           ImGui::SameLine();
 //           ImGui::ColorButton("##RitterColor", ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(20, 20));


 //           if (ImGui::Checkbox("Show Larson Spheres", &showLarson)) {
 //               for (auto& model : models) {
 //                   model->setShowLarsonSphere(showLarson);
 //               }
 //           }
 //           ImGui::SameLine();
 //           ImGui::ColorButton("##LarsonColor", ImVec4(0.0f, 1.0f, 0.0f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(20, 20));

 //           if (ImGui::Checkbox("Show Eigen Spheres", &showEigen)) {
 //               for (auto& model : models) {
 //                   model->setShowEigenSphere(showEigen);
 //               }
 //           }
 //           ImGui::SameLine();
 //           ImGui::ColorButton("##EigenColor", ImVec4(0.0f, 0.0f, 1.0f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(20, 20));

 //           if (ImGui::Button("Show All Sphere Types")) {
 //               showRitter = showLarson = showEigen = true;
 //               for (auto& model : models) {
 //                   model->setShowRitterSphere(true);
 //                   model->setShowLarsonSphere(true);
 //                   model->setShowEigenSphere(true);
 //               }
 //           }
 //           ImGui::SameLine();
 //           if (ImGui::Button("Hide All Sphere Types")) {
 //               showRitter = showLarson = showEigen = false;
 //               for (auto& model : models) {
 //                   model->setShowRitterSphere(false);
 //                   model->setShowLarsonSphere(false);
 //                   model->setShowEigenSphere(false);
 //               }
 //           }
 //       }

 //       if (ImGui::Button("Recalculate All Bounding Volumes")) {
 //           for (auto& model : models) {
 //               model->recalculateBoundingVolume();
 //           }
 //       }

 //       ImGui::TreePop();
 //   }


 //   if (ImGui::TreeNode("BVH Visualization")) {
 //       ImGui::Checkbox("Show Top Down AABB BVH", &showTopDownAABB);
 //       ImGui::Checkbox("Show Top Down Sphere BVH", &showTopDownSphere);
	//	ImGui::Checkbox("Show Bottom Up AABB BVH", &showBottomUpAABB);
 //       ImGui::Checkbox("Show Bottom Up Sphere BVH", &showBottomUpSphere);
 //       ImGui::SliderInt("BVH Depth", &currentBVHDepth, 0, MAX_TREE_DEPTH);

 //       if (ImGui::Button("Recalculate BVHs"))
 //       {
 //           for (auto& model : getModels())
 //           {
 //               model->recalculateBoundingVolume();
 //               model->updateBoundingVoluesBuffers();
 //               updateBVH();

 //           }
 //           buildBVH();
 //       }


 //       ImGui::TreePop();
 //   }
 //   ImGui::End();

 //   for (auto& model : getModels())
	//{

	//	model->updateBoundingVoluesBuffers();
	//}
 //  

 //   if (RE_input::is_key_pressed(RE_0))
 //   {
	//	//debugVisualizeBVH();
 //   }
	//draw();
}
//void printShaderUniforms(GLSLShader& shader) {
//    shader.Use();
//
//    GLint numUniforms;
//    glGetProgramiv(shader.GetHandle(), GL_ACTIVE_UNIFORMS, &numUniforms);
//
//    for (GLint i = 0; i < numUniforms; i++) {
//        GLchar uniformName[256];
//        GLint uniformSize;
//        GLenum uniformType;
//        glGetActiveUniform(shader.GetHandle(), i, sizeof(uniformName), NULL, &uniformSize, &uniformType, uniformName);
//
//        GLint location = glGetUniformLocation(shader.GetHandle(), uniformName);
//
//        std::cout << "Uniform " << i << ": " << uniformName << " (location: " << location << ")" << std::endl;
//
//        // Print the value of the uniform
//        switch (uniformType) {
//        case GL_FLOAT:
//        {
//            float value;
//            glGetUniformfv(shader.GetHandle(), location, &value);
//            std::cout << "  Value: " << value << std::endl;
//            break;
//        }
//        case GL_FLOAT_VEC3:
//        {
//            float value[3];
//            glGetUniformfv(shader.GetHandle(), location, value);
//            std::cout << "  Value: " << value[0] << ", " << value[1] << ", " << value[2] << std::endl;
//            break;
//        }
//        case GL_FLOAT_MAT4:
//        {
//            float value[16];
//            glGetUniformfv(shader.GetHandle(), location, value);
//            std::cout << "  Value: " << std::endl;
//            for (int row = 0; row < 4; row++) {
//                std::cout << "    ";
//                for (int col = 0; col < 4; col++) {
//                    std::cout << value[row * 4 + col] << " ";
//                }
//                std::cout << std::endl;
//            }
//            break;
//        }
//        // Add more cases for other uniform types as needed
//        }
//    }
//
//    shader.UnUse();
//}
//void checkOpenGLState() {
//    GLint polygonMode[2];
//    glGetIntegerv(GL_POLYGON_MODE, polygonMode);
//    std::cout << "Polygon mode: " << polygonMode[0] << std::endl;
//
//    GLboolean depthTest;
//    glGetBooleanv(GL_DEPTH_TEST, &depthTest);
//    std::cout << "Depth test enabled: " << (depthTest ? "true" : "false") << std::endl;
//
//    GLint viewport[4];
//    glGetIntegerv(GL_VIEWPORT, viewport);
//    std::cout << "Viewport: " << viewport[0] << ", " << viewport[1] << ", " << viewport[2] << ", " << viewport[3] << std::endl;
//
//    GLfloat clearColor[4];
//    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
//    std::cout << "Clear color: " << clearColor[0] << ", " << clearColor[1] << ", " << clearColor[2] << ", " << clearColor[3] << std::endl;
//
//    checkGLError("After checking OpenGL state");
//}
//void checkFramebufferStatus() {
//    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//    if (status != GL_FRAMEBUFFER_COMPLETE) {
//        std::cout << "Framebuffer is not complete! Status: " << status << std::endl;
//        switch (status) {
//        case GL_FRAMEBUFFER_UNDEFINED: std::cout << "GL_FRAMEBUFFER_UNDEFINED" << std::endl; break;
//        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl; break;
//        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl; break;
//        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << std::endl; break;
//        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: std::cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << std::endl; break;
//        case GL_FRAMEBUFFER_UNSUPPORTED: std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl; break;
//        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << std::endl; break;
//        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: std::cout << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" << std::endl; break;
//        }
//    }
//    else {
//        std::cout << "Framebuffer is complete." << std::endl;
//    }
//}
//bool isMatrixValid(const glm::mat4& matrix) {
//    for (int i = 0; i < 4; ++i) {
//        for (int j = 0; j < 4; ++j) {
//            if (std::isnan(matrix[i][j]) || std::isinf(matrix[i][j])) {
//                return false;
//            }
//        }
//    }
//    return true;
//}
//bool isProjectionMatrixValid(const glm::mat4& matrix)
//{
//    bool isValid = true;
//
//    // Check if any value is NaN or Inf
//    for (int i = 0; i < 4; ++i) {
//        for (int j = 0; j < 4; ++j) {
//            if (std::isnan(matrix[i][j]) || std::isinf(matrix[i][j])) {
//                std::cerr << "Invalid value at [" << i << "][" << j << "]: " << matrix[i][j] << std::endl;
//                isValid = false;
//            }
//        }
//    }
//
//    // Check if the scale factors are within a reasonable range
//    if (std::abs(matrix[0][0]) < 0.1f || std::abs(matrix[0][0]) > 100.0f) {
//        std::cerr << "Invalid x scale factor: " << matrix[0][0] << std::endl;
//        isValid = false;
//    }
//    if (std::abs(matrix[1][1]) < 0.1f || std::abs(matrix[1][1]) > 100.0f) {
//        std::cerr << "Invalid y scale factor: " << matrix[1][1] << std::endl;
//        isValid = false;
//    }
//
//    // Check if the projection type is as expected (perspective in this case)
//    if (matrix[3][3] != 0.0f) {
//        std::cerr << "Unexpected value at [3][3]: " << matrix[3][3] << " (should be 0)" << std::endl;
//        isValid = false;
//    }
//    if (matrix[2][3] != -1.0f) {
//        std::cerr << "Unexpected value at [2][3]: " << matrix[2][3] << " (should be -1)" << std::endl;
//        isValid = false;
//    }
//
//    return isValid;
//}
//glm::mat4 calculateProjectionMatrix(float fovY, float aspect, float near, float far)
//{
//    float tanHalfFovy = tan(fovY / 2.0f);
//    glm::mat4 proj = glm::mat4(0.0f);
//    proj[0][0] = 1.0f / (aspect * tanHalfFovy);
//    proj[1][1] = 1.0f / (tanHalfFovy);
//    proj[2][2] = -(far + near) / (far - near);
//    proj[2][3] = -1.0f;
//    proj[3][2] = -(2.0f * far * near) / (far - near);
//    return proj;
//}
void Assignment2::draw()
{
	//checkFramebufferStatus();
	//checkOpenGLState();
    if (glfwGetCurrentContext() == nullptr) {
        std::cerr << "OpenGL context lost!" << std::endl;
        return;
    }

    shdr_pgm.Use();


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, GLHelper::width, GLHelper::height);
    glClearColor(1.0f, 1.0f, 1.f, 1.0f);

    // Set up MVP matrix
    float aspectRatio = static_cast<float>(GLHelper::width) / static_cast<float>(GLHelper::height);
    camera.setAspectRatio(aspectRatio);

    camera.setAspectRatio(aspectRatio);
    camera.setFOV(glm::radians(45.0f));
    camera.setNearPlane(0.1f);
    camera.setFarPlane(10000.0f);

    // Update camera position and target
    glm::vec3 currentPosition = camera.getPosition();
    glm::vec3 newTarget = currentPosition + GLHelper::cameraFront;
    camera.setTarget(newTarget);
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    glm::mat4 viewMatrix = camera.getViewMatrix();


    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0001f));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, -10.0f));

    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix; 

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    glm::vec3 cameraPos = camera.getPosition();
    glm::vec3 cameraTarget = camera.getTarget();


    GLuint mvpLoc = glGetUniformLocation(shdr_pgm.GetHandle(), "mvpMatrix");
    checkGLError("Setting mvpMatrix uniform (glUniformMatrix4fv)");

    //shdr_pgm.SetUniform("mvpMatrix", mvpMatrix);
    //checkGLError("Setting mvpMatrix uniform");


    ////GLuint colorLoc = glGetUniformLocation(shdr_pgm.GetHandle(), "uColor");
    GLint location = glGetUniformLocation(shdr_pgm.GetHandle(), "normalMatrix");
    if (location != -1) {
        shdr_pgm.SetUniform("normalMatrix", normalMatrix);
    }
    else {
        std::cout << "Warning: normalMatrix uniform not found in shader program." << std::endl;
    }

    // Set material properties
    shdr_pgm.SetUniform("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
    shdr_pgm.SetUniform("material.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    shdr_pgm.SetUniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    shdr_pgm.SetUniform("material.shininess", 32.0f);
    // Set light properties
    shdr_pgm.SetUniform("light.direction", light.direction);
    shdr_pgm.SetUniform("light.ambient", light.ambient);
    shdr_pgm.SetUniform("light.diffuse", light.diffuse);
    shdr_pgm.SetUniform("light.specular", light.specular);
    shdr_pgm.UnUse();
    glLineWidth(5.0f);
  
    if (sections.find(currentSection) != sections.end()) {
        for (const auto& model : sections[currentSection].parts) {
            model->draw(shdr_pgm, bounding_sphere_shader, camera, modelMatrix);
        }
    }

    checkGLError("Shader unuse in Assignment2::draw");

    //checkGLError("After model draw in Assignment2::draw");

    drawBVH(bounding_sphere_shader, modelMatrix);
  /*  drawBottomUpBVH<AABB>(bounding_sphere_shader, modelMatrix);
    drawBottomUpBVH<Sphere>(bounding_sphere_shader, modelMatrix);*/


	checkGLError("After drawBVH in Assignment2::draw");
    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Assignment2::cleanup()
{
	std::cout << "Assignment 2 cleaned up" << std::endl;
}


// BVH TREES stufF
// Compute the AABB bounding volume over a set of models
void Assignment2::buildBVH() {
    bvhRoot = buildBVHTemplate<AABB>();
    topDownBVHSpere = buildBVHTemplate<Sphere>();
}
void Assignment2::updateBVH() {
    buildBVH();
    buildBottomUpBVH();
    updateBottomUpBVH<AABB>();
    updateBottomUpBVH<Sphere>();
}

//template<typename BoundingVolumeType>
//std::unique_ptr<BVHNode<BoundingVolumeType>> Assignment2::buildBVHTemplate() {
//    std::vector<Model*> allModels;
//    for (auto& model : models) {
//        allModels.push_back(model.get());
//    }
//    return
//        buildTopDownBVH<BoundingVolumeType>(allModels, 0);
//}
template<typename BoundingVolumeType>
std::unique_ptr<BVHNode<BoundingVolumeType>> Assignment2::buildBVHTemplate() {
    std::vector<Model*> currentSectionModels;
    if (sections.find(currentSection) != sections.end()) {
        for (const auto& model : sections[currentSection].parts) {
            currentSectionModels.push_back(model.get());
        }
    }
    return buildTopDownBVH<BoundingVolumeType>(currentSectionModels, 0);
}

template<typename BoundingVolumeType>
void Assignment2::updateBVHTemplate() {
    if (bvhRoot) {
        updateBVHNodeLogic<BoundingVolumeType>(bvhRoot.get());
        updateBVHNodeBuffers<BoundingVolumeType>(bvhRoot.get());
    }
    if (topDownBVHSpere)
    {
        updateBVHNodeLogic<BoundingVolumeType>(topDownBVHSpere.get());
        updateBVHNodeBuffers<BoundingVolumeType>(topDownBVHSpere.get());
    }
}

template<typename BoundingVolumeType>
void Assignment2::drawBVHTemplate(GLSLShader& shader, glm::mat4 mvpMatrix) {
    if constexpr (std::is_same_v<BoundingVolumeType, AABB>) {
        if (bvhRoot && showTopDownAABB) {
            drawBVHNode<AABB>(bvhRoot.get(), shader, mvpMatrix, 0);
        }
        if (bottomUpBVHRootAABB && showBottomUpAABB)
        {
			drawBVHNode<AABB>(bottomUpBVHRootAABB.get(), shader, mvpMatrix, 0);

        }
    }
    else if constexpr (std::is_same_v<BoundingVolumeType, Sphere>) {
        if (topDownBVHSpere && showTopDownSphere) {
            drawBVHNode<Sphere>(topDownBVHSpere.get(), shader, mvpMatrix, 0);
        }
        if (bottomUpBVHRootSphere && showBottomUpSphere)
        {
			drawBVHNode<Sphere>(bottomUpBVHRootSphere.get(), shader, mvpMatrix, 0);

        }
    }
}

template<typename BoundingVolumeType>
std::unique_ptr<BVHNode<BoundingVolumeType>> Assignment2::buildTopDownBVH(std::vector<Model*>& models, int depth) {
    auto node = std::make_unique<BVHNode<BoundingVolumeType>>();
    node->boundingVolume = std::make_unique<BoundingVolumeType>();
    *node->boundingVolume = computeBoundingVolume<BoundingVolumeType>(models);
    node->model = models;

    if (models.size() <= MAX_OBJECTS_PER_LEAF || depth >= MAX_TREE_DEPTH) {
        node->type = BVHNode<BoundingVolumeType>::NodeType::LEAF;
        return node;
    }

    node->type = BVHNode<BoundingVolumeType>::NodeType::INTERNAL;
    int splitIndex = 0;
    partitionObjects<BoundingVolumeType>(models, splitIndex);

    std::vector<Model*> leftModels(models.begin(), models.begin() + splitIndex);
    std::vector<Model*> rightModels(models.begin() + splitIndex, models.end());

   // std::cout << "Splitting node at depth " << depth << ": " << leftModels.size() << " left, " << rightModels.size() << " right" << std::endl;

    node->leftChild = buildTopDownBVH<BoundingVolumeType>(leftModels, depth + 1);
    node->rightChild = buildTopDownBVH<BoundingVolumeType>(rightModels, depth + 1);

    return node;
}


template<typename BoundingVolumeType>
void Assignment2::partitionObjects(std::vector<Model*>& models, int& splitIndex) {
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(std::numeric_limits<float>::lowest());
    for (const auto& model : models) {
        glm::vec3 center;
        glm::vec3 extents;
        if constexpr (std::is_same_v<BoundingVolumeType, AABB>) {
            AABB modelAABB = model->getAABB();
            center = modelAABB.m_Center;
            extents = modelAABB.m_HalfExtents;
        }
        else if constexpr (std::is_same_v<BoundingVolumeType, Sphere>) {
            Sphere modelSphere = model->getRitterSphere();
            center = modelSphere.m_Center;
            extents = glm::vec3(modelSphere.m_Radius);
        }
        glm::mat4 modelMatrix = model->getModelMatrix();
        glm::vec3 transformedCenter = glm::vec3(modelMatrix * glm::vec4(center, 1.0f));
        min = glm::min(min, transformedCenter - extents);
        max = glm::max(max, transformedCenter + extents);
    }
    glm::vec3 spread = max - min;
    int axis = 0;
    if (spread.y > spread.x) axis = 1;
    if (spread.z > spread[axis]) axis = 2;

    std::sort(models.begin(), models.end(), [this, axis](Model* a, Model* b) {
        glm::vec3 centroidA = computeModelCentroid<BoundingVolumeType>(*a);
        glm::vec3 centroidB = computeModelCentroid<BoundingVolumeType>(*b);
        return centroidA[axis] < centroidB[axis];
        });

    splitIndex = models.size() / 2;
}

template<typename BoundingVolumeType>
BoundingVolumeType Assignment2::computeBoundingVolume(std::vector<Model*>& models) {
    if constexpr (std::is_same_v<BoundingVolumeType, AABB>) {
        glm::vec3 min(std::numeric_limits<float>::max());
        glm::vec3 max(std::numeric_limits<float>::lowest());
        for (const auto& model : models) {
            AABB modelAABB = model->getAABB();
            glm::mat4 modelMatrix = model->getModelMatrix();
            glm::vec3 scaledMin = glm::vec3(modelMatrix * glm::vec4(modelAABB.m_Center - modelAABB.m_HalfExtents, 1.0f));
            glm::vec3 scaledMax = glm::vec3(modelMatrix * glm::vec4(modelAABB.m_Center + modelAABB.m_HalfExtents, 1.0f));
            min = glm::min(min, scaledMin);
            max = glm::max(max, scaledMax);
        }
        glm::vec3 center = (min + max) * 0.5f;
        glm::vec3 halfExtents = (max - min) * 0.5f;
        return AABB(center, halfExtents);
    }
    if constexpr (std::is_same_v<BoundingVolumeType, Sphere>) {
        glm::vec3 min(std::numeric_limits<float>::max());
        glm::vec3 max(std::numeric_limits<float>::lowest());

        for (const auto& model : models) {
            Sphere modelSphere = model->getRitterSphere();
            glm::mat4 modelMatrix = model->getModelMatrix();

            glm::vec3 transformedCenter = glm::vec3(glm::vec4(modelSphere.m_Center, 1.0f));
            float transformedRadius = modelSphere.m_Radius;

            min = glm::min(min, transformedCenter - glm::vec3(transformedRadius));
            max = glm::max(max, transformedCenter + glm::vec3(transformedRadius));
        }

        glm::vec3 center = (min + max) * 0.5f;
        float radius = std::max({
                         std::abs(max.x - center.x),
                         std::abs(max.y - center.y),
                         std::abs(max.z - center.z)
            });
        return Sphere(center, radius, 32, 32, glm::vec3(1.0f, 0.0f, 0.0f));
    }
}
template<typename BoundingVolumeType>
glm::vec3 Assignment2::computeModelCentroid(Model& model) {
    glm::vec3 center;
    if constexpr (std::is_same_v<BoundingVolumeType, AABB>) {
        AABB modelAABB = model.getAABB();
        center = modelAABB.m_Center;
    }
    else if constexpr (std::is_same_v<BoundingVolumeType, Sphere>) {
        Sphere modelSphere = model.getRitterSphere();
        center = modelSphere.m_Center;
    }
    glm::mat4 modelMatrix = model.getModelMatrix();
    return glm::vec3(modelMatrix * glm::vec4(center, 1.0f));
}

template<typename BoundingVolumeType>
void Assignment2::updateBVHNodeLogic(BVHNode<BoundingVolumeType>* node) {
    if (!node) return;
    if (node->type == BVHNode<BoundingVolumeType>::NodeType::LEAF) {
        if (!node->model.empty() && node->model[0] != nullptr) {
            node->logicalBoundingVolume = computeBoundingVolume<BoundingVolumeType>(node->model);
        }
    }
    else {
        updateBVHNodeLogic(node->leftChild.get());
        updateBVHNodeLogic(node->rightChild.get());
        if (node->leftChild && node->rightChild) {
            const BoundingVolumeType& leftBV = node->leftChild->logicalBoundingVolume;
            const BoundingVolumeType& rightBV = node->rightChild->logicalBoundingVolume;

            if constexpr (std::is_same_v<BoundingVolumeType, AABB>) {
                glm::vec3 minPoint = glm::min(
                    leftBV.m_Center - leftBV.m_HalfExtents,
                    rightBV.m_Center - rightBV.m_HalfExtents
                );
                glm::vec3 maxPoint = glm::max(
                    leftBV.m_Center + leftBV.m_HalfExtents,
                    rightBV.m_Center + rightBV.m_HalfExtents
                );
                glm::vec3 newCenter = (minPoint + maxPoint) * 0.5f;
                glm::vec3 newHalfExtents = (maxPoint - minPoint) * 0.5f;
                node->logicalBoundingVolume = AABB(newCenter, newHalfExtents);
            }
			if  constexpr (std::is_same_v<BoundingVolumeType, Sphere>)
            {
				glm::vec3 minPoint = glm::min(leftBV.m_Center - leftBV.m_Radius, rightBV.m_Center - rightBV.m_Radius);
				glm::vec3 maxPoint = glm::max(leftBV.m_Center + leftBV.m_Radius, rightBV.m_Center + rightBV.m_Radius);
				glm::vec3 newCenter = (minPoint + maxPoint) * 0.5f;
                float radius = std::max({
                            std::abs(maxPoint.x - newCenter.x),
                            std::abs(maxPoint.y - newCenter.y),
                            std::abs(maxPoint.z - newCenter.z)
                    });
				node->logicalBoundingVolume = Sphere(newCenter, radius);
            }
        }
    }
}
template<typename BoundingVolumeType>
void Assignment2::updateBVHNodeBuffers(BVHNode<BoundingVolumeType>* node) {
    if (!node) return;

    if (!node->boundingVolume) {
        node->boundingVolume = std::make_unique<BoundingVolumeType>();
    }
    *node->boundingVolume = node->logicalBoundingVolume;

    if constexpr (std::is_same_v<BoundingVolumeType, AABB>) {
        node->boundingVolume->updateBuffers(node->boundingVolume->aabbVAO,
            node->boundingVolume->aabbVBO,
            node->boundingVolume->aabbEBO, 0);
    }
    else if constexpr (std::is_same_v<BoundingVolumeType, Sphere>) {
        node->boundingVolume->updateBuffers(node->boundingVolume->sphereVAO,
            node->boundingVolume->sphereVBO,
            node->boundingVolume->sphereEBO, 0);
    }

    checkGLError("After updating node buffers");

    if (node->type == BVHNode<BoundingVolumeType>::NodeType::INTERNAL) {
        updateBVHNodeBuffers(node->leftChild.get());
        updateBVHNodeBuffers(node->rightChild.get());
    }
}
// Function to generate a color based on depth
glm::vec3 generateColor(int depth) {
    // Constants for color cycling
    const float frequency = 0.3f; // Frequency to adjust the rate of color changes

    // Calculate color components based on depth
    float red = sin(frequency * depth + 0) * 0.5f + 0.5f;
    float green = sin(frequency * depth + 2) * 0.5f + 0.5f;
    float blue = sin(frequency * depth + 4) * 0.5f + 0.5f;

    return glm::vec3(red, green, blue);
}

template<typename BoundingVolumeType>
void Assignment2::drawBVHNode(BVHNode<BoundingVolumeType>* node, GLSLShader& shader, glm::mat4 modelMatrix, int depth) {
    if (!node) {
        std::cout << "Null node at depth " << depth << std::endl;
        return;
    }

    if (depth > currentBVHDepth)
        return;
    if (!node->boundingVolume) {
        std::cout << "Null bounding volume at depth " << depth << std::endl;
        return;
    }
    shader.Use();
    glm::mat4 mvpMatrix = camera.getProjectionMatrix() * camera.getViewMatrix() * modelMatrix;
    //shader.SetUniform("mvpMatrix", mvpMatrix);
  // An array of colors for different levels
    const std::vector<glm::vec3> levelColors = {
        glm::vec3(1.0f, 0.0f, 0.0f),  // Red (level 0)
        glm::vec3(1.0f, 0.5f, 0.0f),  // Orange (level 1)
        glm::vec3(1.0f, 1.0f, 0.0f),  // Yellow (level 2)
        glm::vec3(0.0f, 0.0f, 1.0f),  // Blue (level 3)
        glm::vec3(0.0f, 1.0f, 1.0f),  // Cyan (level 4)
        glm::vec3(1.0f, 0.5f, 0.5f),  // Pink (level 5)
        glm::vec3(0.5f, 0.0f, 1.0f),  // Purple (level 6)
        glm::vec3(1.0f, 0.0f, 1.0f),  // Magenta (level 7)
        glm::vec3(0.5f, 0.5f, 0.5f),  // Gray (level 8)
        glm::vec3(0.2f, 0.3f, 0.4f),  // Some Color(level 9)
        glm::vec3(0.6f, 0.3f, 0.1f),  // Brown (level 10)
        glm::vec3(0.0f, 0.5f, 0.5f),  // Teal (level 11)
        glm::vec3(0.5f, 0.5f, 0.0f),  // Olive (level 12)
        glm::vec3(0.5f, 0.0f, 0.0f),  // Maroon (level 13)
        glm::vec3(0.0f, 0.5f, 1.0f),  // Royal Blue (level 14)
        glm::vec3(0.8f, 0.6f, 0.4f)   // Tan (level 15)
    };
    // Choose color based on depth
    glm::vec3 color;
    if (depth < levelColors.size()) {
        color = levelColors[depth];
    }
    else {
        color = generateColor(depth); // Generate color dynamically
    }
    // Leaf nodes remain green
    if (node->type == BVHNode<BoundingVolumeType>::NodeType::LEAF) {
        color = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    shader.SetUniform("color", color);
    shader.SetUniform("sphereType", 4); // 4 for AABB and Sphere in the fragment shader

    glLineWidth(std::max(1.0f, 3.0f - depth * 0.5f));
    try {
        if constexpr (std::is_same_v<BoundingVolumeType, AABB>) {
            if (showTopDownAABB || showBottomUpAABB) {
                shader.SetUniform("mvpMatrix", mvpMatrix);
                shader.SetUniform("color", color);
                shader.SetUniform("sphereType", 4);
                node->boundingVolume->draw(node->boundingVolume->aabbVAO, node->boundingVolume->aabbVBO, node->boundingVolume->aabbEBO, 0);
            }
        }
        if constexpr (std::is_same_v<BoundingVolumeType, Sphere>) {
            if (showTopDownSphere || showBottomUpSphere) {
                shader.SetUniform("mvpMatrix", mvpMatrix);
                shader.SetUniform("color", color);
                shader.SetUniform("sphereType", 4);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                node->boundingVolume->draw(node->boundingVolume->sphereVAO, node->boundingVolume->sphereVBO, node->boundingVolume->sphereEBO, 0);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
    }
    catch (const std::exception& e) {
        std::cout << "Exception caught while drawing node at depth " << depth << ": " << e.what() << std::endl;
    }
    checkGLError("After drawing node");
    if (node->type == BVHNode<BoundingVolumeType>::NodeType::INTERNAL) {
        drawBVHNode(node->leftChild.get(), shader, modelMatrix, depth + 1);
        drawBVHNode(node->rightChild.get(), shader, modelMatrix, depth + 1);
    }
    else if (node->leftChild || node->rightChild) {
        return;
        std::cout << "WARNING: Leaf node has children at depth " << depth << std::endl;
    }
    shader.UnUse();
}

void Assignment2::drawBVH(GLSLShader& shader, glm::mat4 modelMatrix) {
        if (showTopDownAABB) {
            drawBVHTemplate<AABB>(shader, modelMatrix);
        }
        if (showTopDownSphere) {
            drawBVHTemplate<Sphere>(shader, modelMatrix);
        }

        if (showBottomUpAABB) {
            drawBVHTemplate<AABB>(shader, modelMatrix);
        }
        if (showBottomUpSphere) {
            drawBVHTemplate<Sphere>(shader, modelMatrix);
        }
}

void Assignment2::buildBottomUpBVH() {
    std::cout << "Starting buildBottomUpBVH" << std::endl;
    initializeLeafNodes<AABB>();
    std::cout << "Leaf nodes initialized. Count: " << leafNodesAABB.size() << std::endl;
    if (leafNodesAABB.empty()) {
        std::cout << "No leaf nodes to build BVH from" << std::endl;
        return;
    }
    buildBottomUpBVHGeneric<AABB>(leafNodesAABB, bottomUpBVHRootAABB);
    initializeLeafNodes<Sphere>();
    buildBottomUpBVHGeneric<Sphere>(leafNodesSphere, bottomUpBVHRootSphere);

    std::cout << "Bottom-up BVH built. Root node exists: " << (bottomUpBVHRootAABB != nullptr) << std::endl;
}


template<typename BoundingVolumeType>
void Assignment2::initializeLeafNodes()
{
    if constexpr (std::is_same_v<BoundingVolumeType, AABB>) {
        leafNodesAABB.clear();
    }
    if constexpr (std::is_same_v<BoundingVolumeType, Sphere>) {
        leafNodesSphere.clear();
    }

    std::cout << "Initializing leaf nodes for current section: " << currentSection << std::endl;

    if (sections.find(currentSection) != sections.end()) {
        const auto& currentSectionParts = sections[currentSection].parts;
        for (size_t i = 0; i < currentSectionParts.size(); ++i)
        {
            auto& _model = currentSectionParts[i];
            if (!_model)
            {
                std::cout << "Error: Null model in current section at index " << i << std::endl;
                continue;
            }
            try
            {
                auto node = std::make_unique<BVHNode<BoundingVolumeType>>();
                node->type = BVHNode<BoundingVolumeType>::NodeType::LEAF;

                if constexpr (std::is_same_v<BoundingVolumeType, AABB>)
                {
                    AABB modelAABB = _model->getAABB();
                    node->boundingVolume = std::make_unique<BoundingVolumeType>(modelAABB);
                    std::cout << "Created AABB for model in current section at index " << i << std::endl;
                }
                if constexpr (std::is_same_v<BoundingVolumeType, Sphere>)
                {
                    Sphere modelSphere = _model->getRitterSphere();
                    node->boundingVolume = std::make_unique<BoundingVolumeType>(modelSphere);
                    std::cout << "Created Sphere for model in current section at index " << i << std::endl;
                }

                node->model.push_back(_model.get());
                std::cout << "Added model from current section at index " << i << " to node" << std::endl;

                if constexpr (std::is_same_v<BoundingVolumeType, AABB>) {
                    leafNodesAABB.push_back(std::move(node));
                }
                else if constexpr (std::is_same_v<BoundingVolumeType, Sphere>) {
                    leafNodesSphere.push_back(std::move(node));
                }
            }
            catch (const std::exception& e)
            {
                std::cout << "Exception caught while processing model in current section at index " << i << ": " << e.what() << std::endl;
            }
        }
    }
    else {
        std::cout << "Current section " << currentSection << " not found." << std::endl;
    }

    if constexpr (std::is_same_v<BoundingVolumeType, AABB>) {
        std::cout << "Leaf node initialization complete. Total AABB leaf nodes for current section: " << leafNodesAABB.size() << std::endl;
    }
    else if constexpr (std::is_same_v<BoundingVolumeType, Sphere>) {
        std::cout << "Leaf node initialization complete. Total Sphere leaf nodes for current section: " << leafNodesSphere.size() << std::endl;
    }
}template<typename BoundingVolumeType>
Assignment2::MergeCost<BoundingVolumeType> Assignment2::calculateMergeCost(const BVHNode<BoundingVolumeType>* node1, const BVHNode<BoundingVolumeType>* node2) {
    MergeCost<BoundingVolumeType> cost;

    // Calculate distance (nearest neighbor)
    cost.distance = glm::distance(node1->boundingVolume->m_Center, node2->boundingVolume->m_Center);

    // Calculate combined volume
    float volume1, volume2, combinedVolume;

    if constexpr (std::is_same_v<BoundingVolumeType, AABB>) {
        // Calculate volumes for AABBs
        volume1 = 8.0f * node1->boundingVolume->m_HalfExtents.x *
            node1->boundingVolume->m_HalfExtents.y *
            node1->boundingVolume->m_HalfExtents.z;
        volume2 = 8.0f * node2->boundingVolume->m_HalfExtents.x *
            node2->boundingVolume->m_HalfExtents.y *
            node2->boundingVolume->m_HalfExtents.z;

        glm::vec3 minPoint = glm::min(
            node1->boundingVolume->m_Center - node1->boundingVolume->m_HalfExtents,
            node2->boundingVolume->m_Center - node2->boundingVolume->m_HalfExtents
        );
        glm::vec3 maxPoint = glm::max(
            node1->boundingVolume->m_Center + node1->boundingVolume->m_HalfExtents,
            node2->boundingVolume->m_Center + node2->boundingVolume->m_HalfExtents
        );
        glm::vec3 combinedExtents = maxPoint - minPoint;
        combinedVolume = combinedExtents.x * combinedExtents.y * combinedExtents.z;
    }
    else if constexpr (std::is_same_v<BoundingVolumeType, Sphere>) {
        // Calculate volumes for Spheres
        volume1 = (4.0f / 3.0f) * M_PI * std::pow(node1->boundingVolume->m_Radius, 3);
        volume2 = (4.0f / 3.0f) * M_PI * std::pow(node2->boundingVolume->m_Radius, 3);

        float newRadius = (cost.distance + node1->boundingVolume->m_Radius + node2->boundingVolume->m_Radius) * 0.5f;
        combinedVolume = (4.0f / 3.0f) * M_PI * std::pow(newRadius, 3);
    }

    cost.combinedVolume = combinedVolume;

    // Calculate relative increase in volume
    float maxVolume = std::max(volume1, volume2);
    cost.relativeIncrease = (combinedVolume - maxVolume) / maxVolume;

    return cost;
}

template<typename BoundingVolumeType>
void Assignment2::buildBottomUpBVHGeneric(std::vector<std::unique_ptr<BVHNode<BoundingVolumeType>>>& leafNodes, std::unique_ptr<BVHNode<BoundingVolumeType>>& root) {
    std::vector<std::unique_ptr<BVHNode<BoundingVolumeType>>> workingSet;

    // Move leaf nodes into the working set
    for (auto& leafNode : leafNodes) {
        workingSet.push_back(std::move(leafNode));
    }

    while (workingSet.size() > 1) {
        size_t bestI = 0, bestJ = 1;
        float bestScore = std::numeric_limits<float>::max();

        // Find the best pair to merge
        for (size_t i = 0; i < workingSet.size(); ++i) {
            for (size_t j = i + 1; j < workingSet.size(); ++j) {
                MergeCost<BoundingVolumeType> cost = calculateMergeCost(workingSet[i].get(), workingSet[j].get());
                float score = cost.distance + cost.combinedVolume;

                if (score < bestScore) {
                    bestScore = score;
                    bestI = i;
                    bestJ = j;
                }
            }
        }

        // Create a new internal node
        auto newNode = std::make_unique<BVHNode<BoundingVolumeType>>();
        newNode->type = BVHNode<BoundingVolumeType>::NodeType::INTERNAL;
        newNode->leftChild = std::move(workingSet[bestI]);
        newNode->rightChild = std::move(workingSet[bestJ]);

        // Calculate the new bounding volume
        if constexpr (std::is_same_v<BoundingVolumeType, AABB>) {
            glm::vec3 minPoint = glm::min(
                newNode->leftChild->boundingVolume->m_Center - newNode->leftChild->boundingVolume->m_HalfExtents,
                newNode->rightChild->boundingVolume->m_Center - newNode->rightChild->boundingVolume->m_HalfExtents
            );
            glm::vec3 maxPoint = glm::max(
                newNode->leftChild->boundingVolume->m_Center + newNode->leftChild->boundingVolume->m_HalfExtents,
                newNode->rightChild->boundingVolume->m_Center + newNode->rightChild->boundingVolume->m_HalfExtents
            );
            glm::vec3 center = (minPoint + maxPoint) * 0.5f;
            glm::vec3 halfExtents = (maxPoint - minPoint) * 0.5f;
            newNode->boundingVolume = std::make_unique<AABB>(center, halfExtents);
        }
        else if constexpr (std::is_same_v<BoundingVolumeType, Sphere>) {
            glm::vec3 center1 = newNode->leftChild->boundingVolume->m_Center;
            glm::vec3 center2 = newNode->rightChild->boundingVolume->m_Center;
            float radius1 = newNode->leftChild->boundingVolume->m_Radius;
            float radius2 = newNode->rightChild->boundingVolume->m_Radius;

            glm::vec3 direction = center2 - center1;
            // Manually calculate the length of the direction vector
            float distance = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
            glm::vec3 newCenter = center1 + direction * 0.5f;
            float newRadius = (distance + radius1 + radius2) * 0.5f;

            newNode->boundingVolume = std::make_unique<Sphere>(newCenter, newRadius);
        }

        // Combine the model lists
        newNode->model.insert(newNode->model.end(),
            newNode->leftChild->model.begin(),
            newNode->leftChild->model.end());
        newNode->model.insert(newNode->model.end(),
            newNode->rightChild->model.begin(),
            newNode->rightChild->model.end());

        // Remove the merged nodes and add the new node
        workingSet.erase(workingSet.begin() + std::max(bestI, bestJ));
        workingSet.erase(workingSet.begin() + std::min(bestI, bestJ));
        workingSet.push_back(std::move(newNode));
    }

    // The last remaining node is the root of the new BV Tree
    root = std::move(workingSet[0]);
}
template<typename BoundingVolumeType>
void Assignment2::updateBottomUpBVH() {
    if (std::is_same_v<BoundingVolumeType, AABB> && bottomUpBVHRootAABB) {
        updateBVHNodeLogic(bottomUpBVHRootAABB.get());
        updateBVHNodeBuffers(bottomUpBVHRootAABB.get());
    }
    if (std::is_same_v<BoundingVolumeType, Sphere> && bottomUpBVHRootSphere) {
        updateBVHNodeLogic(bottomUpBVHRootSphere.get());
        updateBVHNodeBuffers(bottomUpBVHRootSphere.get());
    }
}