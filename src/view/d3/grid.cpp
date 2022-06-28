#include "grid.hpp"

namespace VTX::View::D3
{

	Renderer::GL::Program * const Grid::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram( "VoxelShader",
													{ IO::FilePath( "voxel/voxel.vert" ),
													  IO::FilePath( "voxel/voxel.geom" ),
													  IO::FilePath( "voxel/voxel.frag" ) } );
	}

	void Grid::_init() { _gl()->glLineWidth( 2.f ); }

	void GridView::declareUniforms()
	{
		_uniformManager->setBindingInformation( "VoxelSettings" );
		_uniformManager->addValue<VTX::Mat4f>( "uMVPMatrix" );
		_uniformManager->addValue<VTX::Vec4f>( "uColor" );
	}

	void Grid::render( const Object3D::Camera & p_camera ) const
	{
		BaseView3D::render( p_camera );
		_program->use();
		const Object3D::Camera & cam = VTXApp::get().getScene().getCamera();

		_uniformManager->updateValue( "uMVPMatrix", cam.getProjectionMatrix() * cam.getViewMatrix() * transform.get() );
		_uniformManager->updateValue( "uColor", color );
		_uniformManager->bind();

		glEnable( GL_LINE_SMOOTH );
		glLineWidth( 2.f );
		glDrawArrays( GL_POINTS, 0, static_cast<GLsizei>( voxelNb ) );
	}

} // namespace VTX::View::D3
