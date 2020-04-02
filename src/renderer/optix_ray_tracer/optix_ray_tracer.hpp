#ifndef __VTX_RENDERER_OPTIX_RAY_TRACER__
#define __VTX_RENDERER_OPTIX_RAY_TRACER__

#ifdef _MSC_VER
#pragma once
#endif

#include "../base_renderer.hpp"
#include "cuda_buffer.hpp"
#include "optix_parameters.hpp"
#include "optix_util.hpp"
#include "util/cuda.hpp"
#include <vector>

namespace VTX
{
	namespace Renderer
	{
		class OptixRayTracer : public BaseRenderer
		{
		  public:
			OptixRayTracer() = default;
			~OptixRayTracer();

			virtual void init( const uint, const uint ) override;
			virtual void renderFrame( const Object3D::Scene & ) override;
			virtual void setShading() override;
			virtual void resize( const uint, const uint ) override;

		  private:
			void _initOptix();
			void _createOptixContext();
			void _createOptixModule();
			// setup optix programs
			void _createOptixRayGeneratorPrograms();
			void _createOptixMissPrograms();
			void _createOptixHitGroupPrograms();
			// create GAS
			OptixTraversableHandle _buildGAS();
			// create pipeline from programs
			void _createOptixPipeline();
			// create shader binding table
			void _createOptixShaderBindingTable();

		  private:
			const Vec3f _backgroundColor = Vec3f( 0.f, 0.f, 0.f );

			// model
			std::vector<Optix::Sphere>	 _spheres;
			CudaBuffer					 _spheresDevBuffer;
			std::vector<Optix::Cylinder> _cylinders;
			CudaBuffer					 _cylindersDevBuffer;

			CudaBuffer _gasOutputBuffer;
			// CUDA data
			int			   _bestDeviceId = -1;
			cudaDeviceProp _deviceProperties;
			CUcontext	   _cudaContext;
			CUstream	   _cudaStream;

			// OptiX data
			OptixDeviceContext _optixContext;
			// optix module contains gpu programs used
			OptixModule					_optixModule;
			OptixModuleCompileOptions	_optixModuleCompileOptions;
			OptixPipeline				_optixPipeline;
			OptixPipelineCompileOptions _optixPipelineCompileOptions;
			OptixPipelineLinkOptions	_optixPipelineLinkOptions;

			// vectors for program groups
			std::vector<OptixProgramGroup> _rayGeneratorPrograms;
			CudaBuffer					   _rayGeneratorRecordsBuffer;
			std::vector<OptixProgramGroup> _missPrograms;
			CudaBuffer					   _missRecordsBuffer;
			std::vector<OptixProgramGroup> _hitGroupPrograms;
			CudaBuffer					   _hitGroupRecordsBuffer;

			// shader binding table (SBT)
			OptixShaderBindingTable _shaderBindingTable = {};

			// launch parameters host/device
			Optix::LaunchParameters _launchParameters;
			CudaBuffer				_launchParametersBuffer;

			CudaBuffer _colorBuffer;
		};
	} // namespace Renderer
} // namespace VTX

#endif
