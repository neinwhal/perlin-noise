#pragma once
#include "glapp.h"
#include "./Camera.h"
#include "./Primitives.h"
#include "./IntersectionTest.h"
#include "./Primitives.h"
//#include "../Testing/src/main.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <glm/glm.hpp>
#include <algorithm>
#include <random>
#include <iterator>
#include <cmath>
#include <map>
#define M_PI 3.14159265358979323846
inline void checkGLError(const char* operation) {
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error after " << operation << ": " << error << std::endl;
	}
}
inline std::string to_string(const glm::vec3& vec) {
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
}
inline std::string to_string_mat4(const glm::mat4& matrix) {
	std::ostringstream oss;
	oss << "[\n";
	for (int i = 0; i < 4; ++i) {
		oss << " [";
		for (int j = 0; j < 4; ++j) {
			oss << matrix[i][j];
			if (j < 3) oss << ", ";
		}
		oss << "]";
		if (i < 3) oss << ",\n";
	}
	oss << "\n]";
	return oss.str();
}
inline std::string to_string_mat3(const glm::mat3& matrix) {
	std::ostringstream oss;
	oss << "[\n";
	for (int i = 0; i < 3; ++i) {  
		oss << " [";
		for (int j = 0; j < 3; ++j) {
			oss << matrix[i][j];
			if (j < 2) oss << ", ";  
		}
		oss << "]";
		if (i < 2) oss << ",\n"; 
	}
	oss << "\n]";
	return oss.str();
}
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 color; 
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	struct Material {
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 ambient;
		float shininess;
	}material;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
		: vertices(vertices), indices(indices) {
		setupMesh();
	}

	void draw(GLSLShader& shader) {
		glBindVertexArray(VAO);

		// Enable vertex attribute arrays
		glEnableVertexAttribArray(0); // position
		glEnableVertexAttribArray(1); // normal
		glEnableVertexAttribArray(2); // texCoords
		glEnableVertexAttribArray(3); // color

		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

		// Disable vertex attribute arrays
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	void setupMesh() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		// Vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		// Vertex Texture Coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		// Vertex Colors
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		glBindVertexArray(0);

		checkGLError("Mesh setup");
	}
private:
	unsigned int VAO, VBO, EBO;

	
};
class Model {
public:
	Model(const std::string& filePath, const glm::vec3& initialPostion = glm::vec3(0.0f))
	{
		loadModel(filePath, initialPostion);
		isInitialized = true;

	}

	// Dump vertex details	
	void printVertexDetails(const std::vector<Mesh>& meshes) {
		for (const auto& mesh : meshes) {
			std::cout << "Mesh Vertex Details:" << std::endl;
			for (const auto& vertex : mesh.vertices) {
				std::cout << "Position: (" << vertex.position.x << ", " << vertex.position.y << ", " << vertex.position.z << ")" << std::endl;
				std::cout << "Normal: (" << vertex.normal.x << ", " << vertex.normal.y << ", " << vertex.normal.z << ")" << std::endl;
				std::cout << "Texture Coords: (" << vertex.texCoords.x << ", " << vertex.texCoords.y << ")" << std::endl;
				std::cout << "Color: (" << vertex.color.x << ", " << vertex.color.y << ", " << vertex.color.z << ")" << std::endl;
			}
			std::cout << "---------------------" << std::endl;
		}
	}

	// ------------- Getters and Setters -------------------------------
	void setTranslation(const glm::vec3& translation) {
		m_translation = translation;
		updateModelMatrix();
	}

	void setRotation(const glm::vec3& rotation) {
		m_rotation = rotation;
		updateModelMatrix();
	}

	void setScale(float scale) {
		m_scale = scale;
		updateModelMatrix();
	}
	void setModelMatrix(const glm::mat4& matrix) {
		m_modelMatrix = matrix;
	}

	void setShowBoundingVolume(bool show) {
		showBoundingVolume = show;
	}

	enum class BVType {None, Sphere, AABB, OBB, Ellipse};

	void setBoundingVolumeType(BVType type) {
		boundingVolumeType = type;
	}
	BVType getBoundingVolumeType() const {
		return boundingVolumeType;
	}

	glm::vec3& getTranslation() {
		return m_translation;
	}
	glm::vec3& getRotation() {
		return m_rotation;
	}
	float getScale() {
		return m_scale;
	}
	glm::mat4& getModelMatrix() {
		return m_modelMatrix;
	}
	AABB& getAABB() {
		return *modelAABB;
	}
	Sphere& getRitterSphere() {
		return *ritterSphere;
	}
	enum class SphereMethod { Ritter, Larson, Eigen };
	enum class EPOS { EPOS3, EPOS4, EPOS6, EPOS12 };

	/*SphereMethod getSphereMethod() const {
		return m_sphereMethod;
	}*/
	//TODO: UGLY WAY, REDUNDANT CODE, REFACTOR, UGLY FOR NOW COS SPHERE AND SHADER NOT WOKING
	void setShowRitterSphere(bool show) { showRitterSphere = show; ritterSphere->m_isVisible = show; }
	void setShowLarsonSphere(bool show) { showLarsonSphere = show; larsonSphere->m_isVisible = show; }
	void setShowEigenSphere(bool show) { showEigenSphere = show; eigenSphere->m_isVisible = show;}
	bool getShowRitterSphere() { return showRitterSphere; }
	bool getShowLarsonSphere() { return showLarsonSphere; }
	bool getShowEigenSphere() { return showEigenSphere; }

	void setEPOS(EPOS epos) {
		currentEPOS = epos;
	}
	EPOS getCurrentEPOS() const {
		return currentEPOS;
	}	

	std::vector<Mesh>& getMeshes() { return meshes; }

	void toggleBoundingVolumes() {
		showBoundingVolume = !showBoundingVolume;
	}

	//------------------------------------------------------------------ //


	// ----------------- Transformations ------------------------------
	void recalculateBoundingVolume() {
		if (boundingVolumeType == BVType::Sphere) {
			//growRitterSphere(modelBSphere->m_Center, modelBSphere->m_Radius);
			updateBoundingSphere();
		}
		else if (boundingVolumeType == BVType::AABB) {
			updateAABB();
		}
		else if (boundingVolumeType == BVType::OBB) {
			updateOBB();
		}
		else if (boundingVolumeType == BVType::Ellipse) {
			updateBoundingEllipse();
		}
	}
	void updateModelMatrix() {
		m_modelMatrix = glm::mat4(1.0f);
		m_modelMatrix = glm::translate(m_modelMatrix, m_translation);
		m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
		m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
		m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
		m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(m_scale));

