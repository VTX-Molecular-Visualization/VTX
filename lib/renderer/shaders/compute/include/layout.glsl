#ifndef LOCAL_SIZE_X
	#define LOCAL_SIZE_X 1
#endif
#ifndef LOCAL_SIZE_Y
	#define LOCAL_SIZE_Y 1
#endif
#ifndef LOCAL_SIZE_Z
	#define LOCAL_SIZE_Z 1
#endif
layout (local_size_x = LOCAL_SIZE_X, local_size_y = LOCAL_SIZE_Y, local_size_z = LOCAL_SIZE_Z) in;