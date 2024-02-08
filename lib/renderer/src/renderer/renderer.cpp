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
							 width,
							 height,
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
						for ( const Proxy::Voxel & proxy : _proxiesVoxels )
						{
							_setData( proxy );
						}

						_renderGraph->fillInfos( infos );
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
		infos			 = StructInfos();
		_needUpdate		 = false;
		_framesRemaining = 0;

		sizeAtoms	  = 0;
		sizeBonds	  = 0;
		sizeRibbons	  = 0;
		sizeVoxels	  = 0;
		sizeMolecules = 0;
	}

	void Renderer::snapshot( std::vector<uchar> & p_image, const size_t p_width, const size_t p_height )
	{
		const size_t snapshotWidth = p_width ? p_width : width;
		const size_t snashotHeight = p_height ? p_height : height;
		bool		 isForceUpdate = forceUpdate;

		_onSnapshotPre( snapshotWidth, snapshotWidth );
		forceUpdate = true;
		_renderGraph->snapshot(
			p_image, std::bind( &Renderer::render, this, std::placeholders::_1 ), snapshotWidth, snapshotWidth
		);
		_onSnapshotPost( width, height );
		forceUpdate = isForceUpdate;
	}
} // namespace VTX::Renderer
