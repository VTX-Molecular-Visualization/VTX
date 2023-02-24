#include "solvent_excluded_surface.hpp"
#include "atom.hpp"
#include "category.hpp"
#include "chain.hpp"
#include "math/marching_cube.hpp"
#include "molecule.hpp"
#include "object3d/helper/aabb.hpp"
#include "object3d/scene.hpp"
#include "renderer/gl/buffer.hpp"
#include "residue.hpp"
#include "selection/selection_manager.hpp"
#include "view/d3/triangle.hpp"
#include "worker/gpu_computer.hpp"

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

		void SolventExcludedSurface::_init()
		{
			refresh();
			refreshSelection( VTX::Selection::SelectionManager::get().getSelectionModel().getMoleculeMap(
				*_category->getMoleculePtr() ) );
		}

		void SolventExcludedSurface::refresh()
		{
			_mode = Mode::GPU;

			if ( _category->isEmpty() )
			{
				return;
			}

			switch ( _mode )
			{
			case SolventExcludedSurface::Mode::CPU: _refreshCPU(); break;
			case SolventExcludedSurface::Mode::GPU:
			{
				_buffer->makeContextCurrent();
				_refreshGPU();
				_buffer->doneContextCurrent();
				break;
			}
			default: break;
			}
		}

		// TODO
		// - Decompose in multiple iterations to handle larger buffers.
		// - Smooth normals on GPU.
		void SolventExcludedSurface::_refreshGPU()
		{
			Tool::Chrono chrono, chrono2;
			chrono.start();
			chrono2.start();
			VTX_DEBUG( "Creating SES..." );

			const std::vector<uint> atomsIdx = _category->generateAtomIndexList();

			// Sort atoms in acceleration grid.
			const float maxVdWRadius = *std::max_element(
				Atom::SYMBOL_VDW_RADIUS, Atom::SYMBOL_VDW_RADIUS + std::size( Atom::SYMBOL_VDW_RADIUS ) );
			const Object3D::Helper::AABB & molAABB = _category->getMoleculePtr()->getAABB();

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
			for ( uint idx : atomsIdx )
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
			VTX_DEBUG( "Atoms sorted in " + std::to_string( chrono2.elapsedTime() ) + "s" );
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
			workerCreateSDF.start( gridSES.getCellCount(), GL_SHADER_STORAGE_BARRIER_BIT );

			// Unbind.
			bufferSesGridData.unbind();
			bufferAtomGridDataSorted.unbind();
			bufferAtomIndexSorted.unbind();
			bufferAtomPosition.unbind();

			chrono2.stop();
			VTX_DEBUG( "SDF created in " + std::to_string( chrono2.elapsedTime() ) + "s" );
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
			workerRefineSDF.start( gridSES.getCellCount(), GL_SHADER_STORAGE_BARRIER_BIT );

			// Unbind.
			bufferSesGridData.unbind();

			chrono2.stop();
			VTX_DEBUG( "SDF boundary created in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			////////////////////////////
			// Worker: reduce grid.
			Worker::GpuComputer workerReduceGrid( IO::FilePath( "ses/reduce_grid.comp" ) );

			std::vector<uint> cellValidities( gridSES.getCellCount(), 0 );
			Buffer			  bufferCellValidities( cellValidities );
			Buffer			  bufferCellHashs( gridSES.getCellCount() * sizeof( uint ) );

			// Bind.
			bufferSesGridData.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			bufferCellValidities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferCellHashs.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );

			workerReduceGrid.getProgram().use();

			workerReduceGrid.getProgram().setVec3u( "uGridSESSize", Vec3u( gridSES.size ) );
			workerReduceGrid.getProgram().setFloat( "uIsovalue", 0.f );

			// Start.
			workerReduceGrid.start( gridSES.getCellCount(), GL_SHADER_STORAGE_BARRIER_BIT );

			bufferCellValidities.getData( cellValidities );

			bufferSesGridData.unbind();
			bufferCellValidities.unbind();
			bufferCellHashs.unbind();

			chrono2.stop();
			VTX_DEBUG( "Grid reduced in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			////////////////////////////
			// Worker: grid compaction.
			Worker::GpuComputer workerGridCompaction( IO::FilePath( "ses/grid_compaction.comp" ) );
			size_t				bufferSize = gridSES.getCellCount();
			VTX_DEBUG( "Grid buffer size before compaction: {}", bufferSize );
			std::exclusive_scan( cellValidities.begin(), cellValidities.end(), cellValidities.begin(), 0 );
			size_t bufferSizeReduced = *( cellValidities.end() - 1 );

			VTX_DEBUG( "Grid buffer size after compaction: {}", bufferSizeReduced );

			Buffer bufferCellValiditiesSum( cellValidities );
			Buffer bufferCellHashsReduced( bufferSizeReduced * sizeof( uint ) );

			// Bind.
			bufferCellValidities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			bufferCellValiditiesSum.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferCellHashs.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );
			bufferCellHashsReduced.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 3 );

			workerGridCompaction.getProgram().use();
			workerGridCompaction.getProgram().setUInt( "uSize", uint( bufferSize ) );
			workerGridCompaction.getProgram().setUInt( "uSizeReduced", uint( bufferSizeReduced ) );

			// Start.
			workerGridCompaction.start( gridSES.getCellCount(), GL_SHADER_STORAGE_BARRIER_BIT );

			// Unbind.
			bufferCellValidities.unbind();
			bufferCellValiditiesSum.unbind();
			bufferCellHashs.unbind();
			bufferCellHashsReduced.unbind();

			chrono2.stop();
			VTX_DEBUG( "Grid compacted in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			/////////////////////////
			// Worker: marching cube.
			Worker::GpuComputer workerMarchingCube( IO::FilePath( "ses/marching_cube.comp" ) );

			// Create SSBOs.
			// Output.
			// 5 triangles max per cell.
			bufferSize = bufferSizeReduced * 5 * 3;
			Buffer			  bufferPositionsTmp( bufferSize * sizeof( Vec4f ) );
			Buffer			  bufferAtomIndicesTmp( bufferSize * sizeof( uint ) );
			std::vector<uint> triangleValidities( bufferSize, 0 );
			Buffer			  bufferTriangleValidities( triangleValidities );
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
			workerMarchingCube.start( bufferSizeReduced, GL_SHADER_STORAGE_BARRIER_BIT );

			// Get validities for next step.
			bufferTriangleValidities.getData( triangleValidities );
			bufferTrianglesPerAtom.getData( trianglesPerAtom );

			// Unbind.
			bufferSesGridData.unbind();
			bufferPositionsTmp.unbind();
			bufferAtomIndicesTmp.unbind();
			bufferTriangleValidities.unbind();
			bufferTriangleTable.unbind();
			bufferCellHashsReduced.unbind();
			bufferTrianglesPerAtom.unbind();

			chrono2.stop();
			VTX_DEBUG( "Marching cube done in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			////////////////////////////
			// Worker: buffer compaction.
			Worker::GpuComputer workerStreamCompaction( IO::FilePath( "ses/buffer_compaction.comp" ) );
			VTX_DEBUG( "Triangle buffer size before compaction: {}", bufferSize );
			//  Perform exclusive scan on validity buffer.
			std::exclusive_scan( triangleValidities.begin(), triangleValidities.end(), triangleValidities.begin(), 0 );

			bufferSizeReduced = *( triangleValidities.end() - 1 );
			_indiceCount	  = uint( bufferSizeReduced );

			assert( _indiceCount % 3 == 0 );

			VTX_DEBUG( "Triangle buffer size after compaction: {}", _indiceCount );

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

			if ( _isInit == false )
			{
				// Create final buffers.
				bufferPositions.set( _indiceCount * sizeof( Vec4f ), Buffer::Flags::MAP_READ_BIT );
				bufferNormals.set( _indiceCount * sizeof( Vec4f ), Buffer::Flags::MAP_WRITE_BIT );
				bufferIndices.set( _indiceCount * sizeof( uint ),
								   Buffer::Flags( Buffer::Flags::MAP_READ_BIT | Buffer::Flags::MAP_WRITE_BIT ) );
				bufferColors.set( _indiceCount * sizeof( Color::Rgba ), Buffer::Flags::MAP_WRITE_BIT );
				bufferVisibilities.set( _indiceCount * sizeof( uint ), Buffer::Flags::DYNAMIC_STORAGE_BIT );
				bufferIds.set( _indiceCount * sizeof( uint ) );
				bufferSelections.set( _indiceCount * sizeof( uint ), Buffer::Flags::DYNAMIC_STORAGE_BIT );
			}

			// Input.
			Buffer bufferAtomColors( _category->getMoleculePtr()->getBufferAtomColors() );
			Buffer bufferAtomVisibilities( _category->getMoleculePtr()->getBufferAtomVisibilities() );
			Buffer bufferAtomIds( _category->getMoleculePtr()->getBufferAtomIds() );
			Buffer bufferAtomToTriangle( _atomsToTriangles );

			// Bind.
			bufferPositions.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			bufferIndices.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferNormals.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );
			bufferColors.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 3 );
			bufferVisibilities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 4 );
			bufferIds.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 5 );
			bufferPositionsTmp.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 6 );
			bufferAtomIndicesTmp.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 7 );
			bufferTriangleValidities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 8 );
			bufferAtomToTriangle.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 9 );
			bufferAtomColors.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 10 );
			bufferAtomVisibilities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 11 );
			bufferAtomIds.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 12 );
			bufferTrianglesPerAtom.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 13 );

			workerStreamCompaction.getProgram().use();
			workerStreamCompaction.getProgram().setUInt( "uSize", uint( bufferSize ) );
			workerStreamCompaction.getProgram().setUInt( "uSizeReduced", uint( bufferSizeReduced ) );

			// Start.
			assert( bufferSize % 3 == 0 );
			workerStreamCompaction.start( bufferSize / 3, GL_SHADER_STORAGE_BARRIER_BIT );

			// Unbind.
			bufferPositions.unbind();
			bufferIndices.unbind();
			bufferNormals.unbind();
			bufferColors.unbind();
			bufferVisibilities.unbind();
			bufferIds.unbind();
			bufferPositionsTmp.unbind();
			bufferAtomIndicesTmp.unbind();
			bufferAtomToTriangle.unbind();
			bufferAtomColors.unbind();
			bufferAtomVisibilities.unbind();
			bufferAtomIds.unbind();
			bufferTrianglesPerAtom.unbind();

			chrono2.stop();
			VTX_DEBUG( "Buffer compacted in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			////////////////////////////
			// Weld vertices.
			std::vector<uint> sortedIndices( _indiceCount );

			Vec4f * ptrPositions
				= bufferPositions.map<Vec4f>( 0, _indiceCount * sizeof( Vec4f ), Buffer::Flags::MAP_READ_BIT );
			uint * ptrIndices = bufferIndices.map<uint>(
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
			VTX_DEBUG( "Positions sorted in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			// Apply permutations.
			std::transform( permutations.begin(),
							permutations.end(),
							sortedIndices.begin(),
							[ & ]( std::size_t i ) { return ptrIndices[ i ]; } );

			chrono2.stop();
			VTX_DEBUG( "Permutations applied in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			// Detect duplicates.
			uint indexCurrent = sortedIndices[ 0 ];
			for ( uint i = 1; i < _indiceCount; ++i )
			{
				const uint indexNext = sortedIndices[ i ];

				if ( Util::Math::length2( ptrPositions[ indexCurrent ] - ptrPositions[ indexNext ] )
					 < EPSILON * EPSILON )
				{
					ptrIndices[ indexNext ] = indexCurrent;
				}
				else
				{
					indexCurrent = indexNext;
				}
			}

			bufferPositions.unmap();
			bufferIndices.unmap();

			chrono2.stop();
			VTX_DEBUG( "Duplicates detected in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

#if VTX_SES_NORMALS_GPU
			{
				////////////////////////////
				// Worker: compute normals (sum).
				Worker::GpuComputer workerComputeNormalsSum( IO::FilePath( "ses/compute_normals_sum.comp" ) );
				Buffer				bufferNormalsCasted( std::vector<Vec4i>( _indiceCount, Vec4i() ) );

				// Bind.
				bufferPositions.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
				bufferNormals.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
				bufferIndices.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );
				bufferNormalsCasted.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 3 );

				workerComputeNormalsSum.getProgram().use();
				workerComputeNormalsSum.getProgram().setUInt( "uSize", _indiceCount );

				// Start.
				assert( _indiceCount % 3 == 0 );
				workerComputeNormalsSum.setBarrierPre( GL_SHADER_STORAGE_BARRIER_BIT );
				workerComputeNormalsSum.start( _indiceCount / 3, GL_SHADER_STORAGE_BARRIER_BIT );

				// Unbind.
				bufferPositions.unbind();
				bufferNormals.unbind();
				bufferIndices.unbind();
				bufferNormalsCasted.unbind();

				////////////////////////////
				// Worker: compute normals (divide).
				Worker::GpuComputer workerComputeNormalsDivide( IO::FilePath( "ses/compute_normals_divide.comp" ) );

				// Bind.
				bufferNormals.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
				bufferNormalsCasted.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );

				workerComputeNormalsDivide.getProgram().use();
				workerComputeNormalsDivide.getProgram().setUInt( "uSize", _indiceCount );

				// Start.
				workerComputeNormalsDivide.start( _indiceCount, GL_SHADER_STORAGE_BARRIER_BIT );

				// Unbind.
				bufferNormals.unbind();
				bufferNormalsCasted.unbind();
			}
#else
			{
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
			}
#endif

			chrono2.stop();
			VTX_DEBUG( "Normals computed in {}s", chrono2.elapsedTime() );
			chrono.stop();
			VTX_DEBUG( "SES created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void SolventExcludedSurface::_refreshCPU()
		{
			Tool::Chrono chrono, chrono2;
			chrono.start();
			chrono2.start();
			VTX_DEBUG( "Creating SES..." );

			_vertices.clear();
			_normals.clear();
			_indices.clear();
			_ids.clear();
			_colors.clear();
			_visibilities.clear();
			_atomsToTriangles.clear();

			const std::vector<uint> atomsIdx = _category->generateAtomIndexList();

			// Sort atoms in acceleration grid.
			const float maxVdWRadius = *std::max_element(
				Atom::SYMBOL_VDW_RADIUS, Atom::SYMBOL_VDW_RADIUS + std::size( Atom::SYMBOL_VDW_RADIUS ) );
			const Object3D::Helper::AABB & molAABB = _category->getAABB();

			const float atomGridCellSize = PROBE_RADIUS + maxVdWRadius;
			const Vec3f gridMin			 = molAABB.getMin() - atomGridCellSize;
			const Vec3f gridMax			 = molAABB.getMax() + atomGridCellSize;

			const Vec3f gridSize = gridMax - gridMin;
			Vec3i		atomGridSize( Util::Math::ceil( gridSize / atomGridCellSize ) );

			Object3D::Helper::Grid gridAtoms( gridMin, Vec3f( atomGridCellSize ), atomGridSize );

			const std::vector<Vec3f> & atomPositions = _category->getMoleculePtr()->getCurrentAtomPositionFrame();

			std::vector<std::vector<uint>> atomGridData2D( gridAtoms.getCellCount(), std::vector<uint>() );

			// Store atom indices in acceleration grid.
			for ( uint i : atomsIdx )
			{
				const uint hash = gridAtoms.gridHash( atomPositions[ i ] );

				if ( hash < atomGridData2D.size() )
				{
					atomGridData2D[ hash ].emplace_back( i );
				}
			}

			chrono2.stop();
			VTX_DEBUG( "Atoms sorted in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			// Compute SES grid and compute SDF.
			Vec3i sesGridSize( Util::Math::ceil( gridSize / VOXEL_SIZE ) );

			Object3D::Helper::Grid gridSES( gridMin, Vec3f( VOXEL_SIZE ), sesGridSize );

			// SES grid data.
			std::vector<SESGridData> sesGridData( gridSES.getCellCount(), SESGridData { PROBE_RADIUS, -1 } );

			// Loop over cells.
			std::vector<uint> boundaryIndex = std::vector<uint>();
			for ( uint sesGridHash = 0; sesGridHash < gridSES.getCellCount(); ++sesGridHash )
			{
				// Get corresponding ses grid data.
				const Vec3i	  sesGridPosition = gridSES.gridPosition( sesGridHash );
				SESGridData & gridData		  = sesGridData[ sesGridHash ];

				// Get corresponding acceleration grid cell hash.
				const Vec3f sesGridCellWorldPosition = gridSES.worldPosition( sesGridPosition );
				const Vec3i atomGridPosition		 = gridAtoms.gridPosition( sesGridCellWorldPosition );

				// Loop over the 27 cells to visit.
				float minDistance = FLOAT_MAX;
				bool  found		  = false;

				for ( int ox = -1; ox <= 1 && !found; ++ox )
				{
					for ( int oy = -1; oy <= 1 && !found; ++oy )
					{
						for ( int oz = -1; oz <= 1 && !found; ++oz )
						{
							Vec3f offset( ox, oy, oz );
							Vec3i gridPositionToVisit( Vec3f( atomGridPosition ) + offset );
							uint  hashToVisit = gridAtoms.gridHash( Vec3i( gridPositionToVisit ) );

							if ( hashToVisit >= atomGridData2D.size() )
							{
								continue;
							}

							// Compute SDF.
							for ( const uint index : atomGridData2D[ hashToVisit ] )
							{
								if ( _category->getMoleculePtr()->getAtom( index ) == nullptr )
								{
									continue;
								}

								float distance
									= Util::Math::distance( atomPositions[ index ], sesGridCellWorldPosition );

								// Inside.
								if ( distance < VOXEL_SIZE )
								{
									gridData.sdf		 = -VOXEL_SIZE;
									gridData.nearestAtom = -1;
									found				 = true;
									// Don't need to loop over other cells.
									break;
								}
								// Boundary.
								else
								{
									distance -= ( PROBE_RADIUS
												  + _category->getMoleculePtr()->getAtom( index )->getVdwRadius() );
									if ( distance < 0.f )
									{
										gridData.sdf = -VOXEL_SIZE;
										if ( distance < minDistance )
										{
											minDistance			 = distance;
											gridData.nearestAtom = index;
										}
									}
								}
							}
						}
					}
				}
			}

			chrono2.stop();
			VTX_DEBUG( "SDF created " + std::to_string( chrono2.elapsedTime() ) + "s" );

			chrono2.start();

			// SDF refinement.
			Vec3i cellsToVisitCount = Util::Math::ceil( Vec3f( PROBE_RADIUS + VOXEL_SIZE ) / gridSES.cellSize );

			// for ( uint sesGridHash : boundaryIndex ) // Why this is slower?
			for ( uint sesGridHash = 0; sesGridHash < gridSES.getCellCount(); ++sesGridHash )
			{
				// Get corresponding ses grid data.
				const Vec3i	  sesGridPosition = gridSES.gridPosition( sesGridHash );
				SESGridData & gridData		  = sesGridData[ sesGridHash ];

				// Not in boundary.
				if ( gridData.nearestAtom == -1 )
				{
					continue;
				}

				const Vec3f sesWorldPosition = gridSES.worldPosition( sesGridPosition );

				float minDistanceWithOutsidePoint = FLOAT_MAX;
				bool  found						  = false;
				for ( int ox = -cellsToVisitCount.x; ox <= cellsToVisitCount.x; ++ox )
				{
					for ( int oy = -cellsToVisitCount.y; oy <= cellsToVisitCount.y; ++oy )
					{
						for ( int oz = -cellsToVisitCount.z; oz <= cellsToVisitCount.z; ++oz )
						{
							Vec3f		offset				= Vec3f( ox, oy, oz );
							const Vec3i gridPositionToVisit = Vec3i( Vec3f( sesGridPosition ) + offset );
							const uint	hashToVisit			= gridSES.gridHash( gridPositionToVisit );

							if ( hashToVisit >= sesGridData.size() )
							{
								continue;
							}

							const Vec3f	  worldPositionToVisit = gridSES.worldPosition( gridPositionToVisit );
							SESGridData & gridDataToVisit	   = sesGridData[ hashToVisit ];

							// If outside.
							if ( gridDataToVisit.sdf == PROBE_RADIUS )
							{
								const float distance = Util::Math::distance( worldPositionToVisit, sesWorldPosition );
								if ( distance < minDistanceWithOutsidePoint )
								{
									minDistanceWithOutsidePoint = distance;
								}
								found = true;
							}
						}
					}
				}

				if ( found )
				{
					gridData.sdf = PROBE_RADIUS - minDistanceWithOutsidePoint;
				}
			}

			chrono2.stop();

			VTX_DEBUG( "SDF boundary created " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			std::vector<std::vector<Vec4f>> atomsToTriangles( atomPositions.size(), std::vector<Vec4f>() );

			// Marching cube to extract mesh.
			const Math::MarchingCube marchingCube = Math::MarchingCube();
			for ( uint x = 0; x < uint( gridSES.size.x ) - 1; ++x )
			{
				for ( uint y = 0; y < uint( gridSES.size.y ) - 1; ++y )
				{
					for ( uint z = 0; z < uint( gridSES.size.z ) - 1; ++z )
					{
						SESGridData gridData[ 8 ] = { sesGridData[ gridSES.gridHash( Vec3i( x, y, z ) ) ],
													  sesGridData[ gridSES.gridHash( Vec3i( x + 1, y, z ) ) ],
													  sesGridData[ gridSES.gridHash( Vec3i( x + 1, y, z + 1 ) ) ],
													  sesGridData[ gridSES.gridHash( Vec3i( x, y, z + 1 ) ) ],
													  sesGridData[ gridSES.gridHash( Vec3i( x, y + 1, z ) ) ],
													  sesGridData[ gridSES.gridHash( Vec3i( x + 1, y + 1, z ) ) ],
													  sesGridData[ gridSES.gridHash( Vec3i( x + 1, y + 1, z + 1 ) ) ],
													  sesGridData[ gridSES.gridHash( Vec3i( x, y + 1, z + 1 ) ) ] };

						Math::MarchingCube::GridCell cell
							= { { gridSES.worldPosition( Vec3i( x, y, z ) ),
								  { gridSES.worldPosition( Vec3i( x + 1, y, z ) ) },
								  { gridSES.worldPosition( Vec3i( x + 1, y, z + 1 ) ) },
								  { gridSES.worldPosition( Vec3i( x, y, z + 1 ) ) },
								  { gridSES.worldPosition( Vec3i( x, y + 1, z ) ) },
								  { gridSES.worldPosition( Vec3i( x + 1, y + 1, z ) ) },
								  { gridSES.worldPosition( Vec3i( x + 1, y + 1, z + 1 ) ) },
								  { gridSES.worldPosition( Vec3i( x, y + 1, z + 1 ) ) } },
								{ gridData[ 0 ].sdf,
								  gridData[ 1 ].sdf,
								  gridData[ 2 ].sdf,
								  gridData[ 3 ].sdf,
								  gridData[ 4 ].sdf,
								  gridData[ 5 ].sdf,
								  gridData[ 6 ].sdf,
								  gridData[ 7 ].sdf } };

						std::vector<std::vector<Vec4f>> cellTriangles = marchingCube.triangulateCell( cell, 0 );
						for ( std::vector<Vec4f> & cellTriangle : cellTriangles )
						{
							assert( cellTriangle.size() == 3 );

							// Get closest atom.
							float closestDistance = FLOAT_MAX;
							uint  closestVertex	  = 0;
							Vec3f centroid = Vec3f( cellTriangle[ 0 ] + cellTriangle[ 1 ] + cellTriangle[ 2 ] ) / 3.f;
							for ( uint vertex = 0; vertex < 8; ++vertex )
							{
								float distance = Util::Math::distance( centroid, cell.vertex[ vertex ] );
								if ( distance < closestDistance )
								{
									closestVertex	= vertex;
									closestDistance = distance;
								}
							}

							// Map atoms with triangle points.
							std::vector<Vec4f> & triangles = atomsToTriangles[ gridData[ closestVertex ].nearestAtom ];
							triangles.insert( triangles.end(), cellTriangle.begin(), cellTriangle.end() );
						}
					}
				}
			}

			chrono2.stop();
			VTX_DEBUG( "Marching cube done in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			// Fill buffers with sorted values and store data as triangle range per atoms.
			_atomsToTriangles = std::vector<Range>( atomPositions.size(), Range { 0, 0 } );
			for ( uint i = 0; i < atomsToTriangles.size(); ++i )
			{
				if ( _category->getMoleculePtr()->getAtom( i ) == nullptr )
				{
					continue;
				}

				const std::vector<Vec4f> & trianglePoints = atomsToTriangles[ i ];

				_atomsToTriangles[ i ].first = uint( _vertices.size() );
				_atomsToTriangles[ i ].count = uint( trianglePoints.size() );

				std::vector<uint> indices = std::vector<uint>( trianglePoints.size() );
				int				  index	  = int( _vertices.size() );
				std::iota( indices.begin(), indices.end(), index );
				_indices.insert( _indices.end(), indices.begin(), indices.end() );

				_vertices.insert( _vertices.end(), trianglePoints.begin(), trianglePoints.end() );

				std::vector<uint> ids = std::vector<uint>( trianglePoints.size() );
				std::fill( ids.begin(), ids.end(), _category->getMoleculePtr()->getAtom( i )->getId() );
				_ids.insert( _ids.end(), ids.begin(), ids.end() );
			}

			chrono2.stop();
			VTX_DEBUG( "Triangles sorting done in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();
			// toIndexed( _vertices, _indices );
			chrono2.stop();
			VTX_DEBUG( "Mesh to indexed computed in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();
			computeNormals( _vertices, _indices, _normals );
			chrono2.stop();
			VTX_DEBUG( "Normals computed in " + std::to_string( chrono2.elapsedTime() ) + "s" );

			_indiceCount = uint( _indices.size() );

			refreshColors();
			refreshVisibilities();

			_buffer->setPositions( _vertices );
			_buffer->setNormals( _normals );
			_buffer->setIds( _ids );
			_buffer->setIndices( _indices );

			_vertices.clear();
			_normals.clear();
			_ids.clear();
			_indices.clear();

			_vertices.shrink_to_fit();
			_normals.shrink_to_fit();
			_ids.shrink_to_fit();
			_indices.shrink_to_fit();

			_atomsToTriangles.shrink_to_fit();

			chrono.stop();
			VTX_DEBUG( "SES created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		// TODO: check if it is still needed after creation.
		void SolventExcludedSurface::refreshColors()
		{
			if ( _mode == Mode::CPU )
			{
				_colors.resize( _indiceCount, Color::Rgba::WHITE );

				for ( uint atomIdx = 0; atomIdx < _atomsToTriangles.size(); ++atomIdx )
				{
					const Atom * const atom = _category->getMoleculePtr()->getAtom( atomIdx );
					if ( atom == nullptr )
					{
						continue;
					}

					const Color::Rgba & color = _category->getMoleculePtr()->getAtomColor( atomIdx );
					std::fill(
						_colors.begin() + _atomsToTriangles[ atomIdx ].first,
						_colors.begin() + _atomsToTriangles[ atomIdx ].first + _atomsToTriangles[ atomIdx ].count,
						color );
				}
				_buffer->setColors( _colors );
				_colors.clear();
				_colors.shrink_to_fit();
			}
			else
			{
				using VTX::Renderer::GL::Buffer;
				_buffer->makeContextCurrent();
				const Buffer & bufferColor = _buffer->getBufferColors();

				Color::Rgba * const ptr = bufferColor.map<Color::Rgba>( Buffer::Access::WRITE_ONLY );

				for ( uint atomIdx = 0; atomIdx < _atomsToTriangles.size(); ++atomIdx )
				{
					const Atom * const atom = _category->getMoleculePtr()->getAtom( atomIdx );
					if ( atom == nullptr )
					{
						continue;
					}

					const Color::Rgba & color = _category->getMoleculePtr()->getAtomColor( atomIdx );

					// TODO: optimize that.
					for ( uint i = 0; i < _atomsToTriangles[ atomIdx ].count; ++i )
					{
						ptr[ _atomsToTriangles[ atomIdx ].first + i ] = color;
					}
				}

				bufferColor.unmap();
				_buffer->doneContextCurrent();
			}
		}

		void SolventExcludedSurface::refreshVisibilities()
		{
			using VTX::Renderer::GL::Buffer;

			if ( _mode == Mode::GPU )
			{
				_buffer->makeContextCurrent();
			}

			_visibilities.resize( _indiceCount, 1 );
			const Buffer & bufferVisibilities = _buffer->getBufferVisibilities();

			std::vector<uint> & visitibilities = _category->getMoleculePtr()->getBufferAtomVisibilities();

			for ( uint atomIdx = 0; atomIdx < _atomsToTriangles.size(); ++atomIdx )
			{
				const Atom * const atom = _category->getMoleculePtr()->getAtom( atomIdx );
				if ( atom == nullptr )
				{
					continue;
				}

				if ( visitibilities[ atomIdx ] == 0 )
				{
					std::fill(
						_visibilities.begin() + _atomsToTriangles[ atomIdx ].first,
						_visibilities.begin() + _atomsToTriangles[ atomIdx ].first + _atomsToTriangles[ atomIdx ].count,
						0 );
				}
			}

			if ( _mode == Mode::CPU )
			{
				_buffer->setVisibilities( _visibilities );
			}
			else
			{
				bufferVisibilities.setSub( _visibilities );
			}

			_visibilities.clear();
			_visibilities.shrink_to_fit();

			if ( _mode == Mode::GPU )
			{
				_buffer->doneContextCurrent();
			}
		}

		void SolventExcludedSurface::refreshSelection( const Model::Selection::MapChainIds * const p_selection )
		{
			using VTX::Renderer::GL::Buffer;

			if ( _mode == Mode::GPU )
			{
				_buffer->makeContextCurrent();
			}

			_selections.resize( _indiceCount, 0 );
			const Buffer & bufferSelections = _buffer->getBufferSelections();

			if ( p_selection != nullptr )
			{
				if ( p_selection->getFullySelectedChildCount() == _category->getMoleculePtr()->getRealChainCount() )
				{
					std::fill( _selections.begin(), _selections.end(), 1 );
				}
				else
				{
					for ( const Model::Selection::PairChainIds & pairChain : *p_selection )
					{
						for ( const Model::Selection::PairResidueIds & pairResidue : pairChain.second )
						{
							for ( const uint & atomIdx : pairResidue.second )
							{
								std::fill( _selections.begin() + _atomsToTriangles[ atomIdx ].first,
										   _selections.begin() + _atomsToTriangles[ atomIdx ].first
											   + _atomsToTriangles[ atomIdx ].count,
										   1 );
							}
						}
					}
				}
			}

			if ( _mode == Mode::CPU )
			{
				_buffer->setSelections( _selections );
			}
			else
			{
				bufferSelections.setSub( _selections );
			}

			_selections.clear();
			_selections.shrink_to_fit();

			if ( _mode == Mode::GPU )
			{
				_buffer->doneContextCurrent();
			}
		}

		void SolventExcludedSurface::_instantiate3DViews()
		{
			_addRenderable(
				MVC::MvcManager::get().instantiateView<View::D3::TriangleSES>( this, VTX::ID::View::D3_TRIANGLE_SES ) );
		}
	} // namespace Model
} // namespace VTX
