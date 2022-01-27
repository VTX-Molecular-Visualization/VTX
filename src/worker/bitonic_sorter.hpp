#ifndef __VTX_WORKER_BITONIC_SORTER__
#define __VTX_WORKER_BITONIC_SORTER__

#include "base_gpu_worker.hpp"

namespace VTX::Worker
{
	class BitonicSorter : public Worker::BaseGpuWorker
	{
	  public:
		explicit BitonicSorter( const IO::FilePath & p_shader,
								const uint			 p_arraySize,
								const uint			 p_maxWorkGroupSize = 512 ) :
			BaseGpuWorker( p_shader, Vec3i(), GL_SHADER_STORAGE_BARRIER_BIT, false ),
			_arraySize( p_arraySize ), _maxWorkGroupSize( p_maxWorkGroupSize )
		{
		}
		~BitonicSorter() = default;

	  protected:
		void _run() override;

	  private:
		uint _arraySize;
		uint _maxWorkGroupSize;
		uint _value		= 0;
		uint _algorithm = 0;

		void _setUniforms() override;
	};
} // namespace VTX::Worker

#endif
