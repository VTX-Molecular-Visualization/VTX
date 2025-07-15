#ifndef BCS_CUDA_SETUP_CUH
#define BCS_CUDA_SETUP_CUH

#include <string_view>
#include <tuple>

#include <cuda.h>
#include <cuda_runtime.h>

enum cudaError;
using cudaError_t = cudaError;
typedef struct cudaGraphicsResource * cudaGraphicsResource_t;
enum cudaGraphicsRegisterFlags;

namespace bcs
{
    inline void cudaCheck( std::string_view msg = "" );
    inline void cudaCheck( std::string_view msg, cudaError_t err );
    inline void cudaCheck( cudaError_t err );

    struct KernelConfig
    {
        inline static KernelConfig From( uint32_t n, uint32_t blockDim );

        dim3 gridDim  = { 1u, 1u, 1u };
        dim3 blockDim = { 1u, 1u, 1u };

        inline operator std::tuple<dim3, dim3>() const;
    };
} // namespace bcs

#include "bcs/cuda/setup.inl"

#endif // BCS_CUDA_SETUP_CUH
