#version 400
precision highp float;

in vec2 vertexPosition;

out vec2 fragCoord;

void main()
{
	gl_Position = vec4(vertexPosition, 0.0, 1.0);
	fragCoord = vertexPosition;
}