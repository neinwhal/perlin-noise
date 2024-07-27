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

// New method to generate DLA terrain
void PerlinNoise::GenerateDLATerrain() {
	// start with initial size
	dlaData.clear();
	dlaList.clear();

	int initial_size{ 8 };
	// populate 8 * 8 for first DLA
	for (int i{ 0 }; i < initial_size; i++) {
		dlaData.push_back({});
		for (int j{ 0 }; j < initial_size; j++) {
			dlaData[i].push_back(0.f);
		}
	}

	int upper_mid = initial_size / 2;
	int lower_mid = upper_mid - 1;
	int random_center_x = random::rand(lower_mid, upper_mid);
	int random_center_y = random::rand(lower_mid, upper_mid);
	dlaData[random_center_x][random_center_y] = 1.f;
	dlaList.push_back({ random_center_x, random_center_y });
	
	while (dlaList.size() < ((dlaData.size() * dlaData[0].size()) / 3) ) {

		// add a random dot
		bool populated = true;
		int dot_location_x, dot_location_y;
		while (populated) {
			dot_location_x = random::rand(0, static_cast<int>(dlaData[0].size() - 1));
			dot_location_y = random::rand(0, static_cast<int>(dlaData[0].size() - 1));
			if (dlaData[dot_location_x][dot_location_y] == 0.f) populated = false;
		}

		bool no_neighbours = true;

		// check for neighbours
		if (dot_location_x > 0) {
			if (dlaData[dot_location_x - 1][dot_location_y] > 0.f) {
				no_neighbours = false;
			}
		}
		if (dot_location_x < dlaData[0].size() - 1) {
			if (dlaData[dot_location_x + 1][dot_location_y] > 0.f) {
				no_neighbours = false;
			}
		}
		if (dot_location_y > 0) {
			if (dlaData[dot_location_x][dot_location_y - 1] > 0.f) {
				no_neighbours = false;
			}
		}
		if (dot_location_y < dlaData.size() - 1) {
			if (dlaData[dot_location_x][dot_location_y + 1] > 0.f) {
				no_neighbours = false;
			}
		}

		while (no_neighbours) {
			// move dot in random position
			int random_movement = random::rand(0, 3);
			if (random_movement == 0) {
				// move left
				if (dot_location_x > 0) {
					dot_location_x--;
				}
			}
			else if (random_movement == 1) {
				// move right
				if (dot_location_x < dlaData[0].size() - 1) {
					dot_location_x++;
				}
			}
			else if (random_movement == 2) {
				// move up
				if (dot_location_y > 0) {
					dot_location_y--;
				}
			}
			else if (random_movement == 3) {
				// move down
				if (dot_location_y < dlaData.size() - 1) {
					dot_location_y++;
				}
			}

			// check for neighbours
			if (dot_location_x > 0) {
				if (dlaData[dot_location_x - 1][dot_location_y] > 0.f) {
					no_neighbours = false;
					break;
				}
			}
			if (dot_location_x < dlaData[0].size() - 1) {
				if (dlaData[dot_location_x + 1][dot_location_y] > 0.f) {
					no_neighbours = false;
					break;
				}
			}
			if (dot_location_y > 0) {
				if (dlaData[dot_location_x][dot_location_y - 1] > 0.f) {
					no_neighbours = false;
					break;
				}
			}
			if (dot_location_y < dlaData.size() - 1) {
				if (dlaData[dot_location_x][dot_location_y + 1] > 0.f) {
					no_neighbours = false;
					break;
				}
			}
		}
		
		dlaData[dot_location_x][dot_location_y] = 1.f;
		dlaList.push_back({ dot_location_x, dot_location_y });
	}

	// resize to a higher level
}
