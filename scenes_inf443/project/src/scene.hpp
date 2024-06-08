#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"
#include "terrain.hpp"

// This definitions allow to use the structures: mesh, mesh_drawable, etc. without mentionning explicitly cgp::
using cgp::mesh;
using cgp::mesh_drawable;
using cgp::vec3;
using cgp::numarray;
using cgp::timer_basic;

// Variables associated to the GUI (buttons, etc)
struct gui_parameters {
	bool display_frame = true;
	bool display_wireframe = false;
	bool pov = false;
	bool pov_race;
	int number_of_instances = 500;
	int max_number_of_instances = 200000;
};



// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {

	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit_euler camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage

	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	timer_basic timer;

	mesh_drawable terrain;
	numarray<vec3> terrain_position;
	numarray<vec3> terrain_normal;
	float L;
	int N;

	mesh_drawable water;
	mesh_drawable grass;
	mesh_drawable car;
	mesh_drawable trees;
	const int trees_nb = 15;
	numarray<vec3> trees_positions;


		//parameters used to animate the car
	float v = 0.0f; //current speed
	float v_max = 0.25f; //max speed
	float v_rch = 0; //speed reached
	float alpha = 0.4f; //coefficient for exponential acceleration
	float beta = 1.0f; //coefficient for exponential decceleration with engine brake
	float t_0 = 0.0f; //initial timecode of a give phase of acceleration
	float angle = 0.2f; //The absolute angle which the wheels take when we turn
	float theta = 0.0f; //Angle of the car along the z-axis
	float theta_point;//Equals 0, angle or -angle depending on whether we're turning
	std::string car_status = "still"; //keeps the current phase of the car acceleration "still, forth, back" in memory
	//postions and axis used for computations
	vec3 car_xaxis;
	vec3 car_yaxis;
	vec3 car_zaxis;
	vec3 car_frontwheel;
	vec3 car_backleftwheel;
	vec3 car_backrightwheel;

	float car_length = 1.0f;
	float car_width = car_length;
	float car_height = car_length;

		//parameters used to control race mode
	bool race_init = false;
	float t_start;
	float chrono;
	std::vector<affine> current_path;
	std::vector<affine> best_path;
	




	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop


	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();
	void display_semiTransparent();


};





