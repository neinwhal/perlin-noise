#include "Noise.hpp"

// New method to generate DLA terrain
void PerlinNoise::GenerateDLATerrain() {
	// start with initial size
	dlaNoise.clear();

	// populate 8 * 8 for first DLA
	for (int i{ 0 }; i < 8; i++) {
		for (int j{ 0 }; j < 8; j++) {
			dlaNoise.push_back(0.f);
		}
	}

	dlaNoise[27] = 1.f;
	// add the first point somewhere in the centre


}