		/*std::cout << "Model Matrix updated:" << std::endl;
		std::cout << "  Translation: " << to_string(m_translation) << std::endl;
		std::cout << "  Rotation: " << to_string(m_rotation) << std::endl;
		std::cout << "  Scale: " << m_scale << std::endl;
		std::cout << "  Resulting Matrix:" << std::endl;*/
		for (int i = 0; i < 4; ++i) {
			std::cout << "    " << to_string(m_modelMatrix[i]) << std::endl;
		}
	}
	void updateBoundingEllipse() {
		if (meshes.empty()) {
			return;
		}

		// Calculate the covariance matrix of the transformed vertices
		glm::mat3 covarianceMatrix = calculateCovarianceMatrixFromMeshes(meshes, m_modelMatrix);

		// Find the principal axis (max eigenvector)
		glm::vec3 principalAxis = findMaxEigenvector(covarianceMatrix);

		// Calculate the other two axes to form an orthonormal basis
		glm::vec3 secondAxis = glm::normalize(glm::cross(principalAxis, glm::vec3(0.0f, 1.0f, 0.0f)));
		if (glm::length(secondAxis) < 0.01f) {  // If principalAxis is close to (0, 1, 0)
			secondAxis = glm::normalize(glm::cross(principalAxis, glm::vec3(1.0f, 0.0f, 0.0f)));
		}
		glm::vec3 thirdAxis = glm::cross(principalAxis, secondAxis);

		// Form the rotation matrix
		glm::mat3 rotationMatrix(principalAxis, secondAxis, thirdAxis);

		// Calculate the center (mean) of all vertices
		glm::vec3 center(0.0f);
		size_t totalVertices = 0;

		for (const auto& mesh : meshes) {
			for (const auto& vertex : mesh.vertices) {
				center += glm::vec3(m_modelMatrix * glm::vec4(vertex.position, 1.0f));
				totalVertices++;
			}
		}
		center /= static_cast<float>(totalVertices);

		// Initialize max extents
		glm::vec3 maxExtents(0.0f);

		// Project all vertices onto the axes and find max extents
		for (const auto& mesh : meshes) {
			for (const auto& vertex : mesh.vertices) {
				glm::vec3 transformedVertex = glm::vec3(m_modelMatrix * glm::vec4(vertex.position, 1.0f));
				glm::vec3 localPos = rotationMatrix * (transformedVertex - center);
				maxExtents = glm::max(maxExtents, glm::abs(localPos));
			}
		}

		// Scale the half-axes to contain all points
		float scaleFactor = 1.01f; // Add a small buffer
		glm::vec3 halfAxes = maxExtents * scaleFactor;

		// Create or update Bounding Ellipse
		if (!modelBEllipse) {
			modelBEllipse = std::make_unique<BoundingEllipse>(center, halfAxes, rotationMatrix, 1.0f);
		}
		else {
			modelBEllipse->setPosition(center);
			modelBEllipse->setHalfAxes(halfAxes);
			modelBEllipse->setRotationMatrix(rotationMatrix);
		}

		updateBoundingVoluesBuffers();

		//std::cout << "Bounding Ellipse updated. Center: " << to_string(center)
		//	<< ", Half Axes: " << to_string(halfAxes)
		//	<< ", Rotation: " << to_string_mat3(rotationMatrix) << std::endl;
	}
	void updateOBB() {
		if (meshes.empty()) {
			return;
		}

		glm::mat3 covarianceMatrix = calculateCovarianceMatrixFromMeshes(meshes, m_modelMatrix);

		glm::vec3 principalAxis = findMaxEigenvector(covarianceMatrix);

		glm::vec3 secondAxis = glm::normalize(glm::cross(principalAxis, glm::vec3(0.0f, 1.0f, 0.0f)));
		if (glm::length(secondAxis) < 0.01f) {  // If principalAxis is close to (0, 1, 0)
			secondAxis = glm::normalize(glm::cross(principalAxis, glm::vec3(1.0f, 0.0f, 0.0f)));
		}
		glm::vec3 thirdAxis = glm::cross(principalAxis, secondAxis);

		glm::mat3 rotationMatrix(principalAxis, secondAxis, thirdAxis);

		glm::vec3 min(std::numeric_limits<float>::max());
		glm::vec3 max(std::numeric_limits<float>::lowest());

		for (const auto& mesh : meshes) {
			for (const auto& vertex : mesh.vertices) {
				glm::vec3 transformedVertex = glm::vec3(m_modelMatrix * glm::vec4(vertex.position, 1.0f));
				glm::vec3 projectedVertex = rotationMatrix * transformedVertex;
				min = glm::min(min, projectedVertex);
				max = glm::max(max, projectedVertex);
			}
		}

		glm::vec3 center = (min + max) * 0.5f;
		glm::vec3 halfExtents = (max - min) * 0.5f;

		center = glm::inverse(rotationMatrix) * center;

		if (!modelOBB) {
			modelOBB = std::make_unique<OBB>(center, halfExtents, rotationMatrix);
		}
		else {
			modelOBB->m_Center = center;
			modelOBB->m_HalfExtents = halfExtents;
			modelOBB->m_RotationMatrix = rotationMatrix;
		}

		updateBoundingVoluesBuffers();

	/*	std::cout << "OBB updated. Center: " << to_string(center)
			<< ", Half Extents: " << to_string(halfExtents)
			<< ", Rotation: " << to_string_mat3(rotationMatrix) << std::endl;*/
	}
	void updateAABB() {
		glm::vec3 min(std::numeric_limits<float>::max());
		glm::vec3 max(std::numeric_limits<float>::lowest());

		for (const auto& mesh : meshes) {
			for (const auto& vertex : mesh.vertices) {
				glm::vec3 transformedPosition = glm::vec3(m_modelMatrix * glm::vec4(vertex.position, 1.0f));
				min = glm::min(min, transformedPosition);
				max = glm::max(max, transformedPosition);
			}
		}

		glm::vec3 center = (min + max) * 0.5f;
		glm::vec3 halfExtents = (max - min) * 0.5f;

		if (!modelAABB) {
			modelAABB = std::make_unique<AABB>(center, halfExtents);
		}
		else {
			modelAABB->m_Center = center;
			modelAABB->m_HalfExtents = halfExtents;
		}

		updateBoundingVoluesBuffers();

		//std::cout << "AABB updated. Center: " << to_string(center)
		//	<< ", Half Extents: " << to_string(halfExtents) << std::endl;
	}
	void updateBoundingSphere() {
		// Reeeter stuff
		std::pair<glm::vec3, float> pairRitter = initialRitterSphere();
		ritterSphere->m_Center = pairRitter.first;
		ritterSphere->m_Radius = pairRitter.second;
		growSphere(ritterSphere.get());

		// Larson sutff
		std::pair<glm::vec3, float> pairLarson = initialLarsonSphere();
		larsonSphere->m_Center = pairLarson.first;
		larsonSphere->m_Radius = pairLarson.second;
		growSphere(larsonSphere.get());

		// Eigen stuff
		std::pair<glm::vec3, float> pairEigen = initialEigenSphere();
		eigenSphere->m_Center = pairEigen.first;
		eigenSphere->m_Radius = pairEigen.second;
		growSphere(eigenSphere.get());

		updateBoundingVoluesBuffers(); // Update all dem buffffers
	}
	void updateBoundingVoluesBuffers()
	{
		//if (boundingVolumeType == BVType::Sphere) {
			if (ritterSphere) {
				ritterSphere->updateBuffers(ritterSphere->sphereVAO, ritterSphere->sphereVBO, ritterSphere->sphereEBO, 0);
			}
			if (larsonSphere) {
				larsonSphere->updateBuffers(larsonSphere->sphereVAO, larsonSphere->sphereVBO, larsonSphere->sphereEBO, 0);
			}
			if (eigenSphere) {
				eigenSphere->updateBuffers(eigenSphere->sphereVAO, eigenSphere->sphereVBO, eigenSphere->sphereEBO, 0);
			}
		
		if (modelAABB) {
			modelAABB->updateBuffers(modelAABB->aabbVAO, modelAABB->aabbVBO, modelAABB->aabbEBO, 0);
		}
		if (modelOBB)
		{
			modelOBB->updateBuffers(modelOBB->obbVAO, modelOBB->obbVBO, modelOBB->obbEBO, 0);
		}
		if (modelBEllipse) {
			GLuint ebo = {};
			modelBEllipse->updateBuffers(modelBEllipse->ellipseVAO, modelBEllipse->ellipseVBO, ebo, 0);
		}
	}
	void draw(GLSLShader& shader, GLSLShader& sphere_shader, const Camera& camera, const glm::mat4& parentModelMatrix) {
		glm::mat4 finalModelMatrix = parentModelMatrix * m_modelMatrix;
		shader.Use();
		shader.SetUniform("modelMatrix", finalModelMatrix);
		shader.SetUniform("mvpMatrix", camera.getProjectionMatrix() * camera.getViewMatrix() * finalModelMatrix);
		for (size_t i = 0; i < meshes.size(); ++i) {
			meshes[i].draw(shader);
		}
		shader.UnUse();
		if (showBoundingVolume) {
			drawBoundingVolumes(sphere_shader, camera, parentModelMatrix);
		}

	}
	void drawBoundingVolumes(GLSLShader& sphere_shader, const Camera& camera, const glm::mat4& modelMatrix) {
    if (showBoundingVolume) {
        sphere_shader.Use();
        glm::mat4 mvpMatrix = camera.getProjectionMatrix() * camera.getViewMatrix() * modelMatrix;
        sphere_shader.SetUniform("mvpMatrix", mvpMatrix);
		GLint current_program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
		//std::cout << "Current shader program: " << current_program << std::endl;
        if (boundingVolumeType == BVType::Sphere) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            
            if (ritterSphere && showRitterSphere) {
                sphere_shader.SetUniform("sphereType", 0);
                ritterSphere->draw(ritterSphere->sphereVAO, ritterSphere->sphereVBO, ritterSphere->sphereEBO, 0);
				checkGLError("After Drawing Ritter Sphere");

            }
            
            if (larsonSphere && showLarsonSphere) {
                sphere_shader.SetUniform("sphereType", 1);
                larsonSphere->draw(larsonSphere->sphereVAO, larsonSphere->sphereVBO, larsonSphere->sphereEBO, 0);
				checkGLError("After Drawing Larson Sphere");

            }
            
            if (eigenSphere && showEigenSphere) {
                sphere_shader.SetUniform("sphereType", 2);
                eigenSphere->draw(eigenSphere->sphereVAO, eigenSphere->sphereVBO, eigenSphere->sphereEBO, 0);
				checkGLError("After Drawing Eigen Sphere");

            }
            
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else if (boundingVolumeType == BVType::AABB && modelAABB) {
            sphere_shader.SetUniform("sphereType", 3);
            modelAABB->draw(aabbVAO, aabbVBO, aabbEBO, 0);
        }

		else if (boundingVolumeType == BVType::OBB && modelOBB)
		{
			sphere_shader.SetUniform("sphereType", 3);
			modelOBB->draw(modelOBB->obbVAO, modelOBB->obbVBO, modelOBB->obbEBO, 0);

		}
		else if (boundingVolumeType == BVType::Ellipse && modelBEllipse) {
			sphere_shader.SetUniform("sphereType", 3);
			GLuint ebo = {};
			modelBEllipse->draw(modelBEllipse->ellipseVAO, modelBEllipse->ellipseVBO, ebo, 0);
		}
        sphere_shader.UnUse();
    }
	}
	void centerModel() {
		glm::vec3 min(std::numeric_limits<float>::max());
		glm::vec3 max(std::numeric_limits<float>::lowest());

		for (const auto& mesh : meshes) {
			for (const auto& vertex : mesh.vertices) {
				min = glm::min(min, vertex.position);
				max = glm::max(max, vertex.position);
			}
		}

		glm::vec3 center = (min + max) * 0.5f;

		for (auto& mesh : meshes) {
			for (auto& vertex : mesh.vertices) {
				vertex.position -= center;
			}
			mesh.setupMesh(); 
		}

		//std::cout << "Model centered. New center: (0, 0, 0)" << std::endl;
		//std::cout << "Model dimensions: " << to_string(max - min) << std::endl;
	}
