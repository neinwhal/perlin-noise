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

float get_relative_pos(int x, size_t size) {
	return static_cast<float>(x) / static_cast<float>(size - 1);
}

float get_squeezed_pos(int pos, size_t size) {
    float ratio = static_cast<float>(pos) / static_cast<float>(size - 1);
    return 0.1f + 0.8f * ratio; // Squeeze towards the center
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

        while (dlaList.size() < ((dlaData.size() * dlaData[0].size()) / 4)) {
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
    }

    if (stage == 1) {
        // Resize to a higher level
        size_t new_size = dlaData.size() * 2;

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
    }
}