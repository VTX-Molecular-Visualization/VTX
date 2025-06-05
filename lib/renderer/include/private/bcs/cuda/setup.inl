#include "bcs/cuda/setup.cuh"

namespace bcs
{
	inline void cudaCheck( std::string_view msg ) { cudaCheck( msg, cudaGetLastError() ); }

	inline void cudaCheck( std::string_view msg, cudaError_t err )
	{
		if ( err != cudaSuccess )
			std::cerr << msg << ": " << cudaGetErrorString( err ) << std::endl;
	}

	inline void cudaCheck( cudaError_t err )
	{
		if ( err != cudaSuccess )
			std::cerr << cudaGetErrorString( err ) << std::endl;
	}

	inline KernelConfig KernelConfig::From( uint32_t n, uint32_t blockDim )
	{
		blockDim = std::min( blockDim, n );
		return { n / blockDim + std::min( n / blockDim, 1u ), blockDim };
	}

	inline KernelConfig::operator std::tuple<dim3, dim3>() const { return { gridDim, blockDim }; }
} // namespace bcs
