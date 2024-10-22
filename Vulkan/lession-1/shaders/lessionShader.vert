#version 450

#extension GL_ARB_separate_shader_objects:enable
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location=0) out vec3 outColor;

layout(binding = 0) uniform VPMatrices{
	mat4 mViewMatrix;
	mat4 mProjectionMatrix;
}vpUBO;

layout(binding=1) uniform ObjectUniform{
	mat4 mModelMatrix;
}ObjectUBO;

//vec2 positions[3] = vec2[](vec2(0.0,-1.0),vec2(0.5,0.0),vec2(-0.5,0.0));

//vec3 colors[3] = vec3[](vec3(1.0,0.0,0.0),vec3(0.0,1.0,0.0),vec3(0.0,0.0,1.0));

void main(){
	gl_Position = vpUBO.mProjectionMatrix * vpUBO.mViewMatrix * ObjectUBO.mModelMatrix * vec4(inPosition,1.0);

	outColor = inColor;
}