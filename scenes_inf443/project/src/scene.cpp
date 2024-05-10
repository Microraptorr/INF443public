#include "scene.hpp"


using namespace cgp;



// This function is called only once at the beginning of the program
// This function can contain any complex operation that can be pre-computed once
void scene_structure::initialize()
{
	std::cout << "Start function scene_structure::initialize()" << std::endl;

	// Set the behavior of the camera and its initial position
	// ********************************************** //
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

	terrain.initialize_data_on_gpu(terrain_mesh);
	terrain.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sand.jpg",GL_REPEAT,GL_REPEAT);

	float sea_lim1 = 10.0f;
	float sea_lim2 = 200.0f;
	float sea_z = -7.0f;
	water.initialize_data_on_gpu(mesh_primitive_grid({ sea_lim1,sea_lim1,sea_z }, { sea_lim2,sea_lim1 ,sea_z }, { sea_lim2,sea_lim2,sea_z }, { sea_lim1,sea_lim2,sea_z },100,100));
	water.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sea.png");
	water.material.alpha = 0.5;



	/*tree.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/palm_tree/palm_tree.obj"));
	tree.model.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, Pi / 2.0f);
	tree.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/palm_tree/palm_tree.jpg", GL_REPEAT, GL_REPEAT);*/

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

	car.initialize_data_on_gpu(mesh_primitive_cube({ 0,0,0 }, car_length / 2));
}


// This function is called permanently at every new frame
// Note that you should avoid having costly computation and large allocation defined there. This function is mostly used to call the draw() functions on pre-existing data.
void scene_structure::display_frame()
{
	//set the uniform parameters
	environment.uniform_generic.uniform_float["time"] = timer.t;
	//Choose between orbital and POV view in the GUI
	if (!gui.pov) environment.camera_view = camera_control.camera_model.matrix_view();
	else {
		environment.camera_view = mat4::build_translation(0, 0, -3) * mat4::build_rotation_from_axis_angle({-1, 0, 0}, 1.4f) * mat4::build_rotation_from_axis_angle({0,0,-1}, -Pi / 2) * (inverse(car.model.rotation) * mat4::build_translation(-car.model.translation));
	}

	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();

	// Update time
	timer.update();

	// conditional display of the global frame (set via the GUI)
	if (gui.display_frame)
		draw(global_frame, environment);
	

	// Draw all the shapes
	draw(terrain, environment);
	/*draw(tree, environment);
	draw(cube1, environment);*/

	//Animate car with QWERTY keyboard
	if (inputs.keyboard.is_pressed(GLFW_KEY_W)) {
		car.model.translation += speed * (cos(theta_point) * car.model.rotation.rotation_transform::matrix_col_x() + sin(theta_point) * car.model.rotation.rotation_transform::matrix_col_y());
	}
	if (inputs.keyboard.is_pressed(GLFW_KEY_S)) {
		car.model.translation -= speed * (cos(theta_point) * car.model.rotation.rotation_transform::matrix_col_x() + sin(theta_point) * car.model.rotation.rotation_transform::matrix_col_y());
	}

	//We calculate the index associated with the (x,y) coordinate of the car in order to find the z coordinate and the associated normal vector
	int idx = std::round(N * (car.model.translation[0] + L) / (2 * L)) * N + std::round(N * (car.model.translation[1] + L) / (2 * L));
	car.model.rotation = rotation_transform::from_vector_transform({ 0,0,1 }, terrain_normal[idx]);
	car.model.translation[2] = terrain_position[idx][2] + car_length / 2;
	theta_point = 0;

	if (inputs.keyboard.is_pressed(GLFW_KEY_A)) {
		theta_point = angle;
		theta += theta_point * speed / car_length;		
	}

	if (inputs.keyboard.is_pressed(GLFW_KEY_D)) {
		theta_point = - angle;
		theta += theta_point * speed / car_length;
	}
	car.model.rotation = rotation_transform::convert_axis_angle_to_quaternion(car.model.rotation.rotation_transform::matrix_col_z(), theta) * car.model.rotation;
	

	

	draw(car, environment);
	
	// Animate the second cube in the water
	/*cube2.model.translation = { -1.0f, 6.0f+0.1*sin(0.5f*timer.t), -0.8f + 0.1f * cos(0.5f * timer.t)};
	cube2.model.rotation = rotation_transform::from_axis_angle({1,-0.2,0},Pi/12.0f*sin(0.5f*timer.t));
	draw(cube2, environment);*/

	if (gui.display_wireframe) {
		draw_wireframe(terrain, environment);
		/*draw_wireframe(water, environment);
		draw_wireframe(tree, environment);
		draw_wireframe(cube1, environment);
		draw_wireframe(cube2, environment);*/
	}
	
	// We display the semi-transparent shapes after the non-transparent ones 
	//   (as semi-transparent shapes are not associated to depth buffer write)
	display_semiTransparent();





}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
	ImGui::Checkbox("POV", &gui.pov);

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

	auto const& camera = camera_control.camera_model;

	// Re-orient the grass shape to always face the camera direction
	//vec3 const right = camera.right();
	//// Rotation such that the grass follows the right-vector of the camera, while pointing toward the z-direction
	//rotation_transform R = rotation_transform::from_frame_transform({ 1,0,0 }, { 0,0,1 }, right, { 0,0,1 });
	//quad_1.model.rotation = R;


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
