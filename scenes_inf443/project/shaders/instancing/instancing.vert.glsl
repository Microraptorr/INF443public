#version 330 core

// Vertex shader - this code is executed for every vertex of the shape

// Inputs coming from VBOs
layout (location = 0) in vec3 vertex_position; // vertex position in local space (x,y,z)
layout (location = 1) in vec3 vertex_normal;   // vertex normal in local space   (nx,ny,nz)
layout (location = 2) in vec3 vertex_color;    // vertex color      (r,g,b)
layout (location = 3) in vec2 vertex_uv;       // vertex uv-texture (u,v)
layout(location = 4) in vec2 hue;
layout (location = 5) in vec3 instance_position;//instance position (x,y,z)

// Output variables sent to the fragment shader
out struct fragment_data
{
    vec3 position; // vertex position in world space
    vec3 normal;   // normal position in world space
    vec3 color;    // vertex color
    vec2 uv;       // vertex uv
	vec2 hue;//color to apply to the billboard according to its altitude
} fragment;

// Uniform variables expected to receive from the C++ program
uniform mat4 model; // Model affine transform matrix associated to the current shape
uniform mat4 view;  // View matrix (rigid transform) of the camera
uniform mat4 projection; // Projection (perspective or orthogonal) matrix of the camera



uniform float time;


// Generate a 3x3 rotation matrix around the z-axis
mat3 rotation_z(in float angle) {
	return mat3(vec3(cos(angle), -sin(angle), 0.0),
		vec3(sin(angle), cos(angle), 0.0),
		vec3(0.0, 0.0, 1.0));
}



// Procedural wind-like deformation
vec3 wind(in vec3 p,in float t) {
	return vec3(0.2*cos(t-p.x), 0.1*sin(0.5*t - 1.2*p.y), 0.0);
}


void main()
{
	// The position of the vertex in the world space
	vec4 position = model * vec4(vertex_position, 1.0);

	//position.xyz = position.xyz + offset; // offset of the instance
	position.xyz = position.xyz + instance_position;
	position.xyz = position.xyz + (position.z-instance_position.z) * wind((position.xyz-instance_position), time); // procedural deformation modeling the wind effect (we scale the deformation along z such that only the tips of the blades are moving while the root remains fixed).

	// The normal of the vertex in the world space
	mat4 modelNormal = transpose(inverse(model));
	vec4 normal = modelNormal*vec4(vertex_normal, 0.0);


	// The projected position of the vertex in the normalized device coordinates:
	vec4 position_projected = projection * view * position;

	// Fill the parameters sent to the fragment shader
	fragment.position = position.xyz;
	fragment.normal   = normal.xyz;
	fragment.color = vertex_color;
	fragment.uv = vertex_uv;
	fragment.hue=hue;

	// gl_Position is a built-in variable which is the expected output of the vertex shader
	gl_Position = position_projected; // gl_Position is the projected vertex position (in normalized device coordinates)
}
