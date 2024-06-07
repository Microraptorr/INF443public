
#include "Gate.hpp"
using namespace cgp;



void Gate::initialize(vec3 pos,float orientation) {


	left.initialize_data_on_gpu(mesh_primitive_cylinder(0.15f,{0,0,0},{0,0,5.0f}));
	right.initialize_data_on_gpu(mesh_primitive_cylinder(0.15f, { 0,0,0 }, { 0,0,5.0f }));


	left.material.color = { 1,0,0 };
	right.material.color = { 0,0,1 };

	

	vec3 rot = { width/2.0f * std::cos(orientation),width/2.0f * std::sin(orientation),0};
	right.model.translation = pos+rot;
	left.model.translation = pos-rot;
	left.model.translation.z = evaluate_terrain_height(left.model.translation.x/2.0f, left.model.translation.y/2.0f);
	right.model.translation.z =evaluate_terrain_height(right.model.translation.x/2.0f, right.model.translation.y/2.0f);
}

void Gate::draw(environment_generic_structure const& environment) {
	cgp::draw(left, environment);
	cgp::draw(right, environment);
}