#include "contour_buildup.hpp"
#include "tool/chrono.hpp"

namespace VTX
{
	namespace Model
	{
		ContourBuildup::ContourBuildup( Molecule * const p_molecule ) :
			BaseModel3D( VTX::ID::Model::MODEL_CONTOUR_BUILDUP ), _molecule( p_molecule )
		{
			Tool::Chrono chrono;
			chrono.start();
			VTX_INFO( "Creating contour buildup structure..." );

			chrono.stop();
			VTX_INFO( "Contour buildup structure created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void ContourBuildup::_init() {}

		void ContourBuildup::_fillBuffer() {}

		void ContourBuildup::_computeAABB() const {}

		void ContourBuildup::_instantiate3DViews() {}
	} // namespace Model
} // namespace VTX
