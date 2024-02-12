#include "renderer/renderer.hpp"
#include "util/math.hpp"

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

	void Renderer::snapshot(
		std::vector<uchar> & p_image,
		const size_t		 p_width,
		const size_t		 p_height,
		const float			 p_fov,
		const float			 p_near,
		const float			 p_far
	)
	{
		Mat4f matrixProjection;
		getUniform( matrixProjection, "Matrix projection" );
		setUniform(
			Util::Math::perspective(
				Util::Math::radians( p_fov ), float( p_width ) / float( p_height ), p_near, p_far
			),
			"Matrix projection"
		);

		_context->snapshot(
			p_image, _renderGraph->getRenderQueue(), _instructions, p_width, p_height, p_fov, p_near, p_far
		);

		setUniform( matrixProjection, "Matrix projection" );
	}

} // namespace VTX::Renderer
