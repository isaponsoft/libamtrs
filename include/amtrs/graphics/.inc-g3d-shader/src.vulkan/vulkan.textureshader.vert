#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform UniformBufferObject
{
    mat4	camera;
    mat4	world;
	vec4	color;
} ubo;


layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 col;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragUv;

void main()
{
   gl_Position  = ubo.camera * ubo.world * pos;
   fragColor	= ubo.color * col;
   fragUv   	= uv;
}