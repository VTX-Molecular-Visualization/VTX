#ifndef __VTX_MODEL_CONTOUR_BUILDUP__
#define __VTX_MODEL_CONTOUR_BUILDUP__

#include "base_model_3d.hpp"
#include "buffer/contour_buildup.hpp"
#include "math/grid.hpp"
#include "renderer/gl/buffer_storage.hpp"
#include "worker/gpu_computer.hpp"

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class ContourBuildup : public BaseModel3D<Buffer::ContourBuildup>
		{
			VTX_MODEL

		  public:
			inline Model::Molecule * const getMolecule() { return _molecule; }

			void render( const Object3D::Camera & p_camera ) const override;

			void print() const;

		  protected:
			void _init() override;
			void _fillBuffer() override;
			void _computeAABB() const override;
			void _instantiate3DViews() override;

		  private:
			struct CBSizes
			{
				uint atomNumber;
				uint circleNb;
				uint probesNb;
				uint spTriangleNb;
			};

			struct GridCellInfo
			{
				int start;
				int end;
				int pad[ 2 ];
			};

			struct AtomInformation
			{
				Vec3f position;
				float radius;
				uint  startCircleIdx;
				uint  neighborNb;
				uint  gridHash;
				int	  pad;
			};

			struct CircleIntersection
			{
				Vec3f center;
				float radius;
				uint  atomId;
				uint  neighboorAtomId;
				uint  startProbeIdx;
				uint  probeNb;
			};

			struct ProbePosition
			{
				Vec3f position;
				uint  atomIndex;
			};

			static const std::size_t _maxAtomNeighborNb;
			static const std::size_t _maxProbePerCircle;
			static const std::size_t _maxProbeNeighborNb;
			static const std::size_t _maxWorkGroupSize;

			Model::Molecule * const _molecule;
			const VTX::Vec3i		_workgroupLocalSize;
			const float				_probeRadius;
			Math::Grid				_accelerationGrid;

			std::size_t _paddedAtomNb		  = 0;
			std::size_t _paddedNbOfSpTriangle = 0;
			std::size_t _torusPatchNb		  = 0;
			std::size_t _trianglePatchNb	  = 0;

			std::vector<AtomInformation>	_atomsInformation;
			std::vector<CircleIntersection> _circles;
			std::vector<ProbePosition>		_probes;
			std::vector<Vec4f>				_probIntersectionBuffer;

			VTX::Renderer::GL::BufferStorage * _ssboSizeBuffer;
			VTX::Renderer::GL::BufferStorage * _ssboAtomPosition;
			VTX::Renderer::GL::BufferStorage * _ssboAtomCellInfo;
			VTX::Renderer::GL::BufferStorage * _ssboCircles;
			VTX::Renderer::GL::BufferStorage * _ssboCirclesProbes;
			VTX::Renderer::GL::BufferStorage * _ssboProbesCellInfo;

			VTX::Worker::GpuComputer * _sortAtomWorker;
			VTX::Worker::GpuComputer * _buildGridWorker;
			VTX::Worker::GpuComputer * _countNeighborsWorker;
			VTX::Worker::GpuComputer * _contourBuildupWorker;
			VTX::Worker::GpuComputer * _buildPatchWorker;
			VTX::Worker::GpuComputer * _sortProbesWorker;
			VTX::Worker::GpuComputer * _buildProbesGridWorker;
			VTX::Worker::GpuComputer * _buildSingularityBufferWorker;

			ContourBuildup( Molecule * const );
			~ContourBuildup() = default;

			void _bitonicSort( VTX::Worker::GpuComputer * const, const std::size_t );
			void _computeGPU();
		};
	} // namespace Model
} // namespace VTX

#endif
