#include "viewpoint.hpp"

namespace VTX
{
	namespace Model
	{
		Viewpoint::Viewpoint( Path * const p_path ) : BaseModel( VTX::ID::Model::MODEL_VIEWPOINT ), _path( p_path )
		{
			_storedName = "Viewpoint";
			_name		= &_storedName;
		}
	} // namespace Model
} // namespace VTX
