/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.
Course:     CS350
Project:    Assignment 1
Filename:   random.hpp
Author(s):  Ang Wei Jie <weijie.ang@digipen.edu>
Date:       16/06/2024

Contents:
	- RNG
====================================================================================*/

#ifndef RANDOM_HPP
#define RANDOM_HPP

namespace random {
	double rand(double lower, double upper); // double randomizer
	float rand(float lower, float upper); // float randomizer
	int rand(int lower, int upper); // int randomizer
}

#endif