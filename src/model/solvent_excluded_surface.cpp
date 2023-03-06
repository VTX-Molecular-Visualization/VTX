#include "solvent_excluded_surface.hpp"
#include "atom.hpp"
#include "category.hpp"
#include "chain.hpp"
#include "custom/iterator.hpp"
#include "math/marching_cube.hpp"
#include "molecule.hpp"
#include "object3d/helper/aabb.hpp"
#include "object3d/scene.hpp"
#include "renderer/gl/buffer.hpp"
#include "residue.hpp"
#include "selection/selection_manager.hpp"
#include "view/d3/triangle.hpp"
#include "worker/gpu_buffer_initializer.hpp"
#include "worker/gpu_computer.hpp"
#include "worker/worker_manager.hpp"

#define VOXEL_SIZE 0.4f

#define VTX_SES_NORMALS_GPU 1

namespace VTX
{
	namespace Model
	{

		SolventExcludedSurface::SolventExcludedSurface( const Category * const p_category ) :
			MeshTriangle(), _category( p_category )
		{
		}

		const Math::Transform & SolventExcludedSurface::getTransform() const
		{
			return _category->getMoleculePtr()->getTransform();
		}

		// TODO
		// - Decompose in multiple iterations to handle larger buffers.
		void SolventExcludedSurface::_init()
		{
			if ( _category->isEmpty() )
			{
				return;
			}

			Tool::Chrono chrono, chrono2;
			chrono.start();
			chrono2.start();
			VTX_DEBUG( "Creating SES..." );

			_buffer->makeContextCurrent();
			_buffer->finish();

			const std::vector<uint> atomsIdx = _category->generateAtomIndexList();

			// Sort atoms in acceleration grid.
			const float maxVdWRadius = *std::max_element(
				Atom::SYMBOL_VDW_RADIUS, Atom::SYMBOL_VDW_RADIUS + std::size( Atom::SYMBOL_VDW_RADIUS ) );
			const Object3D::Helper::AABB molAABB = _category->getAABB();

			const float atomGridCellSize = PROBE_RADIUS + maxVdWRadius;
			const Vec3f gridMin			 = molAABB.getMin() - atomGridCellSize;
			const Vec3f gridMax			 = molAABB.getMax() + atomGridCellSize;

			const Vec3f gridSize	 = gridMax - gridMin;
			Vec3i		atomGridSize = Vec3i( Util::Math::ceil( gridSize / atomGridCellSize ) );

			Object3D::Helper::Grid gridAtoms
				= Object3D::Helper::Grid( gridMin, Vec3f( atomGridCellSize ), atomGridSize );

			std::vector<std::vector<uint>> atomGridDataTmp
				= std::vector<std::vector<uint>>( gridAtoms.getCellCount(), std::vector<uint>() );

			const std::vector<Vec3f> & atomPositions = _category->getMoleculePtr()->getCurrentAtomPositionFrame();

			// Store atom indices in acceleration grid.
			std::vector<Vec4f> atomPositionsVdW = std::vector<Vec4f>( atomPositions.size() );
			for ( const uint idx : atomsIdx )
			{
				const uint hash = gridAtoms.gridHash( atomPositions[ idx ] );

				if ( hash >= atomGridDataTmp.size() )
				{
					continue;
				}

				atomGridDataTmp[ hash ].emplace_back( idx );
				atomPositionsVdW[ idx ]
					= Vec4f( atomPositions[ idx ], _category->getMoleculePtr()->getAtom( idx )->getVdwRadius() );
			}

			// Linerize data in 1D arrays.
			std::vector<Range> atomGridDataSorted = std::vector<Range>( gridAtoms.getCellCount(), Range { 0, 0 } );
			std::vector<uint>  atomIndexSorted	  = std::vector<uint>();

			for ( uint i = 0; i < atomGridDataTmp.size(); ++i )
			{
				const std::vector<uint> & data = atomGridDataTmp[ i ];
				if ( data.empty() == false )
				{
					atomGridDataSorted[ i ] = Range { uint( atomIndexSorted.size() ), uint( data.size() ) };
					atomIndexSorted.insert( atomIndexSorted.end(), data.begin(), data.end() );
				}
			}

			// Free tmp memory.
			atomGridDataTmp.clear();
			atomGridDataTmp.shrink_to_fit();

			chrono2.stop();
			VTX_DEBUG( "Atoms sorted in {}s", chrono2.elapsedTime() );
			chrono2.start();

			// Compute SES grid and compute SDF.
			Vec3i				   sesGridSize = Vec3i( Util::Math::ceil( gridSize / VOXEL_SIZE ) );
			Object3D::Helper::Grid gridSES	   = Object3D::Helper::Grid( gridMin, Vec3f( VOXEL_SIZE ), sesGridSize );

			/////////////////////
			// Worker: create SDF.
			Worker::GpuComputer workerCreateSDF( IO::FilePath( "ses/create_sdf.comp" ) );

			// Create SSBOs.
			using VTX::Renderer::GL::Buffer;
			// Output.
			Buffer bufferSesGridData( gridSES.getCellCount() * sizeof( SESGridData ) );
			// Input.
			Buffer bufferAtomGridDataSorted( atomGridDataSorted );
			Buffer bufferAtomIndexSorted( atomIndexSorted );
			Buffer bufferAtomPosition( atomPositionsVdW );

			// Bind.
			bufferSesGridData.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			bufferAtomGridDataSorted.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferAtomIndexSorted.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );
			bufferAtomPosition.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 3 );

			// Set uniforms.
			workerCreateSDF.getProgram().use();

			workerCreateSDF.getProgram().setVec3f( "uGridAtomWorldOrigin", gridAtoms.worldOrigin );
			workerCreateSDF.getProgram().setVec3f( "uGridSESWorldOrigin", gridSES.worldOrigin );
			workerCreateSDF.getProgram().setVec3u( "uGridAtomSize", Vec3u( gridAtoms.size ) );
			workerCreateSDF.getProgram().setVec3u( "uGridSESSize", Vec3u( gridSES.size ) );
			workerCreateSDF.getProgram().setVec3f( "uGridAtomCellSize", gridAtoms.cellSize );
			workerCreateSDF.getProgram().setVec3f( "uGridSESCellSize", gridSES.cellSize );
			workerCreateSDF.getProgram().setUInt( "uGridAtomCellCount", uint( gridAtoms.getCellCount() ) );
			workerCreateSDF.getProgram().setUInt( "uGridSESCellCount", uint( gridSES.getCellCount() ) );
			workerCreateSDF.getProgram().setFloat( "uProbeRadius", PROBE_RADIUS );
			workerCreateSDF.getProgram().setFloat( "uVoxelSize", VOXEL_SIZE );

			// Start.
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			workerCreateSDF.start( gridSES.getCellCount() );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

			// Unbind.
			bufferSesGridData.unbind();
			bufferAtomGridDataSorted.unbind();
			bufferAtomIndexSorted.unbind();
			bufferAtomPosition.unbind();

			chrono2.stop();
			VTX_DEBUG( "SDF created in {}s", chrono2.elapsedTime() );
			chrono2.start();

			//////////////////////
			// Worker: refine SDF.
			Worker::GpuComputer workerRefineSDF( IO::FilePath( "ses/refine_sdf.comp" ) );

			// Bind.
			bufferSesGridData.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );

			workerRefineSDF.getProgram().use();

			Vec3i cellsToVisitCount = Util::Math::ceil( Vec3f( PROBE_RADIUS + VOXEL_SIZE ) / gridSES.cellSize );

			workerRefineSDF.getProgram().setVec3f( "uGridSESWorldOrigin", gridSES.worldOrigin );
			workerRefineSDF.getProgram().setVec3u( "uGridSESSize", Vec3u( gridSES.size ) );
			workerRefineSDF.getProgram().setVec3f( "uGridSESCellSize", gridSES.cellSize );
			workerRefineSDF.getProgram().setUInt( "uGridSESCellCount", uint( gridSES.getCellCount() ) );
			workerRefineSDF.getProgram().setVec3i( "uCellsToVisitCount", cellsToVisitCount );
			workerRefineSDF.getProgram().setFloat( "uProbeRadius", PROBE_RADIUS );

			// Start
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			workerRefineSDF.start( gridSES.getCellCount() );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

			// Unbind.
			bufferSesGridData.unbind();

			chrono2.stop();
			VTX_DEBUG( "SDF boundary created in {}s", chrono2.elapsedTime() );
			chrono2.start();

			////////////////////////////
			// Worker: reduce grid.
			Worker::GpuComputer workerReduceGrid( IO::FilePath( "ses/reduce_grid.comp" ) );
			size_t				bufferSize = gridSES.getCellCount();

			std::vector<uint> validities( bufferSize, 0 );
			Buffer			  bufferCellValidities( validities, Buffer::Flags::DYNAMIC_STORAGE_BIT );
			Buffer			  bufferCellHashs( bufferSize * sizeof( uint ) );

			// Bind.
			bufferSesGridData.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			bufferCellValidities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferCellHashs.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );

			workerReduceGrid.getProgram().use();

			workerReduceGrid.getProgram().setVec3u( "uGridSESSize", Vec3u( gridSES.size ) );
			workerReduceGrid.getProgram().setFloat( "uIsovalue", 0.f );

			// Start.
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			workerReduceGrid.start( gridSES.getCellCount() );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

			bufferSesGridData.unbind();
			bufferCellValidities.unbind();
			bufferCellHashs.unbind();

			chrono2.stop();
			VTX_DEBUG( "Grid reduced in {}s", chrono2.elapsedTime() );
			chrono2.start();

			////////////////////////////
			// Worker: grid compaction.
			Worker::GpuComputer workerGridCompaction( IO::FilePath( "ses/grid_compaction.comp" ) );

			VTX_DEBUG( "Grid buffer size before compaction: {}", bufferSize );

			bufferCellValidities.getData( validities );
			std::exclusive_scan( validities.begin(), validities.end(), validities.begin(), 0 );
			bufferCellValidities.setSub( validities );

			size_t bufferSizeReduced = validities[ bufferSize - 1 ];

			VTX_DEBUG( "Grid buffer size after compaction: {}", bufferSizeReduced );

			Buffer bufferCellHashsReduced( bufferSizeReduced * sizeof( uint ) );

			// Bind.
			bufferCellValidities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			bufferCellHashs.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferCellHashsReduced.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );

			workerGridCompaction.getProgram().use();
			workerGridCompaction.getProgram().setUInt( "uSize", uint( bufferSize ) );
			workerGridCompaction.getProgram().setUInt( "uSizeReduced", uint( bufferSizeReduced ) );

			// Start.
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			workerGridCompaction.start( gridSES.getCellCount() );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

			// Unbind.
			bufferCellValidities.unbind();
			bufferCellHashs.unbind();
			bufferCellHashsReduced.unbind();

			chrono2.stop();
			VTX_DEBUG( "Grid compacted in {}s", chrono2.elapsedTime() );
			chrono2.start();

			/////////////////////////
			// Worker: marching cube.
			Worker::GpuComputer workerMarchingCube( IO::FilePath( "ses/marching_cube.comp" ) );

			// Create SSBOs.
			// Output.
			// 5 triangles max per cell.
			bufferSize = bufferSizeReduced * 5 * 3;
			validities = std::vector<uint>( bufferSize, 0 );
			Buffer			  bufferPositionsTmp( bufferSize * sizeof( Vec4f ) );
			Buffer			  bufferAtomIndicesTmp( bufferSize * sizeof( uint ) );
			Buffer			  bufferTriangleValidities( validities, Buffer::Flags::DYNAMIC_STORAGE_BIT );
			std::vector<uint> trianglesPerAtom( atomPositions.size(), 0 );
			Buffer			  bufferTrianglesPerAtom( trianglesPerAtom );
			// Input.
			Buffer bufferTriangleTable( 256 * 16 * sizeof( int ), Math::MarchingCube::TRIANGLE_TABLE );

			bufferSesGridData.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			bufferPositionsTmp.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferAtomIndicesTmp.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );
			bufferTriangleValidities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 3 );
			bufferTriangleTable.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 4 );
			bufferCellHashsReduced.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 5 );
			bufferTrianglesPerAtom.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 6 );

			workerMarchingCube.getProgram().use();

			workerMarchingCube.getProgram().setVec3f( "uGridSESWorldOrigin", gridSES.worldOrigin );
			workerMarchingCube.getProgram().setVec3u( "uGridSESSize", Vec3u( gridSES.size ) );
			workerMarchingCube.getProgram().setVec3f( "uGridSESCellSize", gridSES.cellSize );
			workerMarchingCube.getProgram().setFloat( "uIsovalue", 0.f );
			workerMarchingCube.getProgram().setUInt( "uSize", uint( bufferSizeReduced ) );

			// Start.
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			workerMarchingCube.start( bufferSizeReduced );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

			// Unbind.
			bufferSesGridData.unbind();
			bufferPositionsTmp.unbind();
			bufferAtomIndicesTmp.unbind();
			bufferTriangleValidities.unbind();
			bufferTriangleTable.unbind();
			bufferCellHashsReduced.unbind();
			bufferTrianglesPerAtom.unbind();

			chrono2.stop();
			VTX_DEBUG( "Marching cube done in {}s", chrono2.elapsedTime() );
			chrono2.start();

			////////////////////////////
			// Worker: buffer compaction.
			Worker::GpuComputer workerBufferCompaction( IO::FilePath( "ses/buffer_compaction.comp" ) );
			VTX_DEBUG( "Triangle buffer size before compaction: {}", bufferSize );

			// Exclusive scan with std.
			bufferTriangleValidities.getData( validities );
			std::exclusive_scan( validities.begin(), validities.end(), validities.begin(), 0 );
			bufferSizeReduced = validities[ bufferSize - 1 ];

			_indiceCount = uint( bufferSizeReduced );

			assert( _indiceCount % 3 == 0 );

			VTX_DEBUG( "Triangle buffer size after compaction: {}", _indiceCount );

			bufferTrianglesPerAtom.getData( trianglesPerAtom );

			// Compute atom to triangles.
			_atomsToTriangles = std::vector<Range>( atomPositions.size(), Range { 0, 0 } );
			uint counter	  = 0;
			for ( uint i = 0; i < atomPositions.size(); ++i )
			{
				if ( trianglesPerAtom[ i ] == 0 )
				{
					continue;
				}

				const uint size		   = trianglesPerAtom[ i ];
				_atomsToTriangles[ i ] = { counter, size };
				counter += size;
			}
			_atomsToTriangles.shrink_to_fit();

			assert( counter == _indiceCount );

			// Create SSBOs.
			// Output.
			Buffer & bufferPositions	= _buffer->getBufferPositions();
			Buffer & bufferNormals		= _buffer->getBufferNormals();
			Buffer & bufferIndices		= _buffer->getBufferIndices();
			Buffer & bufferColors		= _buffer->getBufferColors();
			Buffer & bufferVisibilities = _buffer->getBufferVisibilities();
			Buffer & bufferIds			= _buffer->getBufferIds();
			Buffer & bufferSelections	= _buffer->getBufferSelections();

			// Create final buffers.
			bufferPositions.set( _indiceCount * sizeof( Vec4f ), Buffer::Flags::MAP_READ_BIT );
			bufferNormals.set( std::vector<Vec4f>( _indiceCount, Vec4f() ) );
			bufferIndices.set( _indiceCount * sizeof( uint ),
							   Buffer::Flags( Buffer::Flags::MAP_READ_BIT | Buffer::Flags::MAP_WRITE_BIT ) );
			bufferColors.set( _indiceCount * sizeof( Color::Rgba ) );
			bufferVisibilities.set( _indiceCount * sizeof( uint ) );
			bufferIds.set( _indiceCount * sizeof( uint ) );
			bufferSelections.set( _indiceCount * sizeof( uint ) );

			// Input.
			Buffer bufferAtomIds( _category->getMoleculePtr()->getBufferAtomIds() );
			Buffer bufferAtomToTriangle( _atomsToTriangles );

			// Bind.
			bufferPositions.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			bufferIndices.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferIds.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );
			bufferPositionsTmp.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 3 );
			bufferAtomIndicesTmp.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 4 );
			bufferTriangleValidities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 5 );
			bufferAtomToTriangle.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 6 );
			bufferAtomIds.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 7 );
			bufferTrianglesPerAtom.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 8 );

			workerBufferCompaction.getProgram().use();
			workerBufferCompaction.getProgram().setUInt( "uSize", uint( bufferSize ) );
			workerBufferCompaction.getProgram().setUInt( "uSizeReduced", uint( bufferSizeReduced ) );

			// Start.
			assert( bufferSize % 3 == 0 );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			workerBufferCompaction.start( bufferSize / 3 );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

			// Unbind.
			bufferPositions.unbind();
			bufferIndices.unbind();
			bufferIds.unbind();
			bufferPositionsTmp.unbind();
			bufferAtomIndicesTmp.unbind();
			bufferTriangleValidities.unbind();
			bufferAtomToTriangle.unbind();
			bufferAtomIds.unbind();
			bufferTrianglesPerAtom.unbind();

			chrono2.stop();
			VTX_DEBUG( "Buffer compacted in {}s", chrono2.elapsedTime() );
			chrono2.start();

			////////////////////////////
			// Weld vertices.
			std::vector<uint> sortedIndices( _indiceCount );

			Vec4f * const ptrPositions
				= bufferPositions.map<Vec4f>( 0, _indiceCount * sizeof( Vec4f ), Buffer::Flags::MAP_READ_BIT );
			uint * const ptrIndices = bufferIndices.map<uint>(
				0,
				_indiceCount * sizeof( uint ),
				Buffer::Flags( Buffer::Flags::MAP_READ_BIT | Buffer::Flags::MAP_WRITE_BIT ) );
			assert( ptrPositions != nullptr );
			assert( ptrIndices != nullptr );

			// Get permutations.
			auto compareVec4Function = []( const Vec4f & p_lhs, const Vec4f & p_rhs )
			{
				if ( p_lhs.x <= p_rhs.x && p_lhs.y <= p_rhs.y && p_lhs.z < p_rhs.z )
					return true;
				if ( p_lhs.x <= p_rhs.x && p_lhs.y < p_rhs.y )
					return true;
				if ( p_lhs.x < p_rhs.x )
					return true;

				return false;
			};

			std::vector<std::size_t> permutations( _indiceCount );
			std::iota( permutations.begin(), permutations.end(), 0 );
			std::sort( permutations.begin(),
					   permutations.end(),
					   [ & ]( std::size_t i, std::size_t j )
					   { return compareVec4Function( ptrPositions[ i ], ptrPositions[ j ] ); } );

			chrono2.stop();
			VTX_DEBUG( "Positions sorted in {}s", chrono2.elapsedTime() );
			chrono2.start();

			// Apply permutations.
			std::transform( permutations.begin(),
							permutations.end(),
							sortedIndices.begin(),
							[ & ]( std::size_t i ) { return ptrIndices[ i ]; } );

			chrono2.stop();
			VTX_DEBUG( "Permutations applied in {}s", chrono2.elapsedTime() );
			chrono2.start();

			// Detect duplicates.
			uint indexCurrent	  = sortedIndices[ 0 ];
			uint duplicateCounter = 0;
			for ( uint i = 1; i < _indiceCount; ++i )
			{
				const uint indexNext = sortedIndices[ i ];

				if ( Util::Math::length2( ptrPositions[ indexCurrent ] - ptrPositions[ indexNext ] )
					 < EPSILON * EPSILON )
				{
					ptrIndices[ indexNext ] = indexCurrent;
					duplicateCounter++;
				}
				else
				{
					indexCurrent = indexNext;
				}
			}

			bufferPositions.unmap();
			bufferIndices.unmap();

			// Amelioration:
			// Vertices are not removed.
			// So _indiceCount is still the size of the vertice buffer with useless index.
			// (need to recompute atomsToTriangle and other stuff)

			chrono2.stop();
			VTX_DEBUG( "{} duplicates over {} vertices detected in {}s",
					   duplicateCounter,
					   _indiceCount,
					   chrono2.elapsedTime() );
			chrono2.start();

#if VTX_SES_NORMALS_GPU
			{
				////////////////////////////
				// Worker: compute normals (sum).
				Worker::GpuComputer workerComputeNormals( IO::FilePath( "ses/compute_normals.comp" ) );
				Buffer				bufferNormalsCasted( std::vector<Vec4i>( _indiceCount, Vec4i() ) );

				// Bind.
				bufferPositions.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
				bufferNormals.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
				bufferIndices.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );
				bufferNormalsCasted.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 3 );

				workerComputeNormals.getProgram().use();
				workerComputeNormals.getProgram().setUInt( "uSize", _indiceCount );

				// Start.
				assert( _indiceCount % 3 == 0 );
				_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
				workerComputeNormals.start( _indiceCount / 3 );
				_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

				// Unbind.
				bufferPositions.unbind();
				bufferNormals.unbind();
				bufferIndices.unbind();
				bufferNormalsCasted.unbind();

				////////////////////////////
				// Worker: compute normals (divide).
				Worker::GpuComputer workerNormalizeNormals( IO::FilePath( "ses/normalize_normals.comp" ) );

				// Bind.
				bufferNormals.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
				bufferNormalsCasted.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );

				workerNormalizeNormals.getProgram().use();
				workerNormalizeNormals.getProgram().setUInt( "uSize", _indiceCount );

				// Start.
				_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
				workerNormalizeNormals.start( _indiceCount );
				_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

				// Unbind.
				bufferNormals.unbind();
				bufferNormalsCasted.unbind();
			}
#else
			{
				_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

				ptrPositions
					= bufferPositions.map<Vec4f>( 0, _indiceCount * sizeof( Vec4f ), Buffer::Flags::MAP_READ_BIT );
				ptrIndices = bufferIndices.map<uint>( 0, _indiceCount * sizeof( uint ), Buffer::Flags::MAP_READ_BIT );
				Vec4f * const ptrNormals
					= bufferNormals.map<Vec4f>( 0, _indiceCount * sizeof( Vec4f ), Buffer::Flags::MAP_WRITE_BIT );
				assert( ptrPositions != nullptr );
				assert( ptrIndices != nullptr );
				assert( ptrNormals != nullptr );

				for ( uint i = 0; i < _indiceCount - 2; i += 3 )
				{
					Vec3f normal = Util::Math::cross(
						Vec3f( ptrPositions[ ptrIndices[ i + 1 ] ] - ptrPositions[ ptrIndices[ i + 2 ] ] ),
						Vec3f( ptrPositions[ ptrIndices[ i + 1 ] ] - ptrPositions[ ptrIndices[ i + 0 ] ] ) );

					assert( Util::Math::length( normal ) != 0.f );
					Util::Math::normalizeSelf( normal );

					for ( uint j = 0; j < 3; ++j )
					{
						ptrNormals[ ptrIndices[ i + j ] ].x += normal.x;
						ptrNormals[ ptrIndices[ i + j ] ].y += normal.y;
						ptrNormals[ ptrIndices[ i + j ] ].z += normal.z;
					}
				}

				for ( uint i = 0; i < _indiceCount; ++i )
				{
					Util::Math::normalizeSelf( ptrNormals[ i ] );
				}

				bufferPositions.unmap();
				bufferIndices.unmap();
				bufferNormals.unmap();

				_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			}
#endif

			chrono2.stop();
			VTX_DEBUG( "Normals computed in {}s", chrono2.elapsedTime() );

			_buffer->doneContextCurrent();

			////////////////////////////
			// Refresh other data.
			// refreshColors();
			refreshVisibilities();
			refreshSelections();

			// No CPU data.
			assert( _vertices.empty() );
			assert( _indices.empty() );
			assert( _colors.empty() );
			assert( _normals.empty() );
			assert( _visibilities.empty() );
			assert( _selections.empty() );

			// Only atoms to triangle indices mapping.
			assert( _atomsToTriangles.empty() == false );

			chrono.stop();
			VTX_DEBUG( "SES created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void SolventExcludedSurface::refreshColors()
		{
			Tool::Chrono chrono;
			chrono.start();

			using VTX::Renderer::GL::Buffer;
			_buffer->makeContextCurrent();

			Buffer & bufferColors	  = _buffer->getBufferColors();
			Buffer & bufferIndices	  = _buffer->getBufferIndices();
			Buffer & bufferAtomColors = _category->getMoleculePtr()->getBuffer()->getBufferColors();
			Buffer	 bufferAtomsToTriangles( _atomsToTriangles );
			Buffer	 bufferCounters( _indiceCount * sizeof( uint ) );
			Buffer	 bufferColorsUint( _indiceCount * sizeof( Vec4u ) );

			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			VTX_WORKER( new Worker::GpuBufferInitializer( bufferCounters, _indiceCount ) );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			VTX_WORKER(
				new Worker::GpuBufferInitializer( bufferColorsUint, _indiceCount, "uvec4", "std140", "uvec4(0)" ) );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

			bufferColors.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			bufferIndices.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferAtomColors.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );
			bufferAtomsToTriangles.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 3 );
			bufferCounters.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 4 );
			bufferColorsUint.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 5 );

			Worker::GpuComputer worker( IO::FilePath( "ses/apply_color.comp" ) );

			worker.getProgram().use();
			worker.getProgram().setUInt( "uSize", uint( _atomsToTriangles.size() ) );
			worker.start( _atomsToTriangles.size() );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

			bufferColors.unbind();
			bufferIndices.unbind();
			bufferAtomColors.unbind();
			bufferAtomsToTriangles.unbind();
			bufferCounters.unbind();
			bufferColorsUint.unbind();

			bufferColors.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			bufferCounters.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferColorsUint.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );

			worker = Worker::GpuComputer( IO::FilePath( "ses/apply_color_divide.comp" ) );

			worker.getProgram().use();
			worker.getProgram().setUInt( "uSize", _indiceCount );
			worker.start( _indiceCount );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

			bufferColors.unbind();
			bufferCounters.unbind();
			bufferColorsUint.unbind();

			_buffer->doneContextCurrent();

			chrono.stop();
			VTX_DEBUG( "SES color refreshed in {}s", chrono.elapsedTime() );
		}

		void SolventExcludedSurface::refreshVisibilities()
		{
			Tool::Chrono chrono;
			chrono.start();

			using VTX::Renderer::GL::Buffer;
			_buffer->makeContextCurrent();

			Buffer & bufferVisibilities		= _buffer->getBufferVisibilities();
			Buffer & bufferIndices			= _buffer->getBufferIndices();
			Buffer & bufferAtomVisibilities = _category->getMoleculePtr()->getBuffer()->getBufferVisibilities();
			Buffer	 bufferAtomsToTriangles( _atomsToTriangles );

			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			VTX_WORKER( new Worker::GpuBufferInitializer( bufferVisibilities, _indiceCount ) );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

			bufferVisibilities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			bufferIndices.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferAtomVisibilities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );
			bufferAtomsToTriangles.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 3 );

			Worker::GpuComputer worker( IO::FilePath( "ses/apply_visibility.comp" ) );

			worker.getProgram().use();
			worker.getProgram().setUInt( "uSize", uint( _atomsToTriangles.size() ) );
			worker.start( _atomsToTriangles.size() );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

			bufferVisibilities.unbind();
			bufferIndices.unbind();
			bufferAtomVisibilities.unbind();
			bufferAtomsToTriangles.unbind();

			_buffer->doneContextCurrent();

			chrono.stop();
			VTX_DEBUG( "SES visibility refreshed in {}s", chrono.elapsedTime() );
		}

		void SolventExcludedSurface::refreshSelections()
		{
			Tool::Chrono chrono;
			chrono.start();

			using VTX::Renderer::GL::Buffer;
			_buffer->makeContextCurrent();

			Buffer & bufferSelections	  = _buffer->getBufferSelections();
			Buffer & bufferIndices		  = _buffer->getBufferIndices();
			Buffer & bufferAtomSelections = _category->getMoleculePtr()->getBuffer()->getBufferSelections();
			Buffer	 bufferAtomsToTriangles( _atomsToTriangles );

			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			VTX_WORKER( new Worker::GpuBufferInitializer( bufferSelections, _indiceCount ) );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

			bufferSelections.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			bufferIndices.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferAtomSelections.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );
			bufferAtomsToTriangles.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 3 );

			Worker::GpuComputer worker( IO::FilePath( "ses/apply_selection.comp" ) );

			worker.getProgram().use();
			worker.getProgram().setUInt( "uSize", uint( _atomsToTriangles.size() ) );
			worker.start( _atomsToTriangles.size() );
			_buffer->memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

			bufferSelections.unbind();
			bufferIndices.unbind();
			bufferAtomSelections.unbind();
			bufferAtomsToTriangles.unbind();

			_buffer->doneContextCurrent();

			chrono.stop();
			VTX_DEBUG( "SES selection refreshed in {}s", chrono.elapsedTime() );
		}

		void SolventExcludedSurface::_instantiate3DViews()
		{
			_addRenderable(
				MVC::MvcManager::get().instantiateView<View::D3::TriangleSES>( this, VTX::ID::View::D3_TRIANGLE_SES ) );
		}
	} // namespace Model
} // namespace VTX
