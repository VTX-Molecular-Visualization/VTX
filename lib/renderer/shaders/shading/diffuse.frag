#version 460 core

#include "common.glsl"

float computeLighting( const UnpackedData p_data, const vec3 p_lightDirection )
{ return max( dot( p_data.normal, p_lightDirection ), 0.f ); }

#include "main.glsl"
