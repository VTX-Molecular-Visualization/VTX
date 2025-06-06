uint globalInvocationHash( uvec3 p_gl_GlobalInvocationID, uvec3 p_gl_NumWorkGroups, uvec3 p_gl_WorkGroupSize )
{
	return 
		p_gl_GlobalInvocationID.x +
		p_gl_GlobalInvocationID.y * p_gl_NumWorkGroups.x * p_gl_WorkGroupSize.x +
		p_gl_GlobalInvocationID.z * p_gl_NumWorkGroups.x * p_gl_NumWorkGroups.y * p_gl_WorkGroupSize.x * p_gl_WorkGroupSize.y;

}