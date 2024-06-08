#include "scene.hpp"


using namespace cgp;



// This function is called only once at the beginning of the program
// This function can contain any complex operation that can be pre-computed once
void scene_structure::initialize()
{
	std::cout << "Start function scene_structure::initialize()" << std::endl;

	// Set the behavior of the camera and its initial position
	// ********************************************** //
	camera_projection.field_of_view = 50.0f * Pi / 180;
	camera_control.initialize(inputs, window); 
	camera_control.set_rotation_axis_z(); // camera rotates around z-axis
	//   look_at(camera_position, targeted_point, up_direction)
	camera_control.look_at(
		{ 100.0f, 100.0f, 15.0f } /* position of the camera in the 3D scene */,
		{0,0,0} /* targeted point in 3D scene */,
		{0,0,1} /* direction of the "up" vector */);


	// Create the global (x,y,z) frame
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());


	// Create the shapes seen in the 3D scene
	// ********************************************** //

	
	terrain_parameters parameters;
	L = parameters.terrain_length;
	N = parameters.terrain_sample;
	mesh terrain_mesh = create_terrain_mesh(L, N);
	deform_terrain(terrain_mesh,parameters);
	terrain_position = terrain_mesh.position;
	terrain_normal = terrain_mesh.normal;
	//storing position and other parameteres so as to access z coordinate in the loop


	// load the blade of grass used for the instancing
	grass.initialize_data_on_gpu(mesh_primitive_quadrangle({ -0.5f,0.0f,0.0f }, { 0.5f,0.0f,0.0f }, { 0.5f,0.0f,1.0f }, { -0.5f,0.0f,1.0f }));
	grass.material.phong = { 1,0,0,1 };
	grass.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/grass.png");
	

	// add a per-instance vertex attribute
	numarray<vec2> instance_colors(gui.max_number_of_instances);
	numarray<vec3> instance_positions(gui.max_number_of_instances);
	numarray<vec3> instance_orientation(gui.max_number_of_instances);
	for (int i = 0; i < instance_colors.size(); ++i) {
		float x = rand_interval(-L / 1.05f, L / 1.05f);
		float y = rand_interval(-L / 1.05f, L / 1.05f);
		//We calculate the index associated with the (x,y) coordinate of the grass in order to find its z coordinate
		int idx = std::round(N * (x + L) / (2 * L)) * N + std::round(N * (y + L) / (2 * L));
		float z = terrain_position[idx][2];
		vec3 normal = terrain_normal[idx];
		instance_positions[i] = {x,y,z};
		instance_colors[i] = { std::abs(2 * z / L),0.0f };
		instance_orientation[i] = normal;
	}
	grass.supplementary_texture["hue_texture"].load_and_initialize_texture_2d_on_gpu(project::path + "assets/grass_hue.jpg");
	grass.initialize_supplementary_data_on_gpu(instance_colors, /*location*/ 4, /*divisor: 1=per instance, 0=per vertex*/ 1);
	grass.initialize_supplementary_data_on_gpu(instance_positions, /*location*/ 5, /*divisor: 1=per instance, 0=per vertex*/ 1);


	//Define at random positions for palm trees, restricting them to the beach
	
	int j = 0;
	while (j < trees_nb) {
		float x = rand_interval(-L / 1.05f, L / 1.05f);
		float y = rand_interval(-L / 1.05f, L / 1.05f);
		//We calculate the index associated with the (x,y) coordinate of the palm tree in order to find its z coordinate
		int idx = std::round(N * (x + L) / (2 * L)) * N + std::round(N * (y + L) / (2 * L));
		float z = terrain_position[idx][2];
		if (z < -3 && z > -7) {
			trees_positions.push_back(vec3({ x,y,z }));
			j += 1;
		}
	}
	trees.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/palm_tree/palm_tree.obj"));
	trees.model.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, Pi / 2.0f);
	trees.model.scaling = 3;
	trees.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/palm_tree/palm_tree.jpg", GL_REPEAT, GL_REPEAT);

	terrain.initialize_data_on_gpu(terrain_mesh);
	terrain.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sand.jpg",GL_REPEAT,GL_REPEAT);

	//sea initialization
	float sea_lim1 = 10.0f;
	float sea_lim2 = 200.0f;
	float sea_z = -7.0f;
	water.initialize_data_on_gpu(mesh_primitive_grid({ sea_lim1,sea_lim1,sea_z }, { sea_lim2,sea_lim1 ,sea_z }, { sea_lim2,sea_lim2,sea_z }, { sea_lim1,sea_lim2,sea_z },100,100));
	water.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sea.png");
	water.material.alpha = 0.5;

	//car initialization
	car.initialize_data_on_gpu(mesh_primitive_cube({ 0,0,0 }, car_length / 2));
	ghost_car.initialize_data_on_gpu(mesh_primitive_cube({ 0,0,0 }, car_length / 2));

	//gate initialization
	gates = new Gate[16];
	vec3 gatepos[16] = { {-27,-34,0},{-49,-12,0},{-93,-13,0},{-140,-5,0}, {-149,44,0},{-126,111,0},{-33,172,0},{68,147,0},{147,156,0},{177,55,0},{129,-10,0},{62,-49,0},{34,-94,0},{0,-95,0},{-12,-84,0},{-25,63,0} };
	float gate_orientation[16] = { 0,30,60,90,135,0,30,60,90,135,0,30,60,90,135,0 };
	for (int i=0;i<16;i++){
	gates = new Gate[20];
	vec3 gatepos[20] = { {-27,-34,0},{-49,-12,0},{-93,-13,0},{-133,-7,0}, { -158,1,0 }, {-149,44,0},{-126,111,0},{-33,172,0},{68,147,0},{147,156,0},{177,55,0},{129,-10,0},{62,-49,0},{56,-60,0}, { 34,-94,0 },{18,-103,0}, { 0,-95,0 },{-12,-84,0}, {-25,-68,0},{-32,-52,0} };
	float gate_orientation[20] = { 0,215,60,45,195,315,0,30,105,180,135,90,90,135,285,90,90,315,0,0 };
	for (int i=0;i<20;i++){
		gates[i].initialize(gatepos[i], gate_orientation[i]);
	}
	//car.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/palm_tree/KART-OBJ"));




	/*cube1.initialize_data_on_gpu(mesh_primitive_cube({ 0,0,0 }, 0.5f));
	cube1.model.rotation = rotation_transform::from_axis_angle({ -1,1,0 }, Pi / 7.0f);
	cube1.model.translation = { 1.0f,1.0f,-0.1f };
	cube1.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/wood.jpg");

	cube2 = cube1;*/

	//set shaders
	opengl_shader_structure water_shader;
	water_shader.load(
		project::path + "shaders/water/water.vert.glsl",
		project::path + "shaders/water/water.frag.glsl");
	water.shader = water_shader;

	// to use correctly the instancing, we will need a specific shader able to treat differently each instance of the grass
	grass.shader.load(project::path + "shaders/instancing/instancing.vert.glsl",
					  project::path + "shaders/instancing/instancing.frag.glsl");
}


// This function is called permanently at every new frame
// Note that you should avoid having costly computation and large allocation defined there. This function is mostly used to call the draw() functions on pre-existing data.
void scene_structure::display_frame()
{
	//set the uniform parameters
	environment.uniform_generic.uniform_float["time"] = timer.t;

	//Choose between orbital and POV view in the GUI
	if (!gui.pov && !gui.pov_race) {
		environment.camera_view = camera_control.camera_model.matrix_view();
		camera_projection.field_of_view = 50.0f * Pi / 180;
		// Re-orient the grass shape to always face the camera direction
		auto const& camera = camera_control.camera_model;
		vec3 const right = camera.right();
		// Rotation such that the grass follows the right-vector of the camera, while pointing toward the z-direction
		rotation_transform R = rotation_transform::from_frame_transform({ 1,0,0 }, { 0,0,1 }, right, { 0,0,1 });
		grass.model.rotation = R;
	}
	else {
		environment.camera_view = mat4::build_translation(0, 0, -2.5f) * mat4::build_rotation_from_axis_angle({-1, 0, 0}, 1.3f) * mat4::build_rotation_from_axis_angle({0,0,-1}, -Pi / 2) * (inverse(car.model.rotation) * mat4::build_translation(-car.model.translation));
		
		//camera is pulled away from the car as it speeds up
		camera_projection.field_of_view = 50.0f * (1 + 2 * v) * Pi / 180;

		rotation_transform R = rotation_transform::from_axis_angle(vec3( 0,0,1 ), Pi / 2) * car.model.rotation;
		grass.model.rotation = R;
	}

	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();

	// Update time
	timer.update();


	// Draw all the shapes
	draw(terrain, environment);
	for (int i = 0; i < 20; i++) {
		gates[i].draw(environment);
	}
	/*draw(tree, environment);
	draw(cube1, environment);*/
	

	// Draw the instances of grass: the third parameter is the number of instances to display
	draw(grass, environment, gui.number_of_instances);

	// Draw instances of palm trees on the beach
	for (int i = 0; i < trees_nb; i++) {
		trees.model.translation = trees_positions[i];
		draw(trees, environment);
	}



	//Animate car with QWERTY keyboard
		
		//We start by computing and keep in memory the axis of the car so as to limit computations since this value will be used several times

	car_xaxis = car.model.rotation.rotation_transform::matrix_col_x();
	car_yaxis = car.model.rotation.rotation_transform::matrix_col_y();
	car_zaxis = car.model.rotation.rotation_transform::matrix_col_z();

	if (std::abs(v) < 0.0001f) { v_rch = 0;}
	if (!(gui.pov_race && chrono < 0)){
		if (inputs.keyboard.is_pressed(GLFW_KEY_W) && !inputs.keyboard.is_pressed(GLFW_KEY_S)) {
			if (car_status != "forth") {
				t_0 = timer.t;
				v_rch = v;
				car_status = "forth";
			}
			v = v_max * (1 - (1 - v_rch / v_max) * exp(-alpha * (timer.t - t_0)));
		}
		else if (inputs.keyboard.is_pressed(GLFW_KEY_S) && !inputs.keyboard.is_pressed(GLFW_KEY_W)) {
			if (car_status != "back") {
				t_0 = timer.t;
				v_rch = v;
				car_status = "back";
			}
			v = -v_max * (1 - (1 + v_rch / v_max) * exp(-alpha * (timer.t - t_0)));
		}
		else {
			if (car_status != "still") {
				t_0 = timer.t;
				v_rch = v;
				car_status = "still";
			}
			v = v_rch * exp(-beta * (timer.t - t_0));
		}
	}
	
	car.model.translation += v * (cos(theta_point) * car_xaxis + sin(theta_point) * car_yaxis);

	car_frontwheel = car.model.translation + (car_length * car_xaxis - car_height * car_zaxis) / 2;
	car_backleftwheel = car.model.translation - (car_width * car_yaxis + car_length * car_xaxis + car_height * car_zaxis) / 2;
	car_backrightwheel = car.model.translation + (car_width * car_yaxis - car_length * car_xaxis - car_height * car_zaxis) / 2;

		//update height with terrain
	car_frontwheel[2] = evaluate_terrain_height(car_frontwheel[0] / 2, car_frontwheel[1] / 2);
	car_backleftwheel[2] = evaluate_terrain_height(car_backleftwheel[0] / 2, car_backleftwheel[1] / 2);
	car_backrightwheel[2] = evaluate_terrain_height(car_backrightwheel[0] / 2, car_backrightwheel[1] / 2);
	car_zaxis = cross(car_backrightwheel - car_frontwheel, car_backleftwheel - car_frontwheel);
	car_zaxis = car_zaxis / norm(car_zaxis);

	car.model.rotation = rotation_transform::from_vector_transform({ 0,0,1 }, car_zaxis);

	//update
	car_xaxis = car.model.rotation.rotation_transform::matrix_col_x();
	car_yaxis = car.model.rotation.rotation_transform::matrix_col_y();
	car_zaxis = car.model.rotation.rotation_transform::matrix_col_z();
	car_backleftwheel = car.model.translation - (car_width * car_yaxis + car_length * car_xaxis + car_height * car_zaxis) / 2;
	car_backrightwheel = car.model.translation + (car_width * car_yaxis - car_length * car_xaxis - car_height * car_zaxis) / 2;


	//calculate center displacement of car compared to center of the backwheel so that to give the correct height to the car
	car.model.translation[2] = evaluate_terrain_height((car_backleftwheel[0] + car_backrightwheel[0])/ 4, (car_backleftwheel[1] + car_backrightwheel[1])/ 4) + dot(car_length * car_xaxis + car_height * car_zaxis, vec3(0,0,1)) / 2;

	theta_point = 0;

	if (inputs.keyboard.is_pressed(GLFW_KEY_A)) {
		theta_point = angle;
		theta += theta_point * v / car_length;		
	}

	if (inputs.keyboard.is_pressed(GLFW_KEY_D)) {
		theta_point = - angle;
		theta += theta_point * v / car_length;
	}
	car.model.rotation = rotation_transform::convert_axis_angle_to_quaternion(car_zaxis, theta) * car.model.rotation;
	
	draw(car, environment);

		//Animate race mode
	if (gui.pov_race) {
		if (!race_init) {
			race_init = true;
			t_start = timer.t + 5.0f;
			gate_count = 0;
			car.model.translation = { x_start, y_start, evaluate_terrain_height(x_start / 2, y_start / 2) };
			theta = theta_start;
			v = 0;
			v_rch = 0;
			car_status = "still";
		}
		chrono = timer.t - t_start;
		ImGui::Text("Chrono de la course : %.2f", chrono);
		std::cout << car.model.translation.x << "    " << car.model.translation.y << "     " << evaluate_terrain_height(car.model.translation.x / 2, car.model.translation.y / 2) << std::endl;
		current_path.push_back(car.model);
		if (best_exist) {
			ImGui::Text("Meilleur temps : %.2f", best_time);
			ghost_car.model = best_path[frame_count];
			frame_count++;
		}
	}
	else {
		race_init = false;
	}
	
	// Animate the second cube in the water
	/*cube2.model.translation = { -1.0f, 6.0f+0.1*sin(0.5f*timer.t), -0.8f + 0.1f * cos(0.5f * timer.t)};
	cube2.model.rotation = rotation_transform::from_axis_angle({1,-0.2,0},Pi/12.0f*sin(0.5f*timer.t));
	draw(cube2, environment);*/

	if (gui.display_wireframe) {
		draw_wireframe(terrain, environment);
		draw_wireframe(grass, environment, { 0,0,1 }, gui.number_of_instances);
		/*draw_wireframe(water, environment);
		draw_wireframe(tree, environment);
		draw_wireframe(cube1, environment);
		draw_wireframe(cube2, environment);*/
	}

	// conditional display of the global frame (set via the GUI)
	if (gui.display_frame)
		draw(global_frame, environment);

	
	// We display the semi-transparent shapes after the non-transparent ones 
	//   (as semi-transparent shapes are not associated to depth buffer write)
	display_semiTransparent();





}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
	ImGui::Checkbox("POV - open_world", &gui.pov);
	ImGui::Checkbox("POV - race", &gui.pov_race);

	// Control the number of instances
	ImGui::SliderInt("Instances", &gui.number_of_instances, 0, gui.max_number_of_instances);
	

}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}

void scene_structure::display_semiTransparent()
{
	// Enable use of alpha component as color blending for transparent elements
	//  alpha = current_color.alpha
	//  new color = previous_color * alpha + current_color * (1-alpha)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Disable depth buffer writing
	//  - Transparent elements cannot use depth buffer
	//  - They are supposed to be display from furest to nearest elements
	glDepthMask(false);

	


	// Sort transparent shapes by depth to camera
	//   This step can be skipped, but it will be associated to visual artifacts

	// Transform matrix (the same matrix which is applied in the vertices in the shader: T = Projection x View)
	//mat4 T = camera_projection.matrix() * camera.matrix_view();
	//// Projected vertices (center of quads) in homogeneous coordinates
	//vec4 p1 = T * vec4{ 0, -0.5f, 0, 1 };
	//vec4 p2 = T * vec4{ 0, +0.5f, 0, 1 };
	//// Depth to camera
	//float z1 = p1.z / p1.w;
	//float z2 = p2.z / p2.w;

	//// Display the quads relative to their depth
	//if (z1 <= z2) {
	//	draw(quad_2, environment);
	//	draw(quad_1, environment);
	//}
	//else {
	//	draw(quad_1, environment);
	//	draw(quad_2, environment);
	//}
	draw(water, environment);
	

	if (gui.display_wireframe) {
		draw_wireframe(water, environment);
	}

	// Don't forget to re-activate the depth-buffer write
	glDepthMask(true);
	glDisable(GL_BLEND);
}
