#pragma once

#include "cgp/cgp.hpp"

//Variables associated to Perlin noise
struct terrain_parameters {
	//Basic terrai parameters
	float terrain_length = 200.0f;
	int terrain_sample = 2000;
	
	//Perlin noise parameters
	float persistency = 0.8f;
	float frequency_gain = 1.9f;
	int octave = 4;
	float terrain_height = 1.0f;
};




// Initialize the mesh of the terrain
cgp::mesh create_terrain_mesh(float& L, int& terrain_sample);

void deform_terrain(cgp::mesh& m, terrain_parameters parameters);
