/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.
Course:     CS350
Project:    Assignment 1
Filename:   random.cpp
Author(s):  Ang Wei Jie <weijie.ang@digipen.edu>
Date:       16/06/2024

Contents:
	- RNG
====================================================================================*/

#include <random>
#include "random.hpp"

namespace random {
	/*!*****************************************************************************
	\brief
		Double randomizer

	\param[in] lower
		double as lower limit

	\param[in] y
		double as upper limit

	\return
		randomized double
	*******************************************************************************/
	double rand(double lower, double upper) {
		if (lower == upper) return lower;

		std::random_device rd;
		std::mt19937 mt(rd()); // use Mersenne Twister pseudo-random generator
		// handle range, half open right here, closed inclusive must be handled
		// separately through "upper" param sent
		std::uniform_real_distribution<double> dist(lower, upper);
		return dist(mt);
	}

	/*!*****************************************************************************
	\brief
		Float randomizer

	\param[in] lower
		float as lower limit

	\param[in] y
		float as upper limit

	\return
		randomized float
	*******************************************************************************/
	float rand(float lower, float upper) {
		if (lower == upper) return lower;

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<float> dist(lower, upper);
		return dist(mt);
	}


	/*!*****************************************************************************
	\brief
		Int randomizer

	\param[in] lower
		int as lower limit

	\param[in] y
		int as upper limit

	\return
		randomized int
	*******************************************************************************/
	int rand(int lower, int upper) {
		if (lower == upper) return lower;

		std::random_device rd;
		std::mt19937 mt(rd()); // use Mersenne Twister pseudo-random generator
		// handle range, closed inclusive
		std::uniform_int_distribution<int> dist(lower, upper);
		return dist(mt);
	}
}