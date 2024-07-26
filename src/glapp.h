/* !
@file		glapp.h
@author		pghali@digipen.edu
@co-author	bjornpokin.c@digipen.edu
@date		06/05/2022

This file contains the declaration of namespace GLApp that encapsulates the
functionality required to implement an OpenGL application including 
compiling, linking, and validating shader programs
setting up geometry and index buffers, 
configuring VAO to present the buffered geometry and index data to
vertex shaders,
configuring textures (in later labs),
configuring cameras (in later labs), 
and transformations (in later labs).
*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLAPP_H
#define GLAPP_H
#include "glhelper.h"
#include "Input.h"
#include <glm/gtc/matrix_transform.hpp>
#include "unordered_map"

/*                                                                   includes
----------------------------------------------------------------------------- */
class Scene_Base
{
public:
	virtual ~Scene_Base() = default;
	virtual void load() = 0;
	virtual void init() = 0;
	virtual void update(double dt) = 0;
	virtual void draw() = 0;
	virtual void cleanup() = 0;

	void setActive(bool active) {
		this->amIActive = active;
	}
	virtual bool isActive() const {
		return amIActive;
	}

private:
	bool amIActive = false;
};

class Scene_Manager
{
public:
	Scene_Manager() = default;
	~Scene_Manager() = default;
	void update(double dt)
	{
		for (auto& [id, scene] : scenes)
		{
			if (scene && scene->isActive())
			{
				scene->update(dt);
				// Check for OpenGL errors after each scene update
				GLenum err;
				while ((err = glGetError()) != GL_NO_ERROR) {
					std::cerr << "OpenGL error detected after updating scene " << id << ": " << err << std::endl;
				}
			}
		}
	}

	void addScene(std::string name, std::shared_ptr<Scene_Base> scene) 
	{
		std::cout << "Adding scene: " << name << std::endl;	
		scenes[name] = scene;
	}
	void activateScene(std::string name) 
	{
		auto scene = scenes.find(name);
		if (scene == scenes.end())
		{
			std::cerr << "Scene not found" << std::endl;
		}
		if (scene != scenes.end())
		{
			std::cout << "Activating scene: " << name << "\n";
			scene->second->setActive(true);
			scene->second->load();
			scene->second->init();
		}
	}

	void deactivateScene(std::string name)
	{
		auto it = scenes.find(name);
		if (it != scenes.end())
		{
			it->second->setActive(false);
			it->second->cleanup();
		}
	}
	Scene_Base* getScene(std::string name)
	{
		auto it = scenes.find(name);
		if (it != scenes.end())
		{
			return it->second.get();
		}
		return nullptr;
	}
	void terminate()
	{
		for (auto& pair : scenes)
		{
			auto& scene = pair.second;
			if (scene->isActive())
			{
				scene->cleanup();
			}
		}
		scenes.clear();
	}
	static Scene_Manager& getInstance()
	{
		static Scene_Manager instance;
		return instance;
	}
private:
	std::unordered_map<std::string, std::shared_ptr<Scene_Base>> scenes;
};


struct GLApp {
  static void init();
  static void update();
  static void draw();
  static void cleanup();
  //using EventcallbackFn = std::function<void(Event&)>;
  //static EventcallbackFn EventCallback;
  // encapsulates state required to render a geometrical model
  struct GLModel {
	  GLenum primitive_type{}; // which OpenGL primitive to be rendered?
	  GLuint vaoid{}; // handle to VAO
	  GLuint elem_cnt{}; // how many elements of primitive of type
	  unsigned int texid;
	  unsigned int bluefireID;
	  std::vector<glm::vec2> pos_vtx;
	  //static GLuint vbo_hdl{};
	  //void MoveLeft(float distance) {
		 // transformationMatrix = glm::translate(transformationMatrix, glm::vec3(-distance, 0.0f, 0.0f));
	  //}
	  //void MoveRight(float distance) {
		 // transformationMatrix = glm::translate(transformationMatrix, glm::vec3(distance, 0.0f, 0.0f));
	  //}
	  //void MoveUp(float distance) {
		 // transformationMatrix = glm::translate(transformationMatrix, glm::vec3(0.0f, distance, 0.0f));
	  //}
	  //void MoveDown(float distance) {
		 // transformationMatrix = glm::translate(transformationMatrix, glm::vec3(0.0f, -distance, 0.0f));
	  //}

	  //Texture texture;
	  // primitive_type are to be rendered
	  // member functions defined in glapp.cpp
	  void setup_vao();
	  void setup_shdrpgm();
	  void draw();
	
  };
  // data member to represent geometric model to be rendered
  // C++ requires this object to have a definition in glapp.cpp!!!
  static GLModel mdl;
  static GLModel fireblue;
  //static Input input;
}; 



#endif /* GLAPP_H */
