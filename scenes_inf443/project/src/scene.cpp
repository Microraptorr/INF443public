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
		{ 5.0f, -4.0f, 3.5f } /* position of the camera in the 3D scene */,
		{0,0,0} /* targeted point in 3D scene */,
		{0,0,1} /* direction of the "up" vector */);


	// Create the global (x,y,z) frame
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());


	// Create the shapes seen in the 3D scene
	// ********************************************** //

	
	perlin_noise_parameters parameters;
	mesh terrain_mesh = create_terrain_mesh();
	deform_terrain(terrain_mesh,parameters);
	terrain.initialize_data_on_gpu(terrain_mesh);
	/*terrain.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sand.jpg");*/

	/*float sea_w = 8.0;
	float sea_z = -0.8f;
	water.initialize_data_on_gpu(mesh_primitive_grid({ -sea_w,-sea_w,sea_z }, { sea_w,-sea_w,sea_z }, { sea_w,sea_w,sea_z }, { -sea_w,sea_w,sea_z }));
	water.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sea.png");*/

	/*tree.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/palm_tree/palm_tree.obj"));
	tree.model.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, Pi / 2.0f);
	tree.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/palm_tree/palm_tree.jpg", GL_REPEAT, GL_REPEAT);*/

	/*cube1.initialize_data_on_gpu(mesh_primitive_cube({ 0,0,0 }, 0.5f));
	cube1.model.rotation = rotation_transform::from_axis_angle({ -1,1,0 }, Pi / 7.0f);
	cube1.model.translation = { 1.0f,1.0f,-0.1f };
	cube1.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/wood.jpg");

	cube2 = cube1;*/

	car.initialize_data_on_gpu(mesh_primitive_cube({ 0,0,0 }, car_length / 2));
}


// This function is called permanently at every new frame
// Note that you should avoid having costly computation and large allocation defined there. This function is mostly used to call the draw() functions on pre-existing data.
void scene_structure::display_frame()
{

	//Choose between orbital and POV view in the GUI
	if (!gui.pov) environment.camera_view = camera_control.camera_model.matrix_view();
	else {
		environment.camera_view = mat4::build_translation(0, 0, -3) * mat4::build_rotation_from_axis_angle({-1, 0, 0}, 1) * mat4::build_rotation_from_axis_angle({0,0,-1}, -Pi / 2) * (inverse(car.model.rotation) * mat4::build_translation(-car.model.translation));
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
	/*draw(water, environment);
	draw(tree, environment);
	draw(cube1, environment);*/

	//Animate car with QWERTY keyboard
	theta = 0;
	if (inputs.keyboard.is_pressed(GLFW_KEY_A)) {
		theta = angle;
		car.model.rotation *= rotation_transform::convert_axis_angle_to_quaternion(car.model.rotation.rotation_transform::matrix_col_z(), theta * speed / car_length);
	}
	if (inputs.keyboard.is_pressed(GLFW_KEY_D)) {
		theta = - angle;
		car.model.rotation *= rotation_transform::convert_axis_angle_to_quaternion(car.model.rotation.rotation_transform::matrix_col_z(), theta * speed / car_length);
	}
	if (inputs.keyboard.is_pressed(GLFW_KEY_W)) {
		car.model.translation += speed * (cos(theta) * car.model.rotation.rotation_transform::matrix_col_x() + sin(theta) * car.model.rotation.rotation_transform::matrix_col_y());
	}
	if (inputs.keyboard.is_pressed(GLFW_KEY_S)) {
		car.model.translation -= speed * (cos(theta) * car.model.rotation.rotation_transform::matrix_col_x() + sin(theta) * car.model.rotation.rotation_transform::matrix_col_y());
	}


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

