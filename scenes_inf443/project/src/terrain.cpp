
#include "terrain.hpp"

using namespace cgp;

mesh create_terrain_mesh()
{
	int const terrain_sample = 180;
	mesh terrain = mesh_primitive_grid({ -1,-1,0 }, { 1,-1,0 }, { 1,1,0 }, { -1,1,0 }, terrain_sample, terrain_sample);
	return terrain;
}

float evaluate_terrain_height(float x, float y) {
	vec2 p_i[4] = { {-10,-10}, {5,5}, {-3,4}, {6,4} };
	float h_i[4] = { 3.0f, -1.5f, 1.0f, 2.0f };
	float sigma_i[4] = { 10.0f,3.0f,4.0f,4.0f };

	float z = 0;
	for (int i = 0; i < 4; i++) {
		float d = norm(vec2(x, y) - p_i[i]) / sigma_i[i];
		z += h_i[i] * std::exp(-d * d);
	}
	return z;
}

void deform_terrain(mesh& m, perlin_noise_parameters parameters)
{
	//Number of samples assuming the grid is a square
	int const N = std::sqrt(m.position.size());

	// Recompute the new vertices
	for (int ku = 0; ku < N; ++ku) {
		for (int kv = 0; kv < N; ++kv) {

			// Compute local parametric coordinates (u,v) \in [0,1]
			const float u = ku / (N - 1.0f);
			const float v = kv / (N - 1.0f);

			int const idx = ku * N + kv;

			// Compute the Perlin noise
			float const noise = noise_perlin({ u, v }, parameters.octave, parameters.persistency, parameters.frequency_gain);

			// use the noise as height value
			m.position[idx].z = parameters.terrain_height * noise*evaluate_terrain_height(ku/100,kv/100);

			// use also the noise as color valuex
			m.color[idx] = 0.3f * vec3(0, 0.5f, 0) + 0.7f * noise * vec3(1, 1, 1);
		}
	}
	m.normal_update();
}