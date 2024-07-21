****** IMPORTANT: Build Configurations - Please only run in Release Mode on x64 ******


a) How to use parts of your user interface that is NOT specified in the assignment description?
Loading Models:
Since we are not to place any .obj file in the submission pls follow the following to load the model correctly.
Create a Models file and place the following powerplant section model in the correct file path, example below.

Copy the models into the respective directories:
For Section 4 Part A:
g0.obj in Solution/Models/ppsection4/part_a/
g1.obj in Solution/Models/ppsection4/part_a/
g2.obj in Solution/Models/ppsection4/part_a/
For Section 4 Part B:
g0.obj in Solution/Models/ppsection4/part_b/
g1.obj in Solution/Models/ppsection4/part_b/

The file paths should look like this:
Solution/Models/ppsection4/part_a/g0.obj
Solution/Models/ppsection4/part_a/g1.obj
Solution/Models/ppsection4/part_a/g2.obj
Solution/Models/ppsection4/part_b/g0.obj
Solution/Models/ppsection4/part_b/g1.obj

Solution/
├── Models/
│   ├── ppsection4/
│   │   ├── part_a/
│   │   └── part_b/
│   ├── ppsection5/
│   │   ├── part_a/
│   │   └── part_b/
│   └── ppsection6/
│       ├── part_a/
│       └── part_b/
│       └── part_c/




The code for loading the model is found in Assignment2.cpp under Assignment2::load() - 
 sections[4].parts.push_back(std::make_unique<Model>("../Models/ppsection4/part_a/g0.obj"));
 sections[4].parts.push_back(std::make_unique<Model>("../Models/ppsection4/part_a/g1.obj"));
 sections[4].parts.push_back(std::make_unique<Model>("../Models/ppsection4/part_a/g2.obj"));
 sections[4].parts.push_back(std::make_unique<Model>("../Models/ppsection4/part_b/g0.obj"));
 sections[4].parts.push_back(std::make_unique<Model>("../Models/ppsection4/part_b/g1.obj"));


To use the parts of the user interface and key press functionality not specified in the assignment description, follow these steps:
Model Controls:
Use the "Select Section" to choose different section of the powerplant model.
Use the "Select Model" dropdown to choose the model you want to manipulate.
Adjust the "Scale", "Translation", and "Rotation" sliders under the "Transform" section to modify the selected model's attributes.
Toggle the visibility of various bounding volumes (Ritter, Larson, Eigen spheres, AABB, OBB, Ellipse) using checkboxes.
Change the bounding volume type for the selected model using the radio buttons and EPOS options.
Click "Recalculate Bounding Volume" to apply changes.

Global Controls:
Select the global bounding volume type for all models from the "Global Bounding Volume Type" dropdown.
Show or hide different sphere types for all models using the corresponding checkboxes.
Click "Recalculate All Bounding Volumes" to apply changes globally.

BVH Visualization:
Toggle visibility of different BVH types (Top Down AABB/Sphere, Bottom Up AABB/Sphere) using checkboxes.
Adjust the "BVH Depth" slider to set the depth of the BVH visualization.
Click "Recalculate BVHs" to update the BVH structure and visualization.

Camera and Light Movement:
Use keys (I, K, J, L, U, O) to move the light source in different directions.
Controls - WASD to move the camera around the scene.
	 - Space to move the camera up
	 - Shift to move the camera down
	 - Scroll Wheel to zoom in and out
         - Right Mouse Click to change what you are looking at

Build Configurations - Please only run in Release Mode on x64

b) Any assumption that you make on how to use the application that, if violated, might cause the application to fail?
Build Configurations - Please only run in Release Mode on x64 ONLY

c) Which part of the assignment has been completed? 
1. Objects loaded correctly from the files and displayed
2. Correct Rendering using diffuse color model 
3. Bounding Volumes:
- AABB
- Bounding Sphere – Ritter’s Method 
- Bounding Sphere – Modified Larsson’s Method
- Bounding Sphere – PCA Method
- OBB
- Ellipse

4.Bounding Volume Hierarchy:
- Top-down construction - AABB & Bounding Spheres
- Bottom-up construction - AABB & Bounding Spheres

5. Interactivity / Display:
- Toggle/Change Bounding Volumes 
- Display Bounding volumes in wireframe 
- Camera Implementation

6. Scene Manager

Extra Credit:
Oriented Bounding Box and Bounding Ellipsoid

d) Which part of the assignment has NOT been completed (not done, not working, etc.) and explanation on why those parts are not completed?
Nil

e) Where the relevant source codes (both C++ and shaders, as applicable) for the assignment are located. Specify the file path (folder name), file name, function name (or line number). 

Testing/Assignment2.h:
Model Processing Functions:
void loadModel();
void processNode();
void processMesh();

Bounding Volume Computation:
void computeModelAABB();
void computeModelBSphere();
void computeModelEllipse();
void computeModelOBB();

void initialRitterSphere();
void initialLarsonSphere();
void initialEigenSphere();
void growSphere(Sphere* sphere);

BVH (Bounding Volume Hierarchy) Management:
Top-Down BVH - 
std::unique_ptr<BVHNode<BoundingVolumeType>> buildTopDownBVH(std::vector<Model*>& models, int depth);

template<typename BoundingVolumeType>
std::unique_ptr<BVHNode<BoundingVolumeType>> buildBVHTemplate();

template<typename BoundingVolumeType>
void updateBVHNodeLogic(BVHNode<BoundingVolumeType>* node);

template<typename BoundingVolumeType>
void updateBVHNodeBuffers(BVHNode<BoundingVolumeType>* node);

template<typename BoundingVolumeType>
void drawBVHNode(BVHNode<BoundingVolumeType>* node, GLSLShader& shader, glm::mat4 mvpMatrix, int depth);

Bottom-Up BVH:
void buildBottomUpBVH();

template<typename BoundingVolumeType>
void buildBottomUpBVHGeneric(std::vector<std::unique_ptr<BVHNode<BoundingVolumeType>>>& leafNodes, std::unique_ptr<BVHNode<BoundingVolumeType>>& root);

template<typename BoundingVolumeType>
void buildBottomUpBVH();

template<typename BoundingVolumeType>
void updateBottomUpBVH();

template<typename BoundingVolumeType>
void drawBottomUpBVH(GLSLShader& shader, glm::mat4 modelMatrix);

template<typename BoundingVolumeType>
void drawBottomUpBVHNode(const BVHNode<BoundingVolumeType>* node, GLSLShader& shader, const glm::mat4& mvpMatrix, int depth);


BVH Initialization and Leaf Node Management:
template<typename BoundingVolumeType>
void initializeLeafNodes();

BVH Updates and Drawing:
void updateBVH();

template<typename BoundingVolumeType>
void updateBVHTemplate();

void drawBVH(GLSLShader& shader, glm::mat4 modelMatrix);

template<typename BoundingVolumeType>
void drawBVHTemplate(GLSLShader& shader, glm::mat4 modelMatrix);

BVH Partitioning and Merging:
template<typename BoundingVolumeType>
void partitionObjects(std::vector<Model*>& models, int& splitIndex);

template<typename BoundingVolumeType>
BoundingVolumeType computeBoundingVolume(std::vector<Model*>& models);

template<typename BoundingVolumeType>
glm::vec3 computeModelCentroid(Model& model);

template<typename BoundingVolumeType>
MergeCost<BoundingVolumeType> calculateMergeCost(const BVHNode<BoundingVolumeType>* node1, const BVHNode<BoundingVolumeType>* node2);


BVH Data Structures:
std::unique_ptr<BVHNode<AABB>> bottomUpBVHRootAABB;
std::vector<std::unique_ptr<BVHNode<AABB>>> leafNodesAABB;

std::unique_ptr<BVHNode<Sphere>> bottomUpBVHRootSphere;
std::vector<std::unique_ptr<BVHNode<Sphere>>> leafNodesSphere;

General BVH Building:
void buildBVH();

Testing/Assignment2.cpp:
Assignment2::load() -
Shader Compilation and Linking:
This function compiles and links two sets of shaders: the main shaders (vertex and fragment) for rendering the models, and the bounding sphere shaders specifically for rendering bounding volumes.
Loading Models: Several models are loaded from different parts of a power plant section, each with a specific position.
buildBVH(): This function constructs the BVH using a top-down approach. In this method, the entire space is recursively subdivided into smaller bounding volumes until each leaf node contains a single model or a manageable number of models.
buildBottomUpBVH(): This function constructs the BVH using a bottom-up approach. This involves starting with individual models and iteratively merging the nearest or smallest volume bounding volumes into larger parent volumes until a single root node is formed.

Testing/include/glapp.h:
class Scene_Base() - This is the base class that all past and future assignment will inherit from.
class Scene_Manager() - This is the scene manager that will handle the load, init, update and draw functionality of each scene

Testing/Camera.h:
class Camera() - This is where the camera class object is located


f) Which machine did you test your application on? 
OS - Window
GPU - Nvida GeForce RTX 3070 Laptop GPU
OpenGL Version - 4.5

g) The number of hours you spent on the assignment, on a weekly basis?
40 hours.

h) Any other useful information pertaining to the application?
Nil

i) Choosing the heuristic for bottom up BVH ( Code can be found in function calculateMergeCost() )
Distance between nodes (nearest neighbor)
Combined volume of the merged nodes
Relative increase in volume

The effect of this heuristic:
Distance: Favors merging nearby nodes, which tends to create more compact sub-trees.
Combined volume: Prefers merging nodes that result in smaller overall volumes, reducing empty space in the BVH.
Relative increase: Discourages merges that cause large jumps in volume, maintaining tighter bounding volumes at each level.

Combine these factors with weights:
float score = 0.3f * cost.distance + 0.4f * cost.combinedVolume + 0.3f * cost.relativeIncrease;

Adjusting these weights allows fine-tuning:
Increasing the distance weight creates more localized groupings.
Emphasizing combined volume prioritizes overall tree compactness.
Boosting relative increase weight maintains tighter bounds but may lead to deeper trees.




 

