#pragma once

#include "cgp/cgp.hpp"

//Variables associated to Perlin noise
struct perlin_noise_parameters {
	float persistency = 0.8f;
	float frequency_gain = 1.9f;
	int octave = 4;
	float terrain_height = 1.0f;
};


// Initialize the mesh of the terrain
cgp::mesh create_terrain_mesh();

void deform_terrain(cgp::mesh& m, perlin_noise_parameters parameters);