private:
	// ----------------- Model Data -----------------
	std::vector<Mesh> meshes;
	std::string directory;
	bool isInitialized = false;
	//------------------------------------------------------------------ //

	// ----------------- Transformations -----------------
	glm::vec3 m_translation{ 0.0f };
	glm::vec3 m_rotation{ 0.0f };
	float m_scale{ 1.0f };
	glm::mat4 m_modelMatrix{ 1.0f };

	// ----------------- Bounding Volumes Toggle -----------------
	bool showBoundingVolume = true;
	BVType boundingVolumeType = BVType::Sphere;
	//------------------------------------------------------------------ //
	
	//struct BVHNode {
	//	std::unique_ptr<GameObject> boundingVolume;
	//	std::vector<BVHNode> children;
	//};
	//BVHNode bvhRoot;

	// ----------------- Loading Model -----------------
	void loadModel(const std::string& path, const glm::vec3& initialPostion = glm::vec3(0.0f)) {
		try {
			std::cout << "Loading model: " << path << std::endl;
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
				std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
				throw std::runtime_error("Failed to load model.");
			}

			directory = path.substr(0, path.find_last_of('/'));
			processNode(scene->mRootNode, scene);

			glm::vec3 min(std::numeric_limits<float>::max());
			glm::vec3 max(std::numeric_limits<float>::lowest());

			for (const auto& mesh : meshes) {
				for (const auto& vertex : mesh.vertices) {
					min = glm::min(min, vertex.position);
					max = glm::max(max, vertex.position);
				}
			}

			size_t totalVertices = 0;
			for (const auto& mesh : meshes) {
				totalVertices += mesh.vertices.size();
			}
			std::cout << "Total vertices loaded: " << totalVertices << std::endl;
			std::cout << "Model loaded. Bounding box:" << std::endl;
			std::cout << "Min: " << to_string(min) << std::endl;
			std::cout << "Max: " << to_string(max) << std::endl;
			std::cout << "Dimensions: " << to_string(max - min) << std::endl;

			centerModel();
			setTranslation(initialPostion);
			
			computeBoundingVolumes();
			//printVertexDetails(meshes);
		}
		catch (const std::exception& e) {
			std::cerr << "Exception occurred in loadModel: " << e.what() << std::endl;
	
		}
	}
	void processNode(aiNode* node, const aiScene* scene) {
		// Process all the node meshes 
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// Then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}
	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		// Process vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			// Process vertex positions, normals and texture coordinates
			vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			if (mesh->HasNormals()) {
				vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			}
			if (mesh->mTextureCoords[0]) {
				vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			}
			else {
				vertex.texCoords = glm::vec2(0.0f, 0.0f);
			}
		
			vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);

			vertices.push_back(vertex);
		}

		// Process indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// Process material
		Mesh::Material material;
		//material.ambient = glm::vec3(0.1f, 0.1f, 0.1f);  // Default values
		//material.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
		//material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
		//material.shininess = 32.0f;
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

			aiColor3D color(0.f, 0.f, 0.f);
			if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
				material.diffuse = glm::vec3(color.r, color.g, color.b);
			}
			if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
				material.specular = glm::vec3(color.r, color.g, color.b);
			}
			if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
				material.ambient = glm::vec3(color.r, color.g, color.b);
			}

			float shininess;
			if (mat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
				material.shininess = shininess;
			}

			// If the obj got color data, use it
			if (mesh->HasVertexColors(0)) {
				for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
					vertices[i].color = glm::vec3(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b);
				}
			}
			else {
				// If no vertex colors, use the diffuse color for all vertices
				for (auto& vertex : vertices) {
					vertex.color = material.diffuse;
				}
			}
		}
		else
		{
			//// If no material, use default values
			//material.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
			//material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
			//material.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
			//material.shininess = 32.0f;

			std::cout << "no mateiral" << std::endl;
		}
		std::cout << "Processed mesh with material properties:" << std::endl;
		std::cout << "Ambient: " << to_string(material.ambient) << std::endl;
		std::cout << "Diffuse: " << to_string(material.diffuse) << std::endl;
		std::cout << "Specular: " << to_string(material.specular) << std::endl;
		std::cout << "Shininess: " << material.shininess << std::endl;
		Mesh result(vertices, indices);
		result.material = material;
		return result;
	}
	// ------------------------------------------------- //


	// ----------------- Bounding Volumes -----------------
		
	//AABB
	GLuint aabbVAO;
	GLuint aabbVBO;
	GLuint aabbEBO;
	std::unique_ptr<AABB> modelAABB;
	std::unique_ptr<BoundingEllipse> modelBEllipse;
	std::unique_ptr<OBB> modelOBB;


	//SphereMethod m_sphereMethod = SphereMethod::Ritter;
	EPOS currentEPOS = EPOS::EPOS6;

	std::unique_ptr<Sphere> ritterSphere; 
	std::unique_ptr<Sphere> larsonSphere;
	std::unique_ptr<Sphere> eigenSphere;

	// laze ass way to go about showing this lol. 
	bool showRitterSphere = false;
	bool showLarsonSphere = false;
	bool showEigenSphere = false;
	bool showOBB = false;
	bool showEllipse = false;

	// Compute Bounding Volumes
	void computeBoundingVolumes()
	{
		computeModelAABB();
		computeModelBSphere();
		computeModelEllipse();
		computeModelOBB();
	}
	// OBB Bounding Calculation
	void computeModelOBB()
	{
		if (meshes.empty()) {
			return;
		}

		glm::mat3 covarianceMatrix = calculateCovarianceMatrixFromMeshes(meshes, m_modelMatrix);

		glm::vec3 principalAxis = findMaxEigenvector(covarianceMatrix);

		glm::vec3 secondAxis = glm::normalize(glm::cross(principalAxis, glm::vec3(0.0f, 1.0f, 0.0f)));
		if (glm::length(secondAxis) < 0.01f) {  // If principalAxis is close to (0, 1, 0)
			secondAxis = glm::normalize(glm::cross(principalAxis, glm::vec3(1.0f, 0.0f, 0.0f)));
		}
		glm::vec3 thirdAxis = glm::cross(principalAxis, secondAxis);

		glm::mat3 rotationMatrix(principalAxis, secondAxis, thirdAxis);

		glm::vec3 minExtents(std::numeric_limits<float>::max());
		glm::vec3 maxExtents(std::numeric_limits<float>::lowest());

		for (const auto& mesh : meshes) {
			for (const auto& vertex : mesh.vertices) {
				glm::vec3 transformedVertex = m_modelMatrix * glm::vec4(vertex.position, 1.0f);
				glm::vec3 projectedVertex = rotationMatrix * transformedVertex;
				minExtents = glm::min(minExtents, projectedVertex);
				maxExtents = glm::max(maxExtents, projectedVertex);
			}
		}

		glm::vec3 center = (minExtents + maxExtents) * 0.5f;
		glm::vec3 halfExtents = (maxExtents - minExtents) * 0.5f;

		center = glm::inverse(rotationMatrix) * center;

		// Create or update OBB
		if (!modelOBB) {
			modelOBB = std::make_unique<OBB>(center, halfExtents, rotationMatrix);
		}
		else {
			modelOBB->setCenter(center);
			modelOBB->setHalfExtents(halfExtents);
			modelOBB->setRotationMatrix(rotationMatrix);
		}
	}

	// Bounding Ellipse Calculation
	void computeModelEllipse()
	{
		if (meshes.empty()) {
			return;
		}

		glm::vec3 center(0.0f);
		size_t totalVertices = 0;

		for (const auto& mesh : meshes) {
			for (const auto& vertex : mesh.vertices) {
				center += glm::vec3(m_modelMatrix * glm::vec4(vertex.position, 1.0f));
				totalVertices++;
			}
		}
		center /= static_cast<float>(totalVertices);

		glm::mat3 covarianceMatrix = calculateCovarianceMatrixFromMeshes(meshes, m_modelMatrix);

		glm::vec3 principalAxis = findMaxEigenvector(covarianceMatrix);

		glm::vec3 secondAxis = glm::normalize(glm::cross(principalAxis, glm::vec3(0.0f, 1.0f, 0.0f)));
		if (glm::length(secondAxis) < 0.01f) {  // If principalAxis is close to (0, 1, 0)
			secondAxis = glm::normalize(glm::cross(principalAxis, glm::vec3(1.0f, 0.0f, 0.0f)));
		}
		glm::vec3 thirdAxis = glm::cross(principalAxis, secondAxis);

		// Form the rotation matrix
		glm::mat3 rotationMatrix(principalAxis, secondAxis, thirdAxis);

		// Calculate the half-axes
		glm::vec3 halfAxes(0.0f);
		for (const auto& mesh : meshes) {
			for (const auto& vertex : mesh.vertices) {
				glm::vec3 localPos = rotationMatrix * (glm::vec3(m_modelMatrix * glm::vec4(vertex.position, 1.0f)) - center);
				halfAxes = glm::max(halfAxes, glm::abs(localPos));
			}
		}

		// Scale the half-axes to contain all points
		float scaleFactor = 1.01f; // Add a small buffer
		halfAxes *= scaleFactor;

		// Create or update the Bounding Ellipse
		if (!modelBEllipse) {
			modelBEllipse = std::make_unique<BoundingEllipse>(center, halfAxes, rotationMatrix, 1.0f); 
		}
		else {
			modelBEllipse->setPosition(center);
			modelBEllipse->setHalfAxes(halfAxes);
			modelBEllipse->setRotationMatrix(rotationMatrix);
		}

		//std::cout << "Bounding Ellipse updated. Center: " << to_string(center)
		//	<< ", Half Axes: " << to_string(halfAxes)
		//	<< ", Rotation: " << to_string_mat3(rotationMatrix) << std::endl;
	}
	
	// AABB Bounding Volumes
	void computeModelAABB()
	{
		// Compute AABB
		glm::vec3 min(std::numeric_limits<float>::max());
		glm::vec3 max(std::numeric_limits<float>::lowest());
		for (const auto& mesh : meshes) {
			for (const auto& vertex : mesh.vertices) {
				min = glm::min(min, vertex.position);
				max = glm::max(max, vertex.position);
			}
		}
		glm::vec3 center = (min + max) * 0.5f;
		glm::vec3 halfExtents = (max - min) * 0.5f;

		// Create or update AABB
		if (!modelAABB) {
			modelAABB = std::make_unique<AABB>(center, halfExtents);
		}
		else {
			modelAABB->setCenter(center);
			modelAABB->setHalfExtents(halfExtents);
		}
	}
	// Sphere Bounding Volume
	// Sphere Bounding Volume
	void computeModelBSphere()
	{
		// Create spheres
		ritterSphere = std::make_unique<Sphere>(initialRitterSphere().first, initialRitterSphere().second, 32, 32, glm::vec3(1.0f, 0.0f, 0.0f));
		larsonSphere = std::make_unique<Sphere>(initialLarsonSphere().first, initialLarsonSphere().second, 32, 32, glm::vec3(0.0f, 1.0f, 0.0f));
		eigenSphere = std::make_unique<Sphere>(initialEigenSphere().first, initialEigenSphere().second, 32, 32, glm::vec3(0.0f, 0.0f, 1.0f));

		ritterSphere->updateBuffers(ritterSphere->sphereVAO, ritterSphere->sphereVBO, ritterSphere->sphereEBO, 0);
		larsonSphere->updateBuffers(larsonSphere->sphereVAO, larsonSphere->sphereVBO, larsonSphere->sphereEBO, 0);
		eigenSphere->updateBuffers(eigenSphere->sphereVAO, eigenSphere->sphereVBO, eigenSphere->sphereEBO, 0);
	}
	//  Calculate the Ritter sphere
	std::pair<glm::vec3, float> initialRitterSphere() {
		glm::vec3 min(std::numeric_limits<float>::max());
		glm::vec3 max(std::numeric_limits<float>::lowest());

		//std::vector<Vertex> allVertices;
		//for (const auto& mesh : meshes) {
		//	allVertices.insert(allVertices.end(), mesh.vertices.begin(), mesh.vertices.end());
		//}
		//int k = std::min(10000, static_cast<int>(allVertices.size()));
		//std::cout << "Number of vertices (RITTER): " << allVertices.size() << std::endl;
		//if (k > allVertices.size())
		//{
		//	std::cerr << "Number of vertices is less than k" << std::endl;
		//	return std::make_pair(glm::vec3(0.0f), 0.0f);
		//}

		//std::vector<Vertex> selectedVertices;
		//std::unordered_set<int> uniqueIndices;
		//int i = 0;
		//while (selectedVertices.size() < k) {
		//	if (uniqueIndices.find(i) == uniqueIndices.end()) {
		//		selectedVertices.push_back(allVertices[i]);
		//		uniqueIndices.insert(i);
		//	}
		//	i++;
		//}
	/*	for (const auto& mesh : meshes) {
			for (const auto& vertex : mesh.vertices) {
				glm::vec3 transformedPosition = glm::vec3(m_modelMatrix * glm::vec4(vertex.position, 1.0f));
				min = glm::min(min, transformedPosition);
				max = glm::max(max, transformedPosition);
			}

		}*/
		int k = 100000;
		int count = 0;

		for (const auto& mesh : meshes) {
			for (const auto& vertex : mesh.vertices) {
				if (count >= k) {
					break;
				}

				glm::vec3 transformedPosition = glm::vec3(m_modelMatrix * glm::vec4(vertex.position, 1.0f));
				min = glm::min(min, transformedPosition);
				max = glm::max(max, transformedPosition);

				count++;
			}

			if (count >= k) {
				break;
			}
		}
		//std::cout << "Max Points: " << to_string(max) << std::endl;
		//std::cout << "Min Points: " << to_string(min) << std::endl;
		glm::vec3 center = (min + max) * 0.5f;
		float radius = std::max({ std::abs(max.x - center.x),
						 std::abs(max.y - center.y),
						 std::abs(max.z - center.z) });

		//std::cout << "Initial Ritter Sphere Center: " << to_string(center) << std::endl;
		//std::cout << "Initial Ritter Sphere Radius: " << radius << std::endl;

		return std::make_pair(center, radius);
	}
	std::pair<glm::vec3, float> initialLarsonSphere() {
		std::vector<Vertex> allVertices;
		for (const auto& mesh : meshes) {
			allVertices.insert(allVertices.end(), mesh.vertices.begin(), mesh.vertices.end());
		}
		int k = std::min(10000, static_cast<int>(allVertices.size()));

		if (k > allVertices.size()) {
			std::cerr << "Number of vertices is less than k" << std::endl;
			return std::make_pair(glm::vec3(0.0f), 0.0f);
		}

		std::vector<Vertex> selectedVertices;
		std::unordered_set<int> uniqueIndices;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distrib(0, allVertices.size() - 1);

		while (selectedVertices.size() < k) {
			int randomIndex = distrib(gen);
			if (uniqueIndices.find(randomIndex) == uniqueIndices.end()) {
				selectedVertices.push_back(allVertices[randomIndex]);
				uniqueIndices.insert(randomIndex);
			}
		}

		// Dictionary of direction vectors
		std::vector<glm::vec3> directions;

		// Switch case for the different direction vector
		switch (currentEPOS) {
		case EPOS::EPOS3:
			directions = {
				glm::vec3(1.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 1.0f)
			};
			break;
		case EPOS::EPOS4:
			directions = {
				glm::vec3(1.0f, 1.0f, 1.0f),
				glm::vec3(1.0f, 1.0f, -1.0f),
				glm::vec3(1.0f, -1.0f, 1.0f),
				glm::vec3(1.0f, -1.0f, -1.0f)
			};
			break;
		case EPOS::EPOS6:
			directions = {
				glm::vec3(1.0f, 1.0f, 0.0f),
				glm::vec3(1.0f, -1.0f, 0.0f),
				glm::vec3(1.0f, 0.0f, 1.0f),
				glm::vec3(1.0f, 0.0f, -1.0f),
				glm::vec3(0.0f, 1.0f, 1.0f),
				glm::vec3(0.0f, 1.0f, -1.0f)
			};
			break;
		case EPOS::EPOS12:
			directions = {
				glm::vec3(1.0f, 1.0f, 2.0f),
				glm::vec3(2.0f, 1.0f, 1.0f),
				glm::vec3(1.0f, 2.0f, 1.0f),
				glm::vec3(1.0f, -1.0f, 2.0f),
				glm::vec3(1.0f, 1.0f, -2.0f),
				glm::vec3(1.0f, -1.0f, -2.0f),
				glm::vec3(2.0f, -1.0f, 1.0f),
				glm::vec3(2.0f, 1.0f, -1.0f),
				glm::vec3(2.0f, -1.0f, -1.0f),
				glm::vec3(1.0f, -2.0f, 1.0f),
				glm::vec3(1.0f, 2.0f, -1.0f),
				glm::vec3(1.0f, -2.0f, -1.0f)
			};
			break;
		}

		// Loop through all the directions and find the extreme points
		glm::vec3 minExetrmalPoints(std::numeric_limits<float>::max());
		glm::vec3 maxExetrmalPoints(std::numeric_limits<float>::lowest());
		for (const auto& direction : directions) {
			float minDot = std::numeric_limits<float>::max();
			float maxDot = std::numeric_limits<float>::lowest();
			glm::vec3 minPoint;
			glm::vec3 maxPoint;
			for (const auto& vertex : selectedVertices) {
				glm::vec3 transformedPosition = glm::vec3(m_modelMatrix * glm::vec4(vertex.position, 1.0f));
				float dot = glm::dot(transformedPosition, glm::normalize(direction));
				if (dot < minDot) {
					minDot = dot;
					minPoint = transformedPosition;
				}
				if (dot > maxDot) {
					maxDot = dot;
					maxPoint = transformedPosition;
				}
			}
			minExetrmalPoints = glm::min(minExetrmalPoints, minPoint);
			maxExetrmalPoints = glm::max(maxExetrmalPoints, maxPoint);
		}

		// Find the center and radius of the sphere
		glm::vec3 center = (maxExetrmalPoints + minExetrmalPoints) * 0.5f;
		//std::cout << "Max Points: " << to_string(maxExetrmalPoints) << std::endl;
		//std::cout << "Min Points: " << to_string(minExetrmalPoints) << std::endl;
		float radius = std::max({
			std::abs(maxExetrmalPoints.x - center.x),
			std::abs(maxExetrmalPoints.y - center.y),
			std::abs(maxExetrmalPoints.z - center.z)
			});
		//std::cout << "Initial Larson Sphere Center: " << to_string(center) << std::endl;
		//std::cout << "Initial Larson Sphere Radius: " << radius << std::endl;

		return std::make_pair(center, radius);
	}

	
	// Calculate the Eigen Sphere
	glm::mat3 calculateCovarianceMatrix(const std::vector<glm::vec3>& points) {
		if (points.empty()) {
			return glm::mat3(1.0f); 
			std::cout << "Points empty in calcuaslteCovranceMAtrix" << std::endl;
		}

		size_t n = points.size();

		// Calculate mean
		glm::vec3 sum(0.0f);
		for (const auto& point : points) {
			sum += point;
		}
		glm::vec3 mean = sum / static_cast<float>(points.size());
		// Calculate covariance matrix elements
		float xx = 0.0f, yy = 0.0f, zz = 0.0f, xy = 0.0f, xz = 0.0f, yz = 0.0f;

		for (const auto& point : points) {
			glm::vec3 diff = point - mean;
			xx += diff.x * diff.x;
			yy += diff.y * diff.y;
			zz += diff.z * diff.z;
			xy += diff.x * diff.y;
			xz += diff.x * diff.z;
			yz += diff.y * diff.z;
		}

		float inv_n = 1.0f / static_cast<float>(n);
		xx *= inv_n;
		yy *= inv_n;
		zz *= inv_n;
		xy *= inv_n;
		xz *= inv_n;
		yz *= inv_n;

		// Construct the covariance matrix
		glm::mat3 covariance;
		covariance[0][0] = xx; covariance[0][1] = xy; covariance[0][2] = xz;
		covariance[1][0] = xy; covariance[1][1] = yy; covariance[1][2] = yz;
		covariance[2][0] = xz; covariance[2][1] = yz; covariance[2][2] = zz;
		return covariance;
	}
	glm::mat3 calculateCovarianceMatrixFromMeshes(const std::vector<Mesh>& meshes, const glm::mat4& modelMatrix) {
		std::vector<glm::vec3> transformedPoints;

		std::vector<Vertex> allVertices;
		for (const auto& mesh : meshes) {
			allVertices.insert(allVertices.end(), mesh.vertices.begin(), mesh.vertices.end());
		}
		int k = std::min(5000, static_cast<int>(allVertices.size()));
		if (k > allVertices.size())
		{
			std::cerr << "Number of vertices is less than k" << std::endl;
		}

		std::vector<Vertex> selectedVertices;
		std::unordered_set<int> uniqueIndices;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distrib(0, allVertices.size() - 1);
		int i = 0;
		while (selectedVertices.size() < k) {
			if (uniqueIndices.find(i) == uniqueIndices.end()) {
				selectedVertices.push_back(allVertices[i]);
				uniqueIndices.insert(i);
			}
			i++;
		}

		for (const auto& vertex : selectedVertices) {
			glm::vec3 transformedPosition = glm::vec3(modelMatrix * glm::vec4(vertex.position, 1.0f));
			transformedPoints.push_back(transformedPosition);
		}
		

		return calculateCovarianceMatrix(transformedPoints);
	}
	const int MAX_ITERATIONS = 50;
	const float EPSILON = 1e-6f;
	void jacobiRotation(glm::mat3& A, glm::mat3& V) {
		V = glm::mat3(1.0f);

		float prevoff;
		for (int n = 0; n < _MAX_ITERATIONS; n++) {
			int p = 0, q = 1;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					if (i == j) continue;
					if (Abs(A[i][j]) > Abs(A[p][q])) {
						p = i;
						q = j;
					}
				}
			}

			float c, s;
			SymSchur2(A, p, q, c, s);
			glm::mat3 J(1.0f);
			J[p][p] = c; J[p][q] = s;
			J[q][p] = -s; J[q][q] = c;

			V = V * J;

			A = glm::transpose(J) * A * J;

			float off = 0.0f;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					if (i == j) continue;
					off += A[i][j] * A[i][j];
				}
			}

			if (n > 2 && off >= prevoff)
				return;

			prevoff = off;
		}

	}
	glm::vec3 findMaxEigenvector(const glm::mat3& covarianceMatrix) {
		glm::mat3 A = covarianceMatrix;
		glm::mat3 V(1.0f); 

		jacobiRotation(A, V);

		float max_eigenvalue = A[0][0];
		int max_index = 0;
		for (int i = 1; i < 3; ++i) {
			if (A[i][i] > max_eigenvalue) {
				max_eigenvalue = A[i][i];
				max_index = i;
			}
		}
		// Return the corresponding eigenvector
		return glm::vec3(V[0][max_index], V[1][max_index], V[2][max_index]);
	}
	std::pair<glm::vec3, float> initialEigenSphere()
	{
		glm::mat3 covarianceMatrix = calculateCovarianceMatrixFromMeshes(meshes, m_modelMatrix);
		glm::vec3 maxDirectionVector = findMaxEigenvector(covarianceMatrix);	
		
		float minDot = std::numeric_limits<float>::max();
		float maxDot = std::numeric_limits<float>::lowest();
		glm::vec3 minPoint;
		glm::vec3 maxPoint;
		for (const auto& mesh : meshes) {
			for (const auto& vertex : mesh.vertices) {
				glm::vec3 transformedPosition = glm::vec3(m_modelMatrix * glm::vec4(vertex.position, 1.0f));
				float dot = glm::dot(transformedPosition, maxDirectionVector);

				if (dot < minDot) {
					minDot = dot;
					minPoint = transformedPosition;
				}
				if (dot > maxDot) {
					maxDot = dot;
					maxPoint = transformedPosition;
				}

			}
		}
		glm::vec3 center = (minPoint + maxPoint) * 0.5f;
		float radius = std::max({
								std::abs(maxPoint.x - center.x),
								std::abs(maxPoint.y - center.y),
								std::abs(maxPoint.z - center.z)
			});
		//std::cout << "Initial Egien Sphere Center: " << to_string(center) << std::endl;
		//std::cout << "Initial Egien Sphere Radius: " << radius << std::endl;
		return std::make_pair(center, radius);
	}
	void growSphere(Sphere* sphere)
	{
		// Grow the sphere 
		for (const auto& mesh : meshes) {
			for (const auto& vertex : mesh.vertices) {
				glm::vec3 transformedPosition = glm::vec3(m_modelMatrix * glm::vec4(vertex.position, 1.0f));
				glm::vec3 direction = transformedPosition - sphere->m_Center;
				float distance = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

				if (distance > sphere->m_Radius) {
					float newRadius = (distance + sphere->m_Radius) * 0.5f;
					glm::vec3 newCenter = sphere->m_Center + ((newRadius - sphere->m_Radius) / distance) * direction;

					sphere->m_Center = newCenter;
					sphere->m_Radius = newRadius;
				}
			}
		}
	}

	const float _EPSILON = 1e-4f;
	const int _MAX_ITERATIONS = 50;

	float Abs(float x) { return std::abs(x); }
	float Sqrt(float x) { return std::sqrt(x); }

	void SymSchur2(glm::mat3& a, int p, int q, float& c, float& s)
	{
		if (Abs(a[p][q]) > _EPSILON) {
			float r = (a[q][q] - a[p][p]) / (2.0f * a[p][q]);
			float t;
			if (r >= 0.0f)
				t = 1.0f / (r + Sqrt(1.0f + r * r));
			else
				t = -1.0f / (-r + Sqrt(1.0f + r * r));
			c = 1.0f / Sqrt(1.0f + t * t);
			s = t * c;
		}
		else {
			c = 1.0f;
			s = 0.0f;
		}
	}
};

template<typename BoundingVolumeType>
class BVHNode {
public:
	enum class NodeType { INTERNAL, LEAF };

	NodeType type;
	std::unique_ptr<BoundingVolumeType> boundingVolume;
	std::vector<Model*> model;
	std::unique_ptr<BVHNode> leftChild;
	std::unique_ptr<BVHNode> rightChild;
	BoundingVolumeType logicalBoundingVolume;

	BVHNode() : type(NodeType::LEAF), boundingVolume(nullptr), leftChild(nullptr), rightChild(nullptr) {}
};
struct Section {
	std::vector<std::unique_ptr<Model>> parts;
};
class Assignment2 : public Scene_Base
{
public:
	Assignment2(Scene_Manager& sceneManager, const std::string& name)
		: _sceneManager(sceneManager), _name(name), elapsedTime(0.0f),
		m_cameraSpeed(2.5f), m_cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), m_cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
		camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 45.0f, (float)GLHelper::width / (float)GLHelper::height, 0.1f, 100.0f),
		pipCamera(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f, 0.1f, 100.0f)
	{
	}
	struct Light {
		glm::vec3 direction;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	} light;
	virtual void load() override;
	virtual void init() override;
	virtual void update(double dt) override;
	virtual void draw() override;
	virtual void cleanup() override;

	void moveLight(float dx, float dy, float dz) {
		light.direction.x += dx;
		light.direction.y += dy;
		light.direction.z += dz;
		light.direction = glm::normalize(light.direction);
	}

	GLSLShader& getShaderProgram() { return shdr_pgm; }
	//std::vector<std::unique_ptr<Model>>& getModels() { return models; }

	void buildBVH();

	template<typename BoundingVolumeType>
	std::unique_ptr<BVHNode<BoundingVolumeType>> buildBVHTemplate();

	void updateBVH();
	template<typename BoundingVolumeType>
	void updateBVHTemplate();

	void drawBVH(GLSLShader& shader, glm::mat4 modelMatrix);

	template<typename BoundingVolumeType>
	void drawBVHTemplate(GLSLShader& shader, glm::mat4 modelMatrix);

	template<typename BoundingVolumeType>
	void partitionObjects(std::vector<Model*>& models, int& splitIndex);

	template<typename BoundingVolumeType>
	BoundingVolumeType computeBoundingVolume(std::vector<Model*>& models);

	template<typename BoundingVolumeType>
	glm::vec3 computeModelCentroid(Model& model);
	
	template<typename BoundingVolumeType>
	void debugPrintBVHStructure(const BVHNode<BoundingVolumeType>* node, int depth = 0) {
		if (!node) return;

		std::string indent(depth * 2, ' ');
		std::cout << indent << "Node at depth " << depth << ":" << std::endl;
		std::cout << indent << "  Type: " << (node->type == BVHNode<BoundingVolumeType>::NodeType::LEAF ? "Leaf" : "Internal") << std::endl;
		std::cout << indent << "  Center: " << to_string(node->boundingVolume->m_Center) << std::endl;
		std::cout << indent << "  Half Extents: " << to_string(node->boundingVolume->m_HalfExtents) << std::endl;
		std::cout << indent << "  Models: " << node->model.size() << std::endl;

		if (node->type == BVHNode<BoundingVolumeType>::NodeType::INTERNAL) {
			debugPrintBVHStructure(node->leftChild.get(), depth + 1);
			debugPrintBVHStructure(node->rightChild.get(), depth + 1);
		}
	}

	//////////////////////////////////////////////
	template<typename BoundingVolumeType>
	void buildBottomUpBVH();

	template<typename BoundingVolumeType>
	void updateBottomUpBVH();

	template<typename BoundingVolumeType>
	void drawBottomUpBVH(GLSLShader& shader, glm::mat4 modelMatrix);

	//void debugVisualizeBVH() {
	//	if (bvhRoot) {
	//		std::cout << "BVH Structure:\n";
	//		debugVisualizeBVHNode(bvhRoot.get(), 0);
	//	}
	//	else {
	//		std::cout << "BVH is empty.\n";
	//	}
	//}
	int MAX_TREE_DEPTH = 30;
	const int MAX_OBJECTS_PER_LEAF = 1;
	int currentBVHDepth = 0;

	template<typename BoundingVolumeType>
	struct MergeCost {
		float distance;
		float combinedVolume;
		float relativeIncrease;
	};
	int currentSection = 4;  // Default to section 4
private:
	// Scene Manager
	Scene_Manager& _sceneManager;
	double elapsedTime;

	// Name of this assignemtn for Scene Manager
	std::string _name;

	// Shader
	GLSLShader shdr_pgm;
	GLSLShader bounding_sphere_shader;

	// Camera
	Camera camera;
	Camera pipCamera;
	float m_cameraSpeed;
	glm::vec3 m_cameraFront;
	glm::vec3 m_cameraUp;

	// Vector to store all Models
	std::map<int, Section> sections;


	// Top Down AABB/Sphere BVH
	std::unique_ptr<BVHNode<AABB>> bvhRoot;  
	std::unique_ptr<BVHNode<Sphere>> topDownBVHSpere;

	bool showBVH = true;
	bool showTopDownAABB = false;
	bool showTopDownSphere = false;

	template<typename BoundingVolumeType>
	std::unique_ptr<BVHNode<BoundingVolumeType>> buildTopDownBVH(std::vector<Model*>& models, int depth);

	template<typename BoundingVolumeType>
	void updateBVHNodeLogic(BVHNode<BoundingVolumeType>* node);

	template<typename BoundingVolumeType>
	void updateBVHNodeBuffers(BVHNode<BoundingVolumeType>* node);

	template<typename BoundingVolumeType>
	void drawBVHNode(BVHNode<BoundingVolumeType>* node, GLSLShader& shader, glm::mat4 mvpMatrix, int depth);


	///////////////////////////////////////////////////////////////////////////////////////////////

	std::unique_ptr<BVHNode<AABB>> bottomUpBVHRootAABB;
	std::vector<std::unique_ptr<BVHNode<AABB>>> leafNodesAABB;

	std::unique_ptr<BVHNode<Sphere>> bottomUpBVHRootSphere;
	std::vector<std::unique_ptr<BVHNode<Sphere>>> leafNodesSphere;

	template<typename BoundingVolumeType>
	void initializeLeafNodes();


	void buildBottomUpBVH();

	template<typename BoundingVolumeType>
	void buildBottomUpBVHGeneric(std::vector<std::unique_ptr<BVHNode<BoundingVolumeType>>>& leafNodes, std::unique_ptr<BVHNode<BoundingVolumeType>>& root);

	template<typename BoundingVolumeType>
	MergeCost<BoundingVolumeType> calculateMergeCost(const BVHNode<BoundingVolumeType>* node1, const BVHNode<BoundingVolumeType>* node2);

	template<typename BoundingVolumeType>
	void drawBottomUpBVHNode(const BVHNode<BoundingVolumeType>* node, GLSLShader& shader, const glm::mat4& mvpMatrix, int depth);

	bool showBottomUpAABB = false;
	bool showBottomUpSphere = false;
};