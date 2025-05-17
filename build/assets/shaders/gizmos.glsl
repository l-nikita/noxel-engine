FragmentShader
[
	#version 330 core

	in vec2 texCoord;
	out vec4 FragColor;

	uniform vec3 color;

	void main() 
	{
		FragColor = vec4(color, 1.0);
	}
]

VertexShader
[
	#version 330 core
	layout (location = 0) in vec3 aPosition; // vertex coordinates

	// uniform variables
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main() 
	{
		gl_Position = projection * view * model * vec4(aPosition, 1.0);
	}
]