
#include "cgp/cgp.hpp"
#include "terrain.hpp"

using namespace cgp;

struct Gate {
	float width = 2.0f;
	mesh_drawable left;
	mesh_drawable right;

	void initialize(vec3 pos, float orientation);
	void draw(environment_generic_structure const& environment);
	bool is_reached(vec3 car_pos, vec3 old_car_pos);
};