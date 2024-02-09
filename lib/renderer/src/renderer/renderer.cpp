#include "renderer/renderer.hpp"

namespace VTX::Renderer
{

	void Renderer::build( const uint p_output, void * p_loader )
	{
		if ( _context )
		{
			clean();
		}

		VTX_INFO(
			"Renderer graph setup total time: {}",
			Util::CHRONO_CPU(
				[ & ]()
				{
					_context = _renderGraph->setup(
						p_loader ? p_loader : _loader,
						width,
						height,
						_shaderPath,
						_instructions,
						_instructionsDurationRanges,
						p_output
					);

					if ( _context )
					{
						for ( const Proxy::Molecule & proxy : _proxiesMolecules )
						{
							_setData( proxy );
						}
						for ( const Proxy::Voxel & proxy : _proxiesVoxels )
						{
							_setData( proxy );
						}

						_context->fillInfos( infos );
						setNeedUpdate( true );
						_callbackReady();
					}
				}
			)
		);
	}

	void Renderer::clean()
	{
		_context = nullptr;
		_instructions.clear();
		_instructionsDurationRanges.clear();
		_renderGraph->clean();
		_needUpdate		 = false;
		_framesRemaining = 0;

		sizeAtoms	  = 0;
		sizeBonds	  = 0;
		sizeRibbons	  = 0;
		sizeVoxels	  = 0;
		sizeMolecules = 0;
		infos		  = StructInfos();

		_callbackClean();
	}

	void Renderer::snapshot( std::vector<uchar> & p_image, const size_t p_width, const size_t p_height )
	{
		const size_t snapshotWidth = p_width ? p_width : width;
		const size_t snashotHeight = p_height ? p_height : height;
		bool		 isForceUpdate = forceUpdate;

		_onSnapshotPre( snapshotWidth, snapshotWidth );
		forceUpdate = true;
		_context->snapshot(
			p_image,
			_renderGraph->getRenderQueue(),
			std::bind( &Renderer::render, this, std::placeholders::_1 ),
			snapshotWidth,
			snapshotWidth
		);
		_onSnapshotPost( width, height );
		forceUpdate = isForceUpdate;
	}
} // namespace VTX::Renderer
