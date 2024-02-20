#ifndef __VTX_RENDERER_RENDERER__
#define __VTX_RENDERER_RENDERER__

#include "context/opengl_45.hpp"
#include "passes.hpp"
#include "proxy/mesh.hpp"
#include "proxy/molecule.hpp"
#include "proxy/representation.hpp"
#include "proxy/voxel.hpp"
#include "render_graph.hpp"
#include "scheduler/depth_first_search.hpp"
#include <util/callback.hpp>
#include <util/chrono.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer
{
	class Renderer
	{
	  public:
		using RenderGraphOpenGL45  = RenderGraph<Context::OpenGL45, Scheduler::DepthFirstSearch>;
		using CallbackClean		   = std::function<void()>;
		using CallbackReady		   = std::function<void()>;
		using CallbackSnapshotPre  = std::function<void( const size_t, const size_t )>;
		using CallbackSnapshotPost = std::function<void( const size_t, const size_t )>;

		Renderer(
			const size_t	 p_width,
			const size_t	 p_height,
			const FilePath & p_shaderPath,
			void *			 p_loader = nullptr
		) :
			width( p_width ),
			height( p_height ), _shaderPath( p_shaderPath ), _loader( p_loader )
		{
			// Graph.
			_renderGraph = std::make_unique<RenderGraphOpenGL45>();

			// Passes.
			Pass * const geo	   = _renderGraph->addPass( descPassGeometric );
			Pass * const depth	   = _renderGraph->addPass( descPassDepth );
			Pass * const ssao	   = _renderGraph->addPass( descPassSSAO );
			Pass * const blurX	   = _renderGraph->addPass( descPassBlur );
			Pass * const blurY	   = _renderGraph->addPass( descPassBlur );
			Pass * const shading   = _renderGraph->addPass( descPassShading );
			Pass * const outline   = _renderGraph->addPass( descPassOutline );
			Pass * const selection = _renderGraph->addPass( descPassSelection );
			Pass * const fxaa	   = _renderGraph->addPass( desPassFXAA );

			// Setup values.
			geo->programs[ 0 ].draw.value().countFunction	 = [ & ]() { return showAtoms ? sizeAtoms : 0; };
			geo->programs[ 1 ].draw.value().countFunction	 = [ & ]() { return showBonds ? sizeBonds : 0; };
			geo->programs[ 2 ].draw.value().countFunction	 = [ & ]() { return showRibbons ? sizeRibbons : 0; };
			geo->programs[ 3 ].draw.value().countFunction	 = [ & ]() { return showVoxels ? sizeVoxels : 0; };
			blurX->name										 = "BlurX";
			blurY->name										 = "BlurY";
			blurY->programs[ 0 ].uniforms.entries[ 0 ].value = StructUniformValue<Vec2i> { Vec2i( 0, 1 ) };

			// Links.
			_renderGraph->addLink( geo, depth, E_CHANNEL_OUTPUT::DEPTH, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( geo, ssao, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( depth, ssao, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_2 );
			_renderGraph->addLink( ssao, blurX, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( depth, blurX, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( blurX, blurY, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( depth, blurY, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( geo, shading, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( geo, shading, E_CHANNEL_OUTPUT::COLOR_1, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( blurY, shading, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_2 );
			_renderGraph->addLink( shading, outline, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( depth, outline, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( geo, selection, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( outline, selection, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( depth, selection, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_2 );
			_renderGraph->addLink( selection, fxaa, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->setOutput( &fxaa->outputs[ E_CHANNEL_OUTPUT::COLOR_0 ] );

			// Shared uniforms.
			_renderGraph->addUniforms(
				{ { { "Matrix view", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
					{ "Matrix projection", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
					{ "Camera position", E_TYPE::VEC3F, StructUniformValue<Vec3f> { VEC3F_ZERO } },
					{ "Camera clip infos", // { _near * _far, _far, _far - _near, _near }
					  E_TYPE::VEC4F,
					  StructUniformValue<Vec4f> { VEC4F_ZERO } },
					{ "Mouse position", E_TYPE::VEC2I, StructUniformValue<Vec2i> { Vec2i { 0, 0 } } },
					{ "Is perspective", E_TYPE::UINT, StructUniformValue<uint> { 1u } } } }
			);

			// TODO: move to pass ubo?
			_renderGraph->addUniforms(
				{ { { "Color layout", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> {} } }, 256 }
			);

			// TODO: dynamic ubo size instead of fixed 256.
			_renderGraph->addUniforms(
				{ { { "Matrix model view", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
					{ "Matrix normal", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } } },
				  256 }
			);
		}

		// Only first entry of the array saved on cpu.
		template<typename T>
		inline void setUniform( const std::vector<T> & p_value, const std::string & p_key )
		{
			assert( _context != nullptr );
			_context->setUniform<T>( p_value, p_key );
			setNeedUpdate( true );
		}

		template<typename T>
		inline void setUniform( const T & p_value, const std::string & p_key )
		{
			assert( _context != nullptr );
			_context->setUniform<T>( p_value, p_key );
			setNeedUpdate( true );
		}

		template<typename T>
		inline void getUniform( T & p_value, const std::string & p_key )
		{
			_context->getUniform<T>( p_value, p_key );
		}

		inline void resize( const size_t p_width, const size_t p_height, const uint p_output = 0 )
		{
			// assert( _context != nullptr );
			//  TODO: delete context check.
			if ( _context == nullptr )
				return;

			width  = p_width;
			height = p_height;

			_context->resize( _renderGraph->getRenderQueue(), p_width, p_height );

			setNeedUpdate( true );
		}

		inline void setOutput( const uint p_output )
		{
			assert( _context != nullptr );

			_context->setOutput( p_output );
			setNeedUpdate( true );
		}

		void build( const uint p_output = 0, void * p_loader = nullptr );
		void clean();

		inline void render( const float p_time )
		{
			if ( _needUpdate || forceUpdate || _framesRemaining > 0 )
			{
				if ( logDurations )
				{
					_renderLog( p_time );
				}
				else
				{
					_render( p_time );
				}

				if ( forceUpdate == false )
				{
					if ( _needUpdate )
					{
						setNeedUpdate( false );
					}
					else
					{
						_framesRemaining--;
					}
				}
			}
		}

		inline void addCallbackReady( const Util::Callback<>::Func & p_cb ) { _callbackReady += p_cb; }
		inline void addCallbackClean( const Util::Callback<>::Func & p_cb ) { _callbackClean += p_cb; }

		inline void setMatrixView( const Mat4f & p_view )
		{
			setUniform( p_view, "Matrix view" );

			// Update model view matrices.
			_setDataModel( _proxiesMolecules );
		}

		inline void setMatrixProjection( const Mat4f & p_proj ) { setUniform( p_proj, "Matrix projection" ); }

		inline void setCameraPosition( const Vec3f & p_position ) { setUniform( p_position, "Camera position" ); }

		inline void setCameraClipInfos( const float p_near, const float p_far )
		{
			setUniform( Vec4f( p_near * p_far, p_far, p_far - p_near, p_near ), "Camera clip infos" );
		}

		inline void setMousePosition( const Vec2i & p_position )
		{
			// setUniform( Vec2i { p_position.x, _height - p_position.y }, "Mouse position" );
		}

		inline void setPerspective( const bool p_perspective )
		{
			setUniform( uint( p_perspective ), "Is perspective" );
		}

		inline void addProxy( const Proxy::Molecule & p_proxy )
		{
			_proxiesMolecules.push_back( p_proxy );

			if ( _renderGraph->isBuilt() )
			{
				_setData( p_proxy );
			}

			setNeedUpdate( true );
		}

		inline void addProxy( const Proxy::Voxel & p_proxy )
		{
			_proxiesVoxels.push_back( p_proxy );

			if ( _renderGraph->isBuilt() )
			{
				_setData( p_proxy );
			}

			setNeedUpdate( true );
		}

		inline void setColorLayout( const std::array<Util::Color::Rgba, 256> & p_layout )
		{
			setUniform( std::vector<Util::Color::Rgba>( p_layout.begin(), p_layout.end() ), "Color layout" );
		}

		void snapshot(
			std::vector<uchar> & p_image,
			const size_t		 p_width,
			const size_t		 p_height,
			const float			 p_fov,
			const float			 p_near,
			const float			 p_far
		);

		inline Vec2i getPickedIds( const size_t p_x, const size_t p_y ) const
		{
			std::any idsAny = std::make_any<Vec2i>();
			_context->getTextureData( idsAny, p_x, height - p_y, "Geometric", E_CHANNEL_OUTPUT::COLOR_2 );
			return std::any_cast<Vec2i>( idsAny );
		}

		inline void setNeedUpdate( const bool p_value )
		{
			_needUpdate = p_value;
			if ( p_value == false )
			{
				_framesRemaining = _BUFFER_COUNT;
			}
		}

		inline void compileShaders() const { _context->compileShaders(); }

		inline RenderGraphOpenGL45 &			  getRenderGraph() { return *_renderGraph; }
		inline const std::vector<Pass *> &		  getAvailablePasses() const { return availablePasses; }
		inline const InstructionsDurationRanges & getInstructionsDurationRanges() const
		{
			return _instructionsDurationRanges;
		}
		inline std::vector<Proxy::Molecule> &		getProxiesMolecules() { return _proxiesMolecules; }
		inline std::vector<Proxy::Mesh> &			getProxiesMeshes() { return _proxiesMeshes; }
		inline std::vector<Proxy::Representation> & getProxiesRepresentations() { return _proxiesRepresentations; }
		inline std::vector<Proxy::Voxel> &			getProxiesVoxels() { return _proxiesVoxels; }

		size_t		width;
		size_t		height;
		StructInfos infos;

		uint sizeAtoms	 = 0;
		uint sizeBonds	 = 0;
		uint sizeRibbons = 0;
		uint sizeVoxels	 = 0;

		uint sizeMolecules = 0;

		bool showAtoms	 = true;
		bool showBonds	 = true;
		bool showRibbons = true;
		bool showVoxels	 = true;

		bool forceUpdate  = false;
		bool logDurations = false;

	  private:
		const size_t _BUFFER_COUNT = 2;

		Context::OpenGL45 * _context		 = nullptr;
		void *				_loader			 = nullptr;
		bool				_needUpdate		 = false;
		size_t				_framesRemaining = _BUFFER_COUNT;

		FilePath							 _shaderPath;
		std::unique_ptr<RenderGraphOpenGL45> _renderGraph;
		Instructions						 _instructions;
		InstructionsDurationRanges			 _instructionsDurationRanges;

		Util::Callback<> _callbackReady;
		Util::Callback<> _callbackClean;

		// TODO: mapping registry.
		std::vector<Proxy::Molecule>	   _proxiesMolecules;
		std::vector<Proxy::Mesh>		   _proxiesMeshes;
		std::vector<Proxy::Representation> _proxiesRepresentations;
		std::vector<Proxy::Voxel>		   _proxiesVoxels;

		// TODO: make "filler" functions for each type of data instead of _setDataX?
		void _setData( const Proxy::Molecule & p_proxy )
		{
			assert( p_proxy.atomIds || p_proxy.residueIds );
			assert( p_proxy.transform );

			if ( p_proxy.atomIds )
			{
				_setDataSpheresCylinders( p_proxy );
			}

			if ( p_proxy.residueIds )
			{
				_setDataRibbons( p_proxy );
			}

			_setDataModel( { p_proxy } );

			sizeMolecules++;
		}

		enum E_ATOM_FLAGS
		{
			VISIBILITY = 0,
			SELECTION  = 1
		};

		void _setDataSpheresCylinders( const Proxy::Molecule & p_proxy )
		{
			// Check sizes.
			assert( p_proxy.atomPositions );
			assert( p_proxy.atomColors );
			assert( p_proxy.atomRadii );
			assert( p_proxy.atomVisibilities );
			assert( p_proxy.atomSelections );
			assert( p_proxy.atomIds );

			assert( p_proxy.atomIds->size() == p_proxy.atomPositions->size() );
			assert( p_proxy.atomIds->size() == p_proxy.atomColors->size() );
			assert( p_proxy.atomIds->size() == p_proxy.atomRadii->size() );
			assert( p_proxy.atomIds->size() == p_proxy.atomVisibilities->size() );
			assert( p_proxy.atomIds->size() == p_proxy.atomSelections->size() );

			// Forward data.
			// TODO: add to current buffer.
			_context->setData( *p_proxy.atomPositions, "SpheresCylindersPositions" );
			_context->setData( *p_proxy.atomColors, "SpheresCylindersColors" );
			_context->setData( *p_proxy.atomRadii, "SpheresCylindersRadii" );
			_context->setData( *p_proxy.atomIds, "SpheresCylindersIds" );
			_context->setData( *p_proxy.bonds, "SpheresCylindersEbo" );

			// Flags.
			std::vector<uchar> atomFlags( p_proxy.atomPositions->size() );
			for ( size_t i = 0; i < atomFlags.size(); ++i )
			{
				uchar flag = 0;
				flag |= ( *p_proxy.atomVisibilities )[ i ] << E_ATOM_FLAGS::VISIBILITY;
				flag |= ( *p_proxy.atomSelections )[ i ] << E_ATOM_FLAGS::SELECTION;
				atomFlags[ i ] = flag;
			}
			_context->setData( atomFlags, "SpheresCylindersFlags" );

			// Model ID.
			uchar modelId = sizeMolecules;
			_context->setData( std::vector<uchar>( p_proxy.atomPositions->size(), modelId ), "SpheresCylindersModels" );

			// Counters.
			sizeAtoms += uint( p_proxy.atomPositions->size() );
			sizeBonds += uint( p_proxy.bonds->size() );
		}

		void _setDataRibbons( const Proxy::Molecule & p_proxy )
		{
			assert( p_proxy.atomNames );
			assert( p_proxy.residueIds );
			assert( p_proxy.residueSecondaryStructureTypes );
			assert( p_proxy.residueColors );
			assert( p_proxy.residueFirstAtomIndexes );
			assert( p_proxy.residueAtomCounts );
			assert( p_proxy.chainFirstResidues );
			assert( p_proxy.chainResidueCounts );

			assert( p_proxy.atomNames->size() == p_proxy.atomPositions->size() );
			assert( p_proxy.residueIds->size() == p_proxy.residueSecondaryStructureTypes->size() );
			assert( p_proxy.residueIds->size() == p_proxy.residueColors->size() );
			assert( p_proxy.residueIds->size() == p_proxy.residueFirstAtomIndexes->size() );
			assert( p_proxy.residueIds->size() == p_proxy.residueAtomCounts->size() );
			assert( p_proxy.chainFirstResidues->size() == p_proxy.chainResidueCounts->size() );

			// Carbon alpha (Ca) positions.
			// Add an extra float increasing along the backbone (to determine direction for two sided ss).
			std::vector<Vec4f> bufferCaPositions;
			// Ca -> O directions.
			std::vector<Vec3f> bufferCaODirections;
			// Secondary structure types.
			std::vector<uchar> bufferSSTypes;
			std::vector<uchar> bufferColors;
			std::vector<uint>  bufferIds;
			std::vector<uchar> bufferFlags;
			std::vector<uchar> bufferModels;

			std::vector<uint> bufferIndices;

			std::map<uint, uint> residueToIndices;
			std::map<uint, uint> residueToPositions;

			std::map<uint, std::vector<uint>> data; // Chain to residues.

			auto _tryConstruct = [ & ](
									 const uint					p_chainIdx,
									 const std::vector<uint> &	p_residueIndex,
									 const std::vector<Vec4f> & p_caPositions,
									 std::vector<Vec3f> &		p_caODirections,
									 const std::vector<uchar> & p_ssTypes,
									 const std::vector<uchar> & p_colors,
									 const std::vector<uint> &	p_ids,
									 const std::vector<uchar> & p_flags,
									 const std::vector<uchar> & p_models

								 )
			{
				if ( p_caPositions.size() >= 4 )
				{
					const size_t nbControlPoints = p_caPositions.size();

					residueToPositions.emplace( p_residueIndex[ 0 ], uint( bufferCaPositions.size() ) );
					residueToIndices.emplace( p_residueIndex[ 0 ], uint( bufferIndices.size() ) );

					const uint offset = uint( bufferCaPositions.size() );

					// Add segment with duplicate first index to evaluate B-spline at 0-1.
					bufferIndices.emplace_back( offset );
					bufferIndices.emplace_back( offset );
					bufferIndices.emplace_back( offset + 1 );
					bufferIndices.emplace_back( offset + 2 );

					for ( uint i = 1; i < nbControlPoints - 2; ++i )
					{
						residueToPositions.emplace( p_residueIndex[ i ], uint( bufferCaPositions.size() + i ) );
						residueToIndices.emplace( p_residueIndex[ i ], uint( bufferIndices.size() ) );

						bufferIndices.emplace_back( offset + i - 1 );
						bufferIndices.emplace_back( offset + i );
						bufferIndices.emplace_back( offset + i + 1 );
						bufferIndices.emplace_back( offset + i + 2 );
					}

					// TODO: better on GPU ?
					// CheckOrientationAndFlip.
					size_t i;
					for ( i = 1; i < p_caODirections.size(); ++i )
					{
						if ( Util::Math::dot( p_caODirections[ i ], p_caODirections[ i - 1 ] ) < 0.f )
						{
							p_caODirections[ i ] = -p_caODirections[ i ];
						}
					}

					// Merge buffers.
					auto it = data.find( p_chainIdx );
					if ( it == data.end() )
					{
						data.emplace( p_chainIdx, std::vector<uint>() );
					}
					data[ p_chainIdx ].insert(
						std::end( data[ p_chainIdx ] ), std::begin( p_residueIndex ), std::end( p_residueIndex )
					);

					bufferCaPositions.insert( bufferCaPositions.end(), p_caPositions.cbegin(), p_caPositions.cend() );
					bufferCaODirections.insert(
						bufferCaODirections.end(), p_caODirections.cbegin(), p_caODirections.cend()
					);
					bufferSSTypes.insert( bufferSSTypes.end(), p_ssTypes.cbegin(), p_ssTypes.cend() );
					bufferColors.insert( bufferColors.end(), p_colors.cbegin(), p_colors.cend() );
					bufferIds.insert( bufferIds.end(), p_ids.cbegin(), p_ids.cend() );
					bufferFlags.insert( bufferFlags.end(), p_flags.cbegin(), p_flags.cend() );
					bufferModels.insert( bufferModels.end(), p_models.cbegin(), p_models.cend() );
				}
			};

			const std::vector<Vec3f> & positions = *p_proxy.atomPositions;

			// Temporary vectors, merged with buffers if constructed.
			std::vector<Vec4f> caPositions;
			std::vector<Vec3f> caODirections;
			std::vector<uchar> types;
			std::vector<uchar> colors;
			std::vector<uint>  ids;
			std::vector<uchar> flags;
			std::vector<uchar> models;
			std::vector<uint>  residueIndex;

			for ( uint chainIdx = 0; chainIdx < p_proxy.chainFirstResidues->size(); ++chainIdx )
			{
				/*
				const Chain * const chain = _molecule->getChain( chainIdx );
				if ( chain == nullptr )
				{
					continue;
				}
				*/

				uint residueCount	 = uint( ( *p_proxy.chainResidueCounts )[ chainIdx ] );
				uint idxFirstResidue = uint( ( *p_proxy.chainFirstResidues )[ chainIdx ] );

				// No enought residues.
				if ( residueCount < 4 ) // TODO: what to do ?
				{
					VTX_DEBUG( "Chain residue count < 4" );
					continue;
				}

				bool createVectors = true;
				int	 residueLast   = -1;
				for ( uint residueIdx = idxFirstResidue; residueIdx < idxFirstResidue + residueCount; ++residueIdx )
				{
					if ( createVectors )
					{
						caPositions	  = std::vector<Vec4f>();
						caODirections = std::vector<Vec3f>();
						types		  = std::vector<uchar>();
						colors		  = std::vector<uchar>();
						ids			  = std::vector<uint>();
						flags		  = std::vector<uchar>();
						models		  = std::vector<uchar>();

						residueIndex = std::vector<uint>();

						createVectors = false;
					}

					/*
					if ( residue == nullptr )
					{
						continue;
					}
					*/

					auto findFirstAtomByName = [ &p_proxy ]( const uint p_residueIdx, const std::string & p_name )
					{
						uint atomCount	  = ( *p_proxy.residueAtomCounts )[ p_residueIdx ];
						uint idxFirstAtom = ( *p_proxy.residueFirstAtomIndexes )[ p_residueIdx ];

						for ( int i = idxFirstAtom; i < int( idxFirstAtom + atomCount ); ++i )
						{
							if ( ( *p_proxy.atomNames )[ i ] == p_name )
							{
								return i;
							}
						}

						return -1;
					};

					// Use backbone to compute spline data.
					// Find alpha carbon.
					int CA = findFirstAtomByName( residueIdx, "CA" );

					// Not an amine acid (water, heme, or phosphate groupment).
					if ( CA == -1 ) // TODO: what to do ?
					{
						continue;
					}

					// Find oxygen.
					int O = findFirstAtomByName( residueIdx, "O" );

					// Missing oxygen atom.
					if ( O == -1 ) // TODO: what to do?
					{
						continue;
					}
					/// TODO: For all these "what to do ?" I think we should render it with spheres or b&s...

					// Compute direction between carbon alpha and oxygen.
					const Vec3f & positionCA   = positions[ CA ];
					const Vec3f & positionO	   = positions[ O ];
					const Vec3f	  directionCAO = Util::Math::normalize( positionO - positionCA );

					// Store residue index for later.
					residueIndex.emplace_back( residueIdx );

					// Add carbon alpha (CA) position and CA-O direction.
					caPositions.emplace_back(
						Vec4f( positionCA, float( bufferCaPositions.size() + caPositions.size() ) )
					);
					caODirections.emplace_back( directionCAO );

					// Add secondary structure type.
					types.emplace_back( ( *p_proxy.residueSecondaryStructureTypes )[ residueIdx ] );

					/*
					switch ( residue->getRepresentation()->getRibbonData().colorMode )
					{
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::JMOL:
						colors.emplace_back( Generic::COLORS_JMOL[ uint( residue->getSecondaryStructure() ) ] );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN:
						colors.emplace_back( residue->getMoleculePtr()->getColor() );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM:
						colors.emplace_back( residue->getRepresentation()->getColor() );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CHAIN:
						colors.emplace_back( residue->getChainPtr()->getColor() );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::RESIDUE:
						colors.emplace_back( residue->getColor() );
						break;
					default: colors.emplace_back( Color::Rgba::WHITE ); break;
					}
					*/

					// Generate number between 0 and 255.
					// int color = ( ( residueIdx * 7 ) % 256 );
					// colors.emplace_back( color );

					colors.emplace_back( ( *p_proxy.residueColors )[ residueIdx ] );

					ids.emplace_back( ( *p_proxy.residueIds )[ residueIdx ] );

					// Flag.
					// TODO.
					flags.emplace_back( 1 );

					/*
					visibilities.emplace_back( uint(
						_molecule->isVisible() && chain->isVisible() && residue->isVisible() && CA->isVisible()
						&& O->isVisible()
					) );
					*/

					// Model ID.
					models.emplace_back( sizeMolecules );

					/*
					if ( residueLast != -1
						 && residue->getIndexInOriginalChain() != residueLast->getIndexInOriginalChain() + 1 )
					{
						_tryConstruct( chainIdx, residueIndex, caPositions, caODirections, types, colors, flags, ids );
						createVectors = true;
					}
					*/

					residueLast = residueIdx;
				}

				// Update buffers and index mapping if SS is constructed.
				_tryConstruct( chainIdx, residueIndex, caPositions, caODirections, types, colors, ids, flags, models );
			}

			// Reverse indices to render the other side.
			// std::vector<uint> indicesReverse = bufferIndices;
			// std::reverse( indicesReverse.begin(), indicesReverse.end() );
			// bufferIndices.insert( bufferIndices.end(), indicesReverse.begin(), indicesReverse.end() );

			// Set data.
			assert( bufferCaPositions.size() == bufferCaODirections.size() );
			assert( bufferCaPositions.size() == bufferSSTypes.size() );
			assert( bufferCaPositions.size() == bufferColors.size() );
			assert( bufferCaPositions.size() == bufferIds.size() );
			assert( bufferCaPositions.size() == bufferFlags.size() );
			assert( bufferCaPositions.size() == bufferModels.size() );

			_context->setData( bufferCaPositions, "RibbonsPositions" );
			_context->setData( bufferCaODirections, "RibbonsDirections" );
			_context->setData( bufferSSTypes, "RibbonsTypes" );
			_context->setData( bufferColors, "RibbonsColors" );
			_context->setData( bufferIds, "RibbonsIds" );
			_context->setData( bufferFlags, "RibbonsFlags" );
			_context->setData( bufferModels, "RibbonsModels" );
			_context->setData( bufferIndices, "RibbonsEbo" );

			sizeRibbons += uint( bufferIndices.size() );
		}

		void _setDataModel( const std::vector<Proxy::Molecule> & p_proxies )
		{
			struct _StructUBOModel
			{
				Mat4f mv;
				Mat4f n;
			};

			std::vector<_StructUBOModel> models;
			Mat4f						 matrixView;
			getUniform( matrixView, "Matrix view" );

			for ( const Proxy::Molecule & proxy : p_proxies )
			{
				const Mat4f matrixModelView = matrixView * *proxy.transform;
				const Mat4f matrixNormal	= Util::Math::transpose( Util::Math::inverse( matrixModelView ) );
				models.emplace_back( _StructUBOModel { matrixModelView, matrixNormal } );
			}

			_context->setUniform( models, "Matrix model view" );
		}

		void _setData( const Proxy::Voxel & p_proxy )
		{
			assert( p_proxy.mins );
			assert( p_proxy.maxs );
			assert( p_proxy.mins->size() == p_proxy.maxs->size() );

			_context->setData( *p_proxy.mins, "VoxelsMins" );
			_context->setData( *p_proxy.maxs, "VoxelsMaxs" );

			sizeVoxels += uint( p_proxy.mins->size() );
		}

		inline void _render( const float p_time ) const
		{
			for ( const Instruction & instruction : _instructions )
			{
				instruction();
			}
		}

		inline void _renderLog( const float p_time )
		{
			for ( InstructionsDurationRange & instructionDurationRange : _instructionsDurationRanges )
			{
				instructionDurationRange.duration = _context->measureTaskDuration(

					[ this, &instructionDurationRange ]()
					{
						for ( size_t i = instructionDurationRange.first; i <= instructionDurationRange.last; ++i )
						{
							_instructions[ i ]();
						}
					}
				);
			}
		}
	};
} // namespace VTX::Renderer
#endif
