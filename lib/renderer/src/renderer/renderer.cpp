#include "renderer/renderer.hpp"

namespace VTX::Renderer
{

	void Renderer::build( const uint p_output, void * p_loader )
	{
		clean();

		VTX_INFO(
			"Renderer graph setup total time: {}",
			Util::CHRONO_CPU(
				[ & ]()
				{
					if ( _renderGraph->setup(
							 p_loader ? p_loader : _loader,
							 _width,
							 _height,
							 _shaderPath,
							 _instructions,
							 _instructionsDurationRanges,
							 p_output
						 ) )
					{
						for ( const Proxy::Molecule & proxy : _proxiesMolecules )
						{
							_setData( proxy );
						}

						_renderGraph->fillInfos( _infos );
						setNeedUpdate( true );
						_onReady();
					}
				}
			)
		);
	}

	void Renderer::clean()
	{
		_onClean();
		_instructions.clear();
		_instructionsDurationRanges.clear();
		_renderGraph->clean();
		_infos			 = StructInfos();
		_needUpdate		 = false;
		_framesRemaining = 0;
	}

	void Renderer::snapshot( std::vector<uchar> & p_image, const size_t p_width, const size_t p_height )
	{
		const size_t width		   = p_width ? p_width : _width;
		const size_t height		   = p_height ? p_height : _height;
		bool		 isForceUpdate = _forceUpdate;

		_onSnapshotPre( width, height );
		_forceUpdate = true;
		_renderGraph->snapshot( p_image, std::bind( &Renderer::render, this, std::placeholders::_1 ), width, height );
		_onSnapshotPost( _width, _height );
		_forceUpdate = isForceUpdate;
	}
} // namespace VTX::Renderer
