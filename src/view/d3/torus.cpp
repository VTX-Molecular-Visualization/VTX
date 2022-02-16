#include "torus.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Torus::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram( "Torus",
													{ IO::FilePath( "torus_patch.vert" ),
													  IO::FilePath( "torus_patch.geom" ),
													  IO::FilePath( "torus_patch.frag" ) } );
	}

	void Torus::_init() {}

	void Torus::render( const Object3D::Camera & p_camera ) const { BaseView3D::render( p_camera ); }

} // namespace VTX::View::D3
