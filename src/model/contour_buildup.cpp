#include "contour_buildup.hpp"
#include "atom.hpp"
#include "molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/chrono.hpp"
#include "ui/main_window.hpp"
#include "view/d3/spherical_triangle.hpp"
#include "view/d3/torus.hpp"

namespace VTX
{
	namespace Model
	{
		const std::size_t ContourBuildup::_maxAtomNeighborNb  = 64;
		const std::size_t ContourBuildup::_maxProbePerCircle  = 8;
		const std::size_t ContourBuildup::_maxProbeNeighborNb = 32;
		const std::size_t ContourBuildup::_maxWorkGroupSize	  = 256;

		ContourBuildup::ContourBuildup( Molecule * const p_molecule ) :
			BaseModel3D( VTX::ID::Model::MODEL_CONTOUR_BUILDUP ), _molecule( p_molecule ),
			_workgroupLocalSize( { 512, 2, 1 } ), _probeRadius( 1.4f )
		{
		}

		void ContourBuildup::render( const Object3D::Camera & p_camera ) const
		{
			if ( _buffer == nullptr )
				return;

			_buffer->bind();
			_buffer->getSsboTorusPatches().bind( 0 );
			MVC::MvcManager::get().getView<View::D3::Torus>( this, VTX::ID::View::D3_TORUS_PATCH )->render( p_camera );
			_buffer->getSsboSphericalTrianglePatches().bind( 0 );
			_buffer->getSsboProbeIntersection().bind( 1 );
			MVC::MvcManager::get()
				.getView<View::D3::SphericalTriangle>( this, VTX::ID::View::D3_SPHERICAL_TRIANGLE_PATCH )
				->render( p_camera );
			_buffer->unbind();
		}

		void ContourBuildup::print() const {}

		void ContourBuildup::_init()
		{
			return;
			Tool::Chrono chrono;
			chrono.start();
			VTX_INFO( "Creating SES..." );
			_computeGPU();
			chrono.stop();
			VTX_INFO( "SES created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void ContourBuildup::_fillBuffer() {}

		void ContourBuildup::_computeAABB() const {}

		void ContourBuildup::_instantiate3DViews()
		{
			_addRenderable(
				MVC::MvcManager::get().instantiateView<View::D3::Torus>( this, VTX::ID::View::D3_TORUS_PATCH ) );
			_addRenderable( MVC::MvcManager::get().instantiateView<View::D3::SphericalTriangle>(
				this, VTX::ID::View::D3_SPHERICAL_TRIANGLE_PATCH ) );
		}

		void ContourBuildup::_computeGPU()
		{
			// Grid.
			const Math::AABB & molAABB		= _molecule->getAABB();
			const float		   maxVdwRadius = *std::max_element(
				   Atom::SYMBOL_VDW_RADIUS, Atom::SYMBOL_VDW_RADIUS + std::size( Atom::SYMBOL_VDW_RADIUS ) );

			_accelerationGrid.worldOrigin = molAABB.getMin() - maxVdwRadius - _probeRadius;
			const Vec3f worldSize
				= glm::abs( molAABB.getMax() + maxVdwRadius + _probeRadius - _accelerationGrid.worldOrigin );

			const std::size_t gridSize
				= Util::Math::nextPowerOfTwoValue( Util::Math::nextPowerOfTwoExponent( _molecule->getAtomCount() ) );
			_accelerationGrid.size	   = Vec3i( static_cast<int>( gridSize ) );
			_accelerationGrid.cellSize = Vec3f( worldSize / Vec3f( _accelerationGrid.size ) );

			_paddedAtomNb = Util::Math::nextPowerOfTwoValue( _molecule->getAtomCount() );

			_paddedNbOfSpTriangle = Util::Math::nextPowerOfTwoValue( _molecule->getAtomCount() * _maxAtomNeighborNb
																	 * _maxProbePerCircle );

			auto trianglePatchesInit = std::vector<uint8_t>( sizeof( Buffer::ContourBuildup::SphericalTrianglePatch )
															 * _paddedNbOfSpTriangle );

			const auto trianglesPatchesDefault = std::vector<Buffer::ContourBuildup::SphericalTrianglePatch>(
				_paddedNbOfSpTriangle,
				Buffer::ContourBuildup::SphericalTrianglePatch {
					Vec4f( 0.f ),
					Vec4f( 0.f ),
					Vec4f( 0.f ),
					Vec4f( 0.f, 0.f, 0.f, static_cast<float>( std::numeric_limits<uint>::max() ) ) } );
			std::memcpy( trianglePatchesInit.data(),
						 trianglesPatchesDefault.data(),
						 sizeof( Buffer::ContourBuildup::SphericalTrianglePatch ) * _paddedNbOfSpTriangle );

			// Create workers.
			_sortAtomWorker	 = new VTX::Worker::GpuComputer( IO::FilePath( "sort_atoms.comp" ) );
			_buildGridWorker = new VTX::Worker::GpuComputer( IO::FilePath( "build_atoms_grid.comp" ) );
			_countNeighborsWorker
				= new VTX::Worker::GpuComputer( IO::FilePath( "count_neighbors.comp" ), _workgroupLocalSize );
			_contourBuildupWorker
				= new VTX::Worker::GpuComputer( IO::FilePath( "contour_buildup.comp" ), _workgroupLocalSize );
			_buildPatchWorker = new VTX::Worker::GpuComputer( IO::FilePath( "build_patch.comp" ), _workgroupLocalSize );
			_buildProbesGridWorker = new VTX::Worker::GpuComputer( IO::FilePath( "build_probes_grid.comp" ) );
			_buildSingularityBufferWorker
				= new VTX::Worker::GpuComputer( IO::FilePath( "build_singularity_buffer.comp" ), _workgroupLocalSize );
			_sortProbesWorker = new VTX::Worker::GpuComputer( IO::FilePath( "sort_probes.comp" ) );

			// Compute atom informations.
			_atomsInformation						  = {};
			const auto &			   atoms		  = _molecule->getAtoms();
			const std::vector<Vec3f> & atomsPositions = _molecule->getAtomPositionFrame( _molecule->getFrame() );

			_atomsInformation.reserve( _molecule->getAtomCount() );
			for ( std::size_t i = 0; i < atoms.size(); i++ )
			{
				const auto & currentPosition = atomsPositions[ i ];
				const Vec3i	 gridPos		 = _accelerationGrid.gridPosition( currentPosition );
				const uint	 hash			 = _accelerationGrid.gridHash( gridPos );

				_atomsInformation.emplace_back(
					AtomInformation { currentPosition, atoms[ i ]->getVdwRadius(), 0, 0, hash } );
			}

			for ( std::size_t i = _atomsInformation.size(); i < _paddedAtomNb; i++ )
			{
				_atomsInformation.emplace_back(
					AtomInformation { Vec3f( 0.f ), 0, 0, 0, std::numeric_limits<VTX::uint>::max() } );
			}

			// Compute atom positions.
			std::vector<uint8_t> atomPositionData
				= std::vector<uint8_t>( _atomsInformation.size() * sizeof( AtomInformation ) );
			const uint atomNb = static_cast<uint>( _molecule->getAtomCount() );
			std::memcpy( atomPositionData.data(), &atomNb, sizeof( uint ) );
			std::memcpy( atomPositionData.data(),
						 _atomsInformation.data(),
						 _atomsInformation.size() * sizeof( AtomInformation ) );

			// SSBO atom position.
			_ssboAtomPosition = new VTX::Renderer::GL::BufferStorage(
				VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER, atomPositionData );

			// SSBO atom cell info.
			const auto atomGridIndices
				= std::vector<GridCellInfo>( static_cast<std::size_t>( _accelerationGrid.size.x )
												 * _accelerationGrid.size.y * _accelerationGrid.size.z,
											 GridCellInfo { -1, -1 } );
			_ssboAtomCellInfo = new VTX::Renderer::GL::BufferStorage(
				VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER, atomGridIndices );

			// SSBO size buffer.
			CBSizes startSize = { _molecule->getAtomCount() };
			_ssboSizeBuffer
				= new VTX::Renderer::GL::BufferStorage( VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER,
														sizeof( CBSizes ),
														startSize,
														VTX::Renderer::GL::BufferStorage::Flags::MAP_READ_BIT );

			// SSBO probes cell info.
			auto probesGridIndices
				= std::vector<GridCellInfo>( static_cast<std::size_t>( _accelerationGrid.size.x )
												 * _accelerationGrid.size.y * _accelerationGrid.size.z,
											 GridCellInfo { -1, -1 } );

			_ssboProbesCellInfo = new VTX::Renderer::GL::BufferStorage(
				VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER, probesGridIndices );

			// SSBO circles.
			const std::size_t circleMaximumNb = _molecule->getAtomCount() * _maxAtomNeighborNb;
			_ssboCircles
				= new VTX::Renderer::GL::BufferStorage( VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER,
														uint( sizeof( CircleIntersection ) * circleMaximumNb ) );

			// SSBO circle probes.
			_ssboCirclesProbes = new VTX::Renderer::GL::BufferStorage(
				VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER,
				uint( sizeof( ProbePosition ) * circleMaximumNb * _maxProbePerCircle ) );

			// Size reader.
			auto readSize = []( VTX::Renderer::GL::BufferStorage * const sizeBuffer, std::size_t idx ) -> std::size_t
			{
				const uint * const sizeData
					= sizeBuffer->map<uint>( uint( idx * sizeof( uint ) ),
											 uint( sizeof( uint ) ),
											 VTX::Renderer::GL::BufferStorage::Flags::MAP_READ_BIT );
				const std::size_t size = static_cast<std::size_t>( *sizeData );
				sizeBuffer->unmap();
				assert( size > 0 );
				return size;
			};

			// Sort atom positions.
			_ssboAtomPosition->bind( 0 );
			_bitonicSort( _sortAtomWorker, _paddedAtomNb );

			// Build grid.
			_ssboSizeBuffer->bind( 0 );
			_ssboAtomPosition->bind( 1 );
			_ssboAtomCellInfo->bind( 2 );
			_buildGridWorker->getProgram().use();
			_buildGridWorker->start( uint( _atomsInformation.size() ), GL_SHADER_STORAGE_BARRIER_BIT );

			// Count neighbors.
			_ssboSizeBuffer->bind( 0 );
			_ssboAtomPosition->bind( 1 );
			_ssboCircles->bind( 2 );
			_ssboAtomCellInfo->bind( 3 );
			_countNeighborsWorker->getProgram().use();
			_countNeighborsWorker->getProgram().setInt( "uMaxNeighborsNb", static_cast<int>( _maxAtomNeighborNb ) );
			_countNeighborsWorker->getProgram().setFloat( "uProbeRadius", _probeRadius );
			_countNeighborsWorker->getProgram().setVec3i( "uGridSize", _accelerationGrid.size );
			_countNeighborsWorker->getProgram().setVec3f( "uCellSize", _accelerationGrid.cellSize );
			_countNeighborsWorker->getProgram().setVec3f( "uWorldOrigin", _accelerationGrid.worldOrigin );
			_countNeighborsWorker->getProgram().setFloat( "uMaxVdwRadius", maxVdwRadius );
			_countNeighborsWorker->start( _molecule->getAtomCount(), GL_SHADER_STORAGE_BARRIER_BIT );

			_torusPatchNb = readSize( _ssboSizeBuffer, 1 );

			_ssboSizeBuffer->bind( 0 );
			_ssboAtomPosition->bind( 1 );
			_ssboCircles->bind( 2 );
			_ssboCirclesProbes->bind( 3 );
			_contourBuildupWorker->getProgram().use();
			_contourBuildupWorker->getProgram().setInt( "uMaxNeighborsNb", static_cast<GLint>( _maxAtomNeighborNb ) );
			_contourBuildupWorker->getProgram().setFloat( "uProbeRadius", _probeRadius );
			_contourBuildupWorker->getProgram().setInt( "uMaxProbePerCircle",
														static_cast<GLint>( _maxProbePerCircle ) );
			_contourBuildupWorker->start( uint( _torusPatchNb ), GL_SHADER_STORAGE_BARRIER_BIT );

			_buffer->getSsboTorusPatches().set(
				std::vector<uint>( sizeof( Buffer::ContourBuildup::TorusPatch ) * _torusPatchNb, 0u ) );

			_ssboSizeBuffer->bind( 0 );
			_ssboAtomPosition->bind( 1 );
			_ssboCircles->bind( 2 );
			_ssboCirclesProbes->bind( 3 );
			_buffer->getSsboTorusPatches().bind( 4 );
			_buffer->getSsboSphericalTrianglePatches().bind( 5 );
			_buildPatchWorker->getProgram().use();
			_buildPatchWorker->getProgram().setFloat( "uProbeRadius", _probeRadius );
			_buildPatchWorker->getProgram().setVec3i( "uGridSize", _accelerationGrid.size );
			_buildPatchWorker->getProgram().setVec3f( "uCellSize", _accelerationGrid.cellSize );
			_buildPatchWorker->getProgram().setVec3f( "uWorldOrigin", _accelerationGrid.worldOrigin );
			// Start a thread per circle
			_buildPatchWorker->start( uint( _torusPatchNb ), GL_SHADER_STORAGE_BARRIER_BIT );

			_trianglePatchNb = readSize( _ssboSizeBuffer, 3 );

			_buffer->getSsboSphericalTrianglePatches().set( trianglePatchesInit );
			_buffer->getSsboSphericalTrianglePatches().bind( 0 );
			_bitonicSort( _sortProbesWorker, _paddedNbOfSpTriangle );

			_ssboSizeBuffer->bind( 0 );
			_buffer->getSsboSphericalTrianglePatches().bind( 1 );
			_ssboProbesCellInfo->bind( 2 );
			_buildProbesGridWorker->getProgram().use();
			_buildProbesGridWorker->start( uint( _trianglePatchNb ), GL_SHADER_STORAGE_BARRIER_BIT );

			// Fill intersection texture
			_buffer->getSsboProbeIntersection().set(
				std::vector( sizeof( Vec4f ) * _trianglePatchNb * _maxProbeNeighborNb, 0u ) );

			_ssboSizeBuffer->bind( 0 );
			_buffer->getSsboSphericalTrianglePatches().bind( 1 );
			_buffer->getSsboProbeIntersection().bind( 2 );
			_ssboProbesCellInfo->bind( 3 );
			_buildSingularityBufferWorker->getProgram().use();
			_buildSingularityBufferWorker->getProgram().setFloat( "uProbeRadius", _probeRadius );
			_buildSingularityBufferWorker->getProgram().setInt( "uMaxProbeNeighborNb",
																static_cast<int>( _maxProbeNeighborNb ) );
			_buildSingularityBufferWorker->getProgram().setVec3i( "uGridSize", _accelerationGrid.size );
			_buildSingularityBufferWorker->getProgram().setVec3f( "uCellSize", _accelerationGrid.cellSize );
			_buildSingularityBufferWorker->getProgram().setVec3f( "uWorldOrigin", _accelerationGrid.worldOrigin );
			// Start a thread per circle
			_buildSingularityBufferWorker->start( uint( _trianglePatchNb ), GL_SHADER_STORAGE_BARRIER_BIT );
		}

		void ContourBuildup::_bitonicSort( VTX::Worker::GpuComputer * const bitonicSort, const std::size_t arraySize )
		{
			assert( arraySize % 2 == 0 );
			if ( arraySize < 2 )
				return;

			std::size_t workGroupWidth = 1;
			if ( arraySize < _maxWorkGroupSize * 2 )
			{
				workGroupWidth = arraySize / 2;
			}
			else
			{
				workGroupWidth = _maxWorkGroupSize;
			}
			const uint32_t workgroup_count = static_cast<uint32_t>( arraySize / ( workGroupWidth * 2 ) );
			uint32_t	   h			   = static_cast<uint32_t>( workGroupWidth ) * 2;

			assert( h <= arraySize );
			assert( h % 2 == 0 );

			// Local Bms
			bitonicSort->getProgram().use();
			bitonicSort->getProgram().setUInt( "n", static_cast<uint>( arraySize ) );
			bitonicSort->getProgram().setUInt( "h", h );
			bitonicSort->getProgram().setUInt( "algorithm", 0u );
			bitonicSort->start( Vec3i { workgroup_count, 1, 1 }, GL_SHADER_STORAGE_BARRIER_BIT );

			// we must now double h, as this happens before every flip
			h *= 2;

			for ( ; h <= arraySize; h *= 2 )
			{
				// big_flip
				bitonicSort->getProgram().use();
				bitonicSort->getProgram().setUInt( "h", h );
				bitonicSort->getProgram().setUInt( "algorithm", 2u );
				bitonicSort->start( Vec3i { workgroup_count, 1, 1 }, GL_SHADER_STORAGE_BARRIER_BIT );

				for ( uint32_t hh = h / 2; hh > 1; hh /= 2 )
				{
					if ( hh <= workGroupWidth * 2 )
					{
						// local_disperse
						// We can fit all elements for a disperse operation into continuous shader
						// workgroup local memory, which means we can complete the rest of the
						// cascade using a single shader invocation.
						bitonicSort->getProgram().use();
						bitonicSort->getProgram().setUInt( "h", hh );
						bitonicSort->getProgram().setUInt( "algorithm", 1u );
						bitonicSort->start( Vec3i { workgroup_count, 1, 1 }, { GL_SHADER_STORAGE_BARRIER_BIT } );

						break;
					}
					else
					{
						// big_disperse
						bitonicSort->getProgram().use();
						bitonicSort->getProgram().setUInt( "h", hh );
						bitonicSort->getProgram().setUInt( "algorithm", 3u );
						bitonicSort->start( Vec3i { workgroup_count, 1, 1 }, { GL_SHADER_STORAGE_BARRIER_BIT } );
					}
				}
			}
		}

	} // namespace Model
} // namespace VTX
