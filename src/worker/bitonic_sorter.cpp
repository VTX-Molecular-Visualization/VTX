#include "bitonic_sorter.hpp"

namespace VTX::Worker
{
	void BitonicSorter::_run()
	{
		assert( _arraySize % 2 == 0 );
		if ( _arraySize < 2 )
		{
			return;
		}

		uint workGroupWidth = 1;
		if ( _arraySize < _maxWorkGroupSize * 2 )
		{
			workGroupWidth = _arraySize / 2;
		}
		else
		{
			workGroupWidth = _maxWorkGroupSize;
		}

		_size = Vec3i( workGroupWidth, 1, 1 );

		BaseGpuWorker::_run();

		const uint workgroupCount = _arraySize / ( workGroupWidth * 2 );
		uint	   h			  = workGroupWidth * 2;

		assert( h <= _arraySize );
		assert( h % 2 == 0 );

		// Local Bms.
		_value = h;
		BaseGpuWorker::_run();

		// We must now double h, as this happens before every flip.
		h *= 2;

		for ( ; h <= _arraySize; h *= 2 )
		{
			// Big_flip.
			_algorithm = 2;
			_value	   = h;
			BaseGpuWorker::_run();

			for ( uint32_t hh = h / 2; hh > 1; hh /= 2 )
			{
				_value = hh;
				if ( hh <= workGroupWidth * 2 )
				{
					// local_disperse
					// We can fit all elements for a disperse operation into continuous shader
					// workgroup local memory, which means we can complete the rest of the
					// cascade using a single shader invocation.
					_algorithm = 1;

					BaseGpuWorker::_run();
					break;
				}
				else
				{
					// big_disperse
					_algorithm = 3;
					BaseGpuWorker::_run();
				}
			}
		}
	}

	void BitonicSorter::_setUniforms()
	{
		_program->setInt( "n", _arraySize );
		_program->setInt( "algorithm", _algorithm );
		_program->setInt( "h", _value );
	}
} // namespace VTX::Worker
