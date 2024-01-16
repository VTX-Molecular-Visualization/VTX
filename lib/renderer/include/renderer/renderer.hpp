#ifndef __VTX_RENDERER_RENDERER__
#define __VTX_RENDERER_RENDERER__

#include "context/opengl_45.hpp"
#include "render_graph.hpp"
#include "scheduler/depth_first_search.hpp"
#include "struct_proxy_mesh.hpp"
#include "struct_proxy_molecule.hpp"
#include <util/chrono.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer
{
	class Renderer
	{
	  public:
		using RenderGraphOpenGL45 = RenderGraph<Context::OpenGL45, Scheduler::DepthFirstSearch>;
		using CallbackClean		  = std::function<void()>;
		using CallbackReady		  = std::function<void()>;

		Renderer(
			const size_t	 p_width,
			const size_t	 p_height,
			const FilePath & p_shaderPath,
			void *			 p_loader = nullptr
		) :
			_width( p_width ),
			_height( p_height ), _shaderPath( p_shaderPath ), _loader( p_loader )
		{
			// Graph.
			_renderGraph = std::make_unique<RenderGraphOpenGL45>();

			// Passes.
			Pass * const geo	 = _renderGraph->addPass( descPassGeometric );
			Pass * const depth	 = _renderGraph->addPass( descPassDepth );
			Pass * const ssao	 = _renderGraph->addPass( descPassSSAO );
			Pass * const blurX	 = _renderGraph->addPass( descPassBlur );
			Pass * const blurY	 = _renderGraph->addPass( descPassBlur );
			Pass * const shading = _renderGraph->addPass( descPassShading );
			Pass * const outline = _renderGraph->addPass( descPassOutline );
			Pass * const fxaa	 = _renderGraph->addPass( desPassFXAA );
			// Pass * const crt	 = _renderGraph->addPass( descPassCRT );

			// Setup values.
			geo->programs[ 0 ].draw.value().count	 = &_sizeAtoms;
			geo->programs[ 1 ].draw.value().count	 = &_sizeBonds;
			geo->programs[ 2 ].draw.value().count	 = &_sizeRibbons;
			blurX->name								 = "BlurX";
			blurY->name								 = "BlurY";
			blurY->programs[ 0 ].uniforms[ 0 ].value = StructUniformValue<Vec2i> { Vec2i( 0, 1 ) };

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
			_renderGraph->addLink( outline, fxaa, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->setOutput( &fxaa->outputs[ E_CHANNEL_OUTPUT::COLOR_0 ] );
			//_renderGraph->addLink( fxaa, crt, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			//_renderGraph->setOutput( &crt->outputs[ E_CHANNEL_OUTPUT::COLOR_0 ] );

			// Shared uniforms.
			_renderGraph->addUniforms(
				{ { "Matrix model", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
				  { "Matrix normal", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
				  { "Matrix view", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
				  { "Matrix projection", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
				  // { _near * _far, _far, _far - _near, _near }
				  { "Camera clip infos", E_TYPE::VEC4F, StructUniformValue<Vec4f> { VEC4F_ZERO } },
				  { "Mouse position", E_TYPE::VEC2I, StructUniformValue<Vec2i> { { 0, 0 } } },
				  { "Is perspective", E_TYPE::UINT, StructUniformValue<uint> { 1 } } }
			);

			// create array of 256 color white.
			std::array<Util::Color::Rgba, 256> colorLayout;
			std::generate( colorLayout.begin(), colorLayout.end(), [] { return Util::Color::Rgba::random(); } );

			_renderGraph->addUniforms( { { "Color layout",
										   E_TYPE::COLOR4_256,
										   StructUniformValue<std::array<Util::Color::Rgba, 256>> { colorLayout } } } );
		}

		template<typename T>
		inline void setUniform( const T & p_value, const std::string & p_key )
		{
			_renderGraph->setUniform<T>( p_value, p_key );
		}

		template<typename T>
		inline void getUniform( T & p_value, const std::string & p_key )
		{
			_renderGraph->getUniform<T>( p_value, p_key );
		}

		inline void resize( const size_t p_width, const size_t p_height, const uint p_output = 0 )
		{
			_width	= p_width;
			_height = p_height;

			if ( _renderGraph->isBuilt() )
			{
				_renderGraph->resize( p_width, p_height );
			}

			VTX_DEBUG( "resize: {} {}", p_width, p_height );
		}

		inline void setOutput( const uint p_output )
		{
			if ( _renderGraph->isBuilt() )
			{
				_renderGraph->setOutput( p_output );
			}
		}

		inline void build( const uint p_output = 0, void * p_loader = nullptr )
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
							for ( const StructProxyMolecule & proxy : _proxiesMolecules )
							{
								_setData( proxy );
							}

							_renderGraph->fillInfos( _infos );

							_onReady();
						}
					}
				)
			);
		}

		inline void clean()
		{
			_instructions.clear();
			_instructionsDurationRanges.clear();
			_renderGraph->clean();
			_infos = StructInfos();

			_onClean();
		}

		inline void render( const float p_time )
		{
			if ( _logDurations )
			{
				for ( InstructionsDurationRange & instructionDurationRange : _instructionsDurationRanges )
				{
					instructionDurationRange.duration = _renderGraph->measureTaskDuration(

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
			else
			{
				for ( const Instruction & instruction : _instructions )
				{
					instruction();
				}
			}
		}

		inline void setCallbackClean( const CallbackClean & p_cb ) { _callbackClean = p_cb; }

		inline void setCallbackReady( const CallbackReady & p_cb ) { _callbackReady = p_cb; }

		inline void setMatrixView( const Mat4f & p_view ) { setUniform( p_view, "Matrix view" ); }

		inline void setMatrixProjection( const Mat4f & p_proj ) { setUniform( p_proj, "Matrix projection" ); }

		inline void setCameraClipInfos( const float p_near, const float p_far )
		{
			setUniform( Vec4f( p_near * p_far, p_far, p_far - p_near, p_near ), "Camera clip infos" );
		}

		inline void setMousePosition( const Vec2i & p_position )
		{
			setUniform( Vec2i { p_position.x, _height - p_position.y }, "Mouse position" );
		}

		inline void addMolecule( const StructProxyMolecule & p_proxy )
		{
			_proxiesMolecules.push_back( p_proxy );

			if ( _renderGraph->isBuilt() )
			{
				_setData( p_proxy );
			}
		}

		inline void setColorLayout( const std::array<Util::Color::Rgba, 256> p_layout )
		{
			setUniform( p_layout, "Color layout" );
		}

		inline const size_t getWidth() const { return _width; }
		inline const size_t getHeight() const { return _height; }
		inline const size_t getAtomCount() const { return _sizeAtoms; }
		inline const size_t getBondCount() const { return _sizeBonds; }

		inline const StructInfos & getInfos() const { return _infos; }

		inline const bool isLogDurations() const { return _logDurations; }
		inline void		  setLogDurations( const bool p_value ) { _logDurations = p_value; }
		inline void		  compileShaders() const { _renderGraph->compileShaders(); }

		// Debug purposes only.
		inline RenderGraphOpenGL45 &			  getRenderGraph() { return *_renderGraph; }
		inline const InstructionsDurationRanges & getInstructionsDurationRanges() const
		{
			return _instructionsDurationRanges;
		}

	  private:
		void * _loader = nullptr;

		size_t								 _width;
		size_t								 _height;
		FilePath							 _shaderPath;
		std::unique_ptr<RenderGraphOpenGL45> _renderGraph;
		Instructions						 _instructions;
		InstructionsDurationRanges			 _instructionsDurationRanges;
		StructInfos							 _infos;
		bool								 _logDurations = false;

		CallbackClean _callbackClean;
		CallbackReady _callbackReady;

		std::vector<StructProxyMolecule> _proxiesMolecules;

		size_t _sizeAtoms	= 0;
		size_t _sizeBonds	= 0;
		size_t _sizeRibbons = 0;

		void _setData( const StructProxyMolecule & p_proxy )
		{
			_setDataSpheresCylinders( p_proxy );
			//_setDataRibbons( p_proxy );
		}

		// TODO: make "filler" functions for each type of data ?
		// TODO: mapping registry.
		enum E_ATOM_FLAGS
		{
			VISIBILITY = 0,
			SELECTION  = 1
		};

		void _setDataSpheresCylinders( const StructProxyMolecule & p_proxy )
		{
			assert( p_proxy.atomPositions->size() == p_proxy.atomColors->size() );
			assert( p_proxy.atomPositions->size() == p_proxy.atomRadii->size() );
			assert( p_proxy.atomPositions->size() == p_proxy.atomVisibilities->size() );
			assert( p_proxy.atomPositions->size() == p_proxy.atomSelections->size() );
			assert( p_proxy.atomPositions->size() == p_proxy.atomIds->size() );

			_renderGraph->setData( *p_proxy.atomPositions, "SpheresCylindersPositions" );
			_renderGraph->setData( *p_proxy.atomColors, "SpheresCylindersColors" );
			_renderGraph->setData( *p_proxy.atomRadii, "SpheresCylindersRadii" );
			_renderGraph->setData( *p_proxy.atomIds, "SpheresCylindersIds" );
			_renderGraph->setData( *p_proxy.bonds, "SpheresCylindersEbo" );

			std::vector<uchar> atomFlags( p_proxy.atomPositions->size() );

			for ( size_t i = 0; i < atomFlags.size(); ++i )
			{
				uchar flag = 0;
				flag |= ( *p_proxy.atomVisibilities )[ i ] << E_ATOM_FLAGS::VISIBILITY;
				flag |= ( *p_proxy.atomSelections )[ i ] << E_ATOM_FLAGS::SELECTION;
				atomFlags[ i ] = flag;
			}

			_renderGraph->setData( atomFlags, "SpheresCylindersFlags" );

			_sizeAtoms = p_proxy.atomPositions->size();
			_sizeBonds = p_proxy.bonds->size();
		}

		void _setDataRibbons( const StructProxyMolecule & p_proxy )
		{
			// Carbon alpha (Ca) positions.
			// Add an extra float increasing along the backbone (to determine direction for two sided ss).
			std::vector<Vec4f> _bufferCaPositions;
			// Ca -> O directions.
			std::vector<Vec3f> _bufferCaODirections;
			// Secondary structure types.
			std::vector<uchar> _bufferSSTypes;
			std::vector<uchar> _bufferColors;
			std::vector<uint>  _bufferIds;
			std::vector<uchar> _bufferFlags;
			std::vector<uint>  _bufferIndices;

			std::map<uint, uint> _residueToIndices;
			std::map<uint, uint> _residueToPositions;

			std::map<uint, std::vector<uint>> _data; // Chain to residues.

			auto _tryConstruct = [ & ](
									 const uint					p_chainIdx,
									 const std::vector<uint> &	p_residueIndex,
									 const std::vector<Vec4f> & p_caPositions,
									 std::vector<Vec3f> &		p_caODirections,
									 const std::vector<uchar> & p_ssTypes,
									 const std::vector<uchar> & p_colors,
									 const std::vector<uchar> & p_flags,
									 const std::vector<uint> &	p_ids
								 )
			{
				if ( p_caPositions.size() >= 4 )
				{
					const size_t nbControlPoints = p_caPositions.size();

					_residueToPositions.emplace( p_residueIndex[ 0 ], uint( _bufferCaPositions.size() ) );
					_residueToIndices.emplace( p_residueIndex[ 0 ], uint( _bufferIndices.size() ) );

					const uint offset = uint( _bufferCaPositions.size() );

					// Add segment with duplicate first index to evaluate B-spline at 0-1.
					_bufferIndices.emplace_back( offset );
					_bufferIndices.emplace_back( offset );
					_bufferIndices.emplace_back( offset + 1 );
					_bufferIndices.emplace_back( offset + 2 );

					for ( uint i = 1; i < nbControlPoints - 2; ++i )
					{
						_residueToPositions.emplace( p_residueIndex[ i ], uint( _bufferCaPositions.size() + i ) );
						_residueToIndices.emplace( p_residueIndex[ i ], uint( _bufferIndices.size() ) );

						_bufferIndices.emplace_back( offset + i - 1 );
						_bufferIndices.emplace_back( offset + i );
						_bufferIndices.emplace_back( offset + i + 1 );
						_bufferIndices.emplace_back( offset + i + 2 );
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
					auto it = _data.find( p_chainIdx );
					if ( it == _data.end() )
					{
						_data.emplace( p_chainIdx, std::vector<uint>() );
					}
					_data[ p_chainIdx ].insert(
						std::end( _data[ p_chainIdx ] ), std::begin( p_residueIndex ), std::end( p_residueIndex )
					);

					_bufferCaPositions.insert( _bufferCaPositions.end(), p_caPositions.cbegin(), p_caPositions.cend() );
					_bufferCaODirections.insert(
						_bufferCaODirections.end(), p_caODirections.cbegin(), p_caODirections.cend()
					);
					_bufferSSTypes.insert( _bufferSSTypes.end(), p_ssTypes.cbegin(), p_ssTypes.cend() );
					_bufferColors.insert( _bufferColors.end(), p_colors.cbegin(), p_colors.cend() );
					_bufferFlags.insert( _bufferFlags.end(), p_flags.cbegin(), p_flags.cend() );
					_bufferIds.insert( _bufferIds.end(), p_ids.cbegin(), p_ids.cend() );
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

				uint residueCount	 = ( *p_proxy.chainResidueCounts )[ chainIdx ];
				uint idxFirstResidue = ( *p_proxy.chainFirstResidues )[ chainIdx ];

				// No enought residues.
				if ( residueCount < 4 ) // TODO: what to do ?
				{
					// VTX_DEBUG( "Chain residue count < 4" );
					// std::cout << "residue count < 4 in chain " << chain->getName() << std::endl;
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
						flags		  = std::vector<uchar>();
						ids			  = std::vector<uint>();
						residueIndex  = std::vector<uint>();

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
							if ( ( *p_proxy.atomNames )[ p_residueIdx ] == p_name )
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
						Vec4f( positionCA, float( _bufferCaPositions.size() + caPositions.size() ) )
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
					int color = ( ( residueIdx * 7 ) % 256 );
					colors.emplace_back( color );

					// Flag.
					// TODO.
					flags.emplace_back( 1 );

					/*
					visibilities.emplace_back( uint(
						_molecule->isVisible() && chain->isVisible() && residue->isVisible() && CA->isVisible()
						&& O->isVisible()
					) );
					*/

					ids.emplace_back( ( *p_proxy.residueIds )[ residueIdx ] );
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
				_tryConstruct( chainIdx, residueIndex, caPositions, caODirections, types, colors, flags, ids );
			}

			// Reverse indices to render the other side.
			// std::vector<uint> indicesReverse = _bufferIndices;
			// std::reverse( indicesReverse.begin(), indicesReverse.end() );
			//_bufferIndices.insert( _bufferIndices.end(), indicesReverse.begin(), indicesReverse.end() );

			// Set data.
			assert( _bufferCaPositions.size() == _bufferCaODirections.size() );
			assert( _bufferCaPositions.size() == _bufferSSTypes.size() );
			assert( _bufferCaPositions.size() == _bufferColors.size() );
			assert( _bufferCaPositions.size() == _bufferFlags.size() );
			assert( _bufferCaPositions.size() == _bufferIds.size() );

			_renderGraph->setData( _bufferCaPositions, "RibbonsPositions" );
			_renderGraph->setData( _bufferCaODirections, "RibbonsDirections" );
			_renderGraph->setData( _bufferSSTypes, "RibbonsTypes" );
			_renderGraph->setData( _bufferColors, "RibbonsColors" );
			_renderGraph->setData( _bufferFlags, "RibbonsFlags" );
			_renderGraph->setData( _bufferIds, "RibbonsIds" );
		}

		inline void _onClean()
		{
			if ( _callbackClean )
			{
				_callbackClean();
			}
		}

		inline void _onReady()
		{
			if ( _callbackReady )
			{
				_callbackReady();
			}
		}
	};
} // namespace VTX::Renderer
#endif
