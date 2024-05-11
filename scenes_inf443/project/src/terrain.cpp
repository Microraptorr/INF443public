
#include "terrain.hpp"

using namespace cgp;



mesh create_terrain_mesh(float& L, int& terrain_sample)
{
	mesh terrain_mesh = mesh_primitive_grid({ -L,-L,0 }, { L,-L,0 }, { L,L,0 }, { -L,L,0 }, terrain_sample, terrain_sample);
	return terrain_mesh;
}
//get texture coordinates with a 45� rotation)
//vec2 get_texture_uv(float x, float y) {
//	float r = std::sqrt(2);
//	//scaling of the coordinates
//	x = x / 200 * r;
//	y = y / 200 * r;
//	//rotation
//	float xprime = r / 2 * x - r / 2 * y;
//	float yprime = r / 2 * x + r / 2 * y;
//	//translation
//	xprime += 0.5f;
//	yprime += 0.5f;
//	return { xprime,yprime };
//}

float evaluate_terrain_height(float x, float y) {
	vec2 p_i[23] = { {-100, 0}, {-88, -25}, {-76, -40}, {-64, -55}, {-52, -65}, {-40, -60}, {-28, -45}, {-16, -30}, {-4, -15}, {8, -30}, {20, -45}, {32, -60},{37, 33.5}, {44, 37}, {51, 40.5}, {58, 44}, {65, 47.5}, {72, 51}, {79, 54.5}, {86, 58}, {93, 61.5}, {100, 65},{100,100}

	};
	float h_i[23] = { 19.72f, 36.65f, 12.59f, 45.96f, 4.29f, 39.53f, 18.37f, 29.79f, 8.36f, 43.83f, 2.54f, 27.96f,-1.7f, -1.2f, -0.8f, -2.0f, -1.4f, -3.0f, -2.1f, -1.3f, -1.9f, -1.3f,-40.0f


	};
	float sigma_i[23] = { 21.15f, 19.32f, 10.68f, 21.66f, 12.42f, 24.78f, 17.76f, 23.94f, 11.04f, 19.92f, 7.74f, 19.02f,75.0f, 36.0f, 93.0f, 45.0f, 120.0f, 105.0f, 84.0f, 126.0f, 51.0f, 117.0f,20.0f

	};

	float z = 0;
	for (int i = 0; i < 23; i++) {
		float d = norm(vec2(x, y) - p_i[i]) / (3.0*sigma_i[i]);
		z += h_i[i]/3.5 * std::exp(-d * d);
	}
	return z;
}

void deform_terrain(mesh& m, terrain_parameters parameters)
{
	int const N = parameters.terrain_sample;
	float const L = parameters.terrain_length;

	m.uv.resize(N * N);

	// Recompute the new vertices
	for (int ku = 0; ku < N; ++ku) {
		for (int kv = 0; kv < N; ++kv) {

			// Compute local parametric coordinates (u,v) \in [0,1]
			const float u = ku / (N - 1.0f);
			const float v = kv / (N - 1.0f);

			int const idx = ku * N + kv;

			// Compute the Perlin noise
			float const noise = noise_perlin({ 2*u, 2*v }, parameters.octave, parameters.persistency, parameters.frequency_gain);

			//Compute coordinates for terrain height calculation
			float x = ku * L / N - L/2;
			float y = kv * L / N - L / 2;

			// use the noise as height value
			float alt=parameters.terrain_height*noise*evaluate_terrain_height(x, y);
			m.position[idx].z = alt;

			// use also the noise as color valuex
			if (alt > -3) m.color[idx] = 0.7f * vec3(0, 0.5f, 0) + 0.8f * alt / 45.0 * vec3(1, 1, 1);
			else if (alt > -7 && alt < -3) m.color[idx] = 2.5*vec3(0.231, 0.224, 0.109);
			else m.color[idx] = 0.5f*vec3(1,1,1);

			//set texture 
			m.uv[kv + N * ku] = { u,v };
		}
	}
	m.normal_update();
}

