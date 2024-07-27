#include "Noise.hpp"
#include "random.hpp"

int getCenterIndex(int gridSize, int quadrantIndex) {
	int halfSize = gridSize / 2;

	int topLeft = (halfSize - 1) * gridSize + (halfSize - 1);
	int topRight = (halfSize - 1) * gridSize + halfSize;
	int bottomLeft = halfSize * gridSize + (halfSize - 1);
	int bottomRight = halfSize * gridSize + halfSize;

	switch (quadrantIndex) {
	case 0: return topLeft;
	case 1: return topRight;
	case 2: return bottomLeft;
	case 3: return bottomRight;
	default: throw std::out_of_range("quadrantIndex must be between 0 and 3");
	}
}

float get_squeezed_pos(int pos, size_t size) {
    return static_cast<float>(pos) / static_cast<float>(size - 1);
    //float ratio = static_cast<float>(pos) / static_cast<float>(size - 1);
    //return 0.1f + 0.8f * ratio; // Squeeze towards the center
}

// Function to perform convolution on a 2D vector with a simple averaging kernel
std::vector<std::vector<float>> simpleAverageConvolution(const std::vector<std::vector<float>>& input) {
    int rows = static_cast<int>(input.size());
    int cols = static_cast<int>(input[0].size());

    std::vector<std::vector<float>> output(rows, std::vector<float>(cols, 0.0f));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float sum = input[i][j];
            int count = 1;

            // Check up
            if (i > 0) {
                sum += input[i - 1][j];
                count++;
            }
            // Check down
            if (i < rows - 1) {
                sum += input[i + 1][j];
                count++;
            }
            // Check left
            if (j > 0) {
                sum += input[i][j - 1];
                count++;
            }
            // Check right
            if (j < cols - 1) {
                sum += input[i][j + 1];
                count++;
            }

            // Calculate the average
            output[i][j] = sum / static_cast<float>(count);
        }
    }

    return output;
}

// New method to generate DLA terrain
void PerlinNoise::GenerateDLATerrain(int stage) {
    if (stage == 0) {
        // Start with initial size
        dlaData.clear();
        dlaList.clear();

        int initial_size{ 8 };
        // Populate 8 * 8 for first DLA
        dlaData.resize(initial_size, std::vector<float>(initial_size, 0.f));

        int upper_mid = initial_size / 2;
        int lower_mid = upper_mid - 1;
        int random_center_x = random::rand(lower_mid, upper_mid);
        int random_center_y = random::rand(lower_mid, upper_mid);
        dlaData[random_center_x][random_center_y] = 1.f;
        dlaList.push_back({
            get_squeezed_pos(random_center_x, dlaData[0].size()),
            get_squeezed_pos(random_center_y, dlaData.size()),
            -1.f, -1.f
            });

        while (dlaList.size() < ((dlaData.size() * dlaData[0].size()) / 12)) {
            // Add a random dot
            bool populated = true;
            int dot_location_x, dot_location_y;
            while (populated) {
                dot_location_x = random::rand(0, static_cast<int>(dlaData[0].size() - 1));
                dot_location_y = random::rand(0, static_cast<int>(dlaData[0].size() - 1));
                if (dlaData[dot_location_x][dot_location_y] == 0.f) populated = false;
            }

            bool attached = false;
            float neighbour_x = -1, neighbour_y = -1;

            while (!attached) {
                // Move dot in random direction
                int random_movement = random::rand(0, 3);
                if (random_movement == 0 && dot_location_x > 0) { // move left
                    dot_location_x--;
                }
                else if (random_movement == 1 && dot_location_x < dlaData[0].size() - 1) { // move right
                    dot_location_x++;
                }
                else if (random_movement == 2 && dot_location_y > 0) { // move up
                    dot_location_y--;
                }
                else if (random_movement == 3 && dot_location_y < dlaData.size() - 1) { // move down
                    dot_location_y++;
                }

                // Check for neighbours
                if (dot_location_x > 0 && dlaData[dot_location_x - 1][dot_location_y] > 0.f) {
                    neighbour_x = get_squeezed_pos(dot_location_x - 1, dlaData[0].size());
                    neighbour_y = get_squeezed_pos(dot_location_y, dlaData.size());
                    attached = true;
                }
                else if (dot_location_x < dlaData[0].size() - 1 && dlaData[dot_location_x + 1][dot_location_y] > 0.f) {
                    neighbour_x = get_squeezed_pos(dot_location_x + 1, dlaData[0].size());
                    neighbour_y = get_squeezed_pos(dot_location_y, dlaData.size());
                    attached = true;
                }
                else if (dot_location_y > 0 && dlaData[dot_location_x][dot_location_y - 1] > 0.f) {
                    neighbour_x = get_squeezed_pos(dot_location_x, dlaData[0].size());
                    neighbour_y = get_squeezed_pos(dot_location_y - 1, dlaData.size());
                    attached = true;
                }
                else if (dot_location_y < dlaData.size() - 1 && dlaData[dot_location_x][dot_location_y + 1] > 0.f) {
                    neighbour_x = get_squeezed_pos(dot_location_x, dlaData[0].size());
                    neighbour_y = get_squeezed_pos(dot_location_y + 1, dlaData.size());
                    attached = true;
                }
            }

            dlaData[dot_location_x][dot_location_y] = 1.f;
            dlaList.push_back({
                get_squeezed_pos(dot_location_x, dlaData[0].size()),
                get_squeezed_pos(dot_location_y, dlaData.size()),
                neighbour_x, neighbour_y
            });
        }
        blurry_dlaData = dlaData;
    }

    if (stage == 1) {
        // Resize to a higher level
        size_t old_size = dlaData.size();
        size_t new_size = dlaData.size() * 2;

        // BLURRY upscale
        std::vector<std::vector<float>> new_blurry_dlaData;
        new_blurry_dlaData.resize(new_size, std::vector<float>(new_size, 0.f));

        for (size_t i = 0; i < new_size; ++i) {
            for (size_t j = 0; j < new_size; ++j) {
                // Calculate the corresponding positions in the old data
                float old_x = static_cast<float>(i) / 2.0f;
                float old_y = static_cast<float>(j) / 2.0f;

                int x0 = static_cast<int>(std::floor(old_x));
                int x1 = x0 + 1;
                int y0 = static_cast<int>(std::floor(old_y));
                int y1 = y0 + 1;

                // Boundary check
                x0 = std::clamp(x0, 0, static_cast<int>(old_size - 1));
                x1 = std::clamp(x1, 0, static_cast<int>(old_size - 1));
                y0 = std::clamp(y0, 0, static_cast<int>(old_size - 1));
                y1 = std::clamp(y1, 0, static_cast<int>(old_size - 1));

                float x_weight = old_x - x0;
                float y_weight = old_y - y0;

                // Get the values from the four corners
                float top_left = blurry_dlaData[x0][y0];
                float top_right = blurry_dlaData[x1][y0];
                float bottom_left = blurry_dlaData[x0][y1];
                float bottom_right = blurry_dlaData[x1][y1];

                // Perform bilinear interpolation
                float interpolated_value = (top_left * (1 - x_weight) * (1 - y_weight) +
                    top_right * x_weight * (1 - y_weight) +
                    bottom_left * (1 - x_weight) * y_weight +
                    bottom_right * x_weight * y_weight);

                // Assign the interpolated value to the new_blurry_dlaData
                new_blurry_dlaData[i][j] = interpolated_value;
            }
        }
        blurry_dlaData = new_blurry_dlaData;
        
        // CRISP upscale
        dlaData.clear();
        dlaData.resize(new_size, std::vector<float>(new_size, 0.f));

        // Populate the new vector with old data
        for (const auto& node : dlaList) {
            int new_x = static_cast<int>(node.x * (new_size - 1));
            int new_y = static_cast<int>(node.y * (new_size - 1));

            // Fill the path between (new_x, new_y) and (parent_x, parent_y)
            if (node.parent_x < 0.f || node.parent_y < 0.f) continue;

            int parent_x = static_cast<int>(node.parent_x * (new_size - 1));
            int parent_y = static_cast<int>(node.parent_y * (new_size - 1));

            int dx = std::abs(parent_x - new_x);
            int dy = std::abs(parent_y - new_y);
            int sx = (new_x < parent_x) ? 1 : -1;
            int sy = (new_y < parent_y) ? 1 : -1;
            int err = dx - dy;

            while (true) {
                if (new_x >= 0 && new_x < new_size && new_y >= 0 && new_y < new_size) {
                    dlaData[new_x][new_y] = 1.0f;
                }

                if (new_x == parent_x && new_y == parent_y) break;

                int e2 = 2 * err;
                if (e2 > -dy) {
                    err -= dy;
                    new_x += sx;
                }
                if (e2 < dx) {
                    err += dx;
                    new_y += sy;
                }
            }
        }

        // Continue to populate the map with additional dots
        size_t target_size = ((dlaData.size() * dlaData[0].size()) / static_cast<size_t>(3 * std::sqrt(new_size))); // Adjusted based on size
        while (dlaList.size() < target_size) {
            // Add a random dot
            bool populated = true;
            int dot_location_x, dot_location_y;
            while (populated) {
                dot_location_x = random::rand(0, static_cast<int>(dlaData[0].size() - 1));
                dot_location_y = random::rand(0, static_cast<int>(dlaData[0].size() - 1));
                if (dlaData[dot_location_x][dot_location_y] == 0.f) populated = false;
            }

            bool attached = false;
            float neighbour_x = -1, neighbour_y = -1;

            while (!attached) {
                // Move dot in random direction
                int random_movement = random::rand(0, 3);
                if (random_movement == 0 && dot_location_x > 0) { // move left
                    dot_location_x--;
                }
                else if (random_movement == 1 && dot_location_x < dlaData[0].size() - 1) { // move right
                    dot_location_x++;
                }
                else if (random_movement == 2 && dot_location_y > 0) { // move up
                    dot_location_y--;
                }
                else if (random_movement == 3 && dot_location_y < dlaData.size() - 1) { // move down
                    dot_location_y++;
                }

                // Check for neighbours
                if (dot_location_x > 0 && dlaData[dot_location_x - 1][dot_location_y] > 0.f) {
                    neighbour_x = get_squeezed_pos(dot_location_x - 1, dlaData[0].size());
                    neighbour_y = get_squeezed_pos(dot_location_y, dlaData.size());
                    attached = true;
                }
                else if (dot_location_x < dlaData[0].size() - 1 && dlaData[dot_location_x + 1][dot_location_y] > 0.f) {
                    neighbour_x = get_squeezed_pos(dot_location_x + 1, dlaData[0].size());
                    neighbour_y = get_squeezed_pos(dot_location_y, dlaData.size());
                    attached = true;
                }
                else if (dot_location_y > 0 && dlaData[dot_location_x][dot_location_y - 1] > 0.f) {
                    neighbour_x = get_squeezed_pos(dot_location_x, dlaData[0].size());
                    neighbour_y = get_squeezed_pos(dot_location_y - 1, dlaData.size());
                    attached = true;
                }
                else if (dot_location_y < dlaData.size() - 1 && dlaData[dot_location_x][dot_location_y + 1] > 0.f) {
                    neighbour_x = get_squeezed_pos(dot_location_x, dlaData[0].size());
                    neighbour_y = get_squeezed_pos(dot_location_y + 1, dlaData.size());
                    attached = true;
                }
            }

            dlaData[dot_location_x][dot_location_y] = 1.f;
            dlaList.push_back({
                get_squeezed_pos(dot_location_x, dlaData[0].size()),
                get_squeezed_pos(dot_location_y, dlaData.size()),
                neighbour_x, neighbour_y
                });
        }

        for (size_t i = 0; i < dlaData.size(); ++i) {
            for (size_t j = 0; j < dlaData[0].size(); ++j) {
                blurry_dlaData[i][j] += dlaData[i][j];
            }
        }
        blurry_dlaData = simpleAverageConvolution(blurry_dlaData);

        //for 
    }
}


