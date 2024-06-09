
#include "Gate.hpp"
using namespace cgp;



void Gate::initialize(vec3 pos,float orientation, bool is_flipped) {


	left.initialize_data_on_gpu(mesh_primitive_cylinder(0.15f,{0,0,0},{0,0,5.0f}));
	right.initialize_data_on_gpu(mesh_primitive_cylinder(0.15f, { 0,0,0 }, { 0,0,5.0f }));


	left.material.color = { 1,0,0 };
	right.material.color = { 0,0,1 };

	flipped = is_flipped;

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

bool Gate::is_reached(vec3 car_pos,vec3 old_car_pos) {
	float a = (right.model.translation.y - left.model.translation.y) / (right.model.translation.x - left.model.translation.x);
	float b = right.model.translation.y - a * right.model.translation.x;
	float epsilon = 0.25f; // tolerance when checking when it is in between because sometimes, the gates share the same x or the same y
	bool infinite_line_crossed_nonflipped = (car_pos.y < a * car_pos.x + b) && (old_car_pos.y >= a * old_car_pos.x + b);
	bool infinite_line_crossed_flipped = (car_pos.y > a * car_pos.x + b) && (old_car_pos.y <= a * old_car_pos.x + b);
	bool is_between_x = std::min(right.model.translation.x, left.model.translation.x) - epsilon < car_pos.x && car_pos.x < std::max(right.model.translation.x, left.model.translation.x) + epsilon;
	bool is_between_y = std::min(right.model.translation.y, left.model.translation.y) - epsilon < car_pos.y && car_pos.y < std::max(right.model.translation.y, left.model.translation.y) + epsilon;
	if (flipped) return(infinite_line_crossed_flipped&& is_between_x&& is_between_y);
	else return(infinite_line_crossed_nonflipped && is_between_x && is_between_y);
}