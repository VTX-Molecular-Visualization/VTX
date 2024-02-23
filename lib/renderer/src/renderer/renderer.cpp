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
						_refreshData();
						_context->fillInfos( infos );
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

		sizeAtoms	= 0;
		sizeBonds	= 0;
		sizeRibbons = 0;
		sizeVoxels	= 0;
		infos		= StructInfos();

		_callbackClean();
	}

	void Renderer::_refreshDataSpheresCylinders()
	{
		// Check data.
		size_t totalAtoms = 0;
		size_t totalBonds = 0;
		for ( const Proxy::Molecule & proxy : _proxiesMolecules )
		{
			// Check sizes.
			assert( proxy.atomPositions );
			assert( proxy.atomColors );
			assert( proxy.atomRadii );
			assert( proxy.atomVisibilities );
			assert( proxy.atomSelections );
			assert( proxy.atomIds );

			assert( proxy.atomIds->size() == proxy.atomPositions->size() );
			assert( proxy.atomIds->size() == proxy.atomColors->size() );
			assert( proxy.atomIds->size() == proxy.atomRadii->size() );
			assert( proxy.atomIds->size() == proxy.atomVisibilities->size() );
			assert( proxy.atomIds->size() == proxy.atomSelections->size() );

			totalAtoms += proxy.atomPositions->size();
			totalBonds += proxy.bonds->size();
		}

		// Create buffers.
		_context->setData<Vec3f>( totalAtoms, "SpheresCylindersPositions" );
		_context->setData<uchar>( totalAtoms, "SpheresCylindersColors" );
		_context->setData<float>( totalAtoms, "SpheresCylindersRadii" );
		_context->setData<uint>( totalAtoms, "SpheresCylindersIds" );
		_context->setData<uchar>( totalAtoms, "SpheresCylindersFlags" );
		_context->setData<uchar>( totalAtoms, "SpheresCylindersModels" );
		_context->setData<uint>( totalBonds, "SpheresCylindersEbo" );

		// TODO: save those informations for further updates?
		size_t offsetAtoms = 0;
		size_t offsetBonds = 0;
		uchar  modelId	   = 0;
		for ( const Proxy::Molecule & proxy : _proxiesMolecules )
		{
			const size_t atomCount = proxy.atomPositions->size();
			const size_t bondCount = proxy.bonds->size();

			// Fill buffers.
			_context->setSubData( *proxy.atomPositions, "SpheresCylindersPositions", offsetAtoms );
			_context->setSubData( *proxy.atomColors, "SpheresCylindersColors", offsetAtoms );
			_context->setSubData( *proxy.atomRadii, "SpheresCylindersRadii", offsetAtoms );
			_context->setSubData( *proxy.atomIds, "SpheresCylindersIds", offsetAtoms );

			// Flags.
			std::vector<uchar> atomFlags( atomCount );
			for ( size_t i = 0; i < atomFlags.size(); ++i )
			{
				uchar flag = 0;
				flag |= ( *proxy.atomVisibilities )[ i ] << E_ELEMENT_FLAGS::VISIBILITY;
				flag |= ( *proxy.atomSelections )[ i ] << E_ELEMENT_FLAGS::SELECTION;
				atomFlags[ i ] = flag;
			}
			_context->setSubData( atomFlags, "SpheresCylindersFlags", offsetAtoms );

			_context->setSubData( std::vector<uchar>( atomCount, modelId ), "SpheresCylindersModels", offsetAtoms );

			// Move bonds.
			std::vector<uint> bonds( bondCount );
			for ( size_t i = 0; i < bondCount; ++i )
			{
				bonds[ i ] = uint( ( *proxy.bonds )[ i ] + offsetAtoms );
			}
			_context->setSubData( bonds, "SpheresCylindersEbo", offsetBonds );

			// Offsets.
			offsetAtoms += atomCount;
			offsetBonds += bondCount;
			modelId++;
		}

		// Counters.
		sizeAtoms = totalAtoms;
		sizeBonds = totalBonds;
	}

	void Renderer::_refreshDataModels()
	{
		std::vector<_StructUBOModel> models;
		Mat4f						 matrixView;
		getUniform( matrixView, "Matrix view" );

		for ( const Proxy::Molecule & proxy : _proxiesMolecules )
		{
			const Mat4f matrixModelView = matrixView * *proxy.transform;
			const Mat4f matrixNormal	= Util::Math::transpose( Util::Math::inverse( matrixModelView ) );
			models.emplace_back( _StructUBOModel { matrixModelView, matrixNormal } );
		}

		_context->setUniform( models, "Matrix model view" );
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
		Mat4f matrixProjectionOld;
		getUniform( matrixProjectionOld, "Matrix projection" );
		Mat4f matrixProjection = Util::Math::perspective(
			Util::Math::radians( p_fov ), float( p_width ) / float( p_height ), p_near, p_far
		);
		setUniform( matrixProjection, "Matrix projection" );
		_context->snapshot( p_image, _renderGraph->getRenderQueue(), _instructions, p_width, p_height );
		setUniform( matrixProjectionOld, "Matrix projection" );
	}

} // namespace VTX::Renderer