void PerlinNoise::GenerateTerrainDLA() {
    // Calculate the correct size for a square grid
    int dlaSize = static_cast<int>(std::sqrt(flat_blurry_dlaData.size()));
    float minHeight = 0.0f;
    float maxHeight = 1.0f;

    vertices[DLA_NOISE].clear();
    indices[DLA_NOISE].clear();

    // Color palette 
    glm::vec3 deepWaterColor(0.0f, 0.05f, 0.3f);
    glm::vec3 shallowWaterColor(0.0f, 0.4f, 0.8f);
    glm::vec3 sandColor(0.76f, 0.7f, 0.5f);
    glm::vec3 grassColor(0.0f, 0.5f, 0.0f);
    glm::vec3 forestColor(0.0f, 0.3f, 0.0f);
    glm::vec3 rockColor(0.5f, 0.5f, 0.5f);
    glm::vec3 snowColor(1.0f, 1.0f, 1.0f);

    float waterLevel = 0.35f;

    // Find the min and max heights in the data for better normalization
    float dataMin = *std::min_element(flat_blurry_dlaData.begin(), flat_blurry_dlaData.end());
    float dataMax = *std::max_element(flat_blurry_dlaData.begin(), flat_blurry_dlaData.end());

    for (int z = 0; z < dlaSize; z++) {
        for (int x = 0; x < dlaSize; x++) {
            int index = z * dlaSize + x;
            float normalizedHeight = (flat_blurry_dlaData[index] - dataMin) / (dataMax - dataMin);
            float y = minHeight + normalizedHeight * (maxHeight - minHeight);

            glm::vec3 position(
                x / static_cast<float>(dlaSize - 1) - 0.5f,
                y * heightMultiplier,
                z / static_cast<float>(dlaSize - 1) - 0.5f
            );

            glm::vec3 color;

            if (normalizedHeight < waterLevel) {
                float waterDepth = (waterLevel - normalizedHeight) / waterLevel;
                color = glm::mix(shallowWaterColor, deepWaterColor, waterDepth);
                position.y = waterLevel * heightMultiplier;
            }
            else if (normalizedHeight < 0.4f) {
                float t = (normalizedHeight - waterLevel) / 0.05f;
                color = glm::mix(sandColor, grassColor, t);
            }
            else if (normalizedHeight < 0.7f) {
                float t = (normalizedHeight - 0.4f) / 0.3f;
                color = glm::mix(grassColor, forestColor, t);
            }
            else if (normalizedHeight < 0.85f) {
                float t = (normalizedHeight - 0.7f) / 0.15f;
                color = glm::mix(forestColor, rockColor, t);
            }
            else {
                float t = (normalizedHeight - 0.85f) / 0.15f;
                color = glm::mix(rockColor, snowColor, t);
            }

            vertices[DLA_NOISE].push_back({ position, color });
        }
    }

    // Generate indices
    for (int z = 0; z < dlaSize - 1; z++) {
        for (int x = 0; x < dlaSize - 1; x++) {
            int topLeft = z * dlaSize + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * dlaSize + x;
            int bottomRight = bottomLeft + 1;

            indices[DLA_NOISE].push_back(topLeft);
            indices[DLA_NOISE].push_back(bottomLeft);
            indices[DLA_NOISE].push_back(topRight);

            indices[DLA_NOISE].push_back(topRight);
            indices[DLA_NOISE].push_back(bottomLeft);
            indices[DLA_NOISE].push_back(bottomRight);
        }
    }
}