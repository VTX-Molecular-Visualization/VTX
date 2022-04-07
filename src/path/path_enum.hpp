#ifndef __VTX_PATH_ENUM__
#define __VTX_PATH_ENUM__

namespace VTX::Path
{
	enum class DURATION_MODE : int
    {
        CONSTANT_SPEED,
        PATH,
        VIEWPOINT
    };

    enum class INTERPOLATION_MODE : int
    {
        LINEAR,
        CATMULL_ROM,
        CUBIC
    };

} // namespace VTX::Trajectory
#endif
