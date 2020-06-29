#ifndef __VTX_RENDERER_OPTIX_RAY_TRACER__
#define __VTX_RENDERER_OPTIX_RAY_TRACER__

#ifdef _MSC_VER
#pragma once
#endif

#ifdef OPTIX_DEFINED

#include "../base_renderer.hpp"
#include "cuda/buffer.hpp"
#include "optix_parameters.hpp"
#include "optix_util.hpp"
#include "rt_scene_optix.hpp"
#include <vector>

namespace VTX::Renderer::Optix
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

		const inline std::vector<uchar4> & getPixels() const { return _pixels; }

	  private:
		void _initOptix();
		void _createOptixContext();
		void _createOptixModule();
		// setup optix programs
		void _createOptixRayGeneratorPrograms();
		void _createOptixMissPrograms();
		void _createOptixHitGroupSpherePrograms();
		void _createOptixHitGroupCylinderPrograms();
		// create GAS
		OptixTraversableHandle _buildGAS();
		// create pipeline from programs
		void _createOptixPipeline();
		// create shader binding table
		void _createOptixShaderBindingTable();

	  private:
		std::vector<uchar4> _pixels			 = std::vector<uchar4>();
		const Vec3f			_backgroundColor = Vec3f( 0.f, 0.f, 0.f );

		// model
		Optix::Scene _scene;

		CUDA::Buffer _gasOutputBuffer;
		OptixAabb	 _gasAABB;
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

		// Program groups
		OptixProgramGroup			   _rayGeneratorProgram;
		CUDA::Buffer				   _rayGeneratorRecordBuffer;
		OptixProgramGroup			   _missProgram;
		CUDA::Buffer				   _missRecordBuffer;
		OptixProgramGroup			   _hitGroupSphereProgram;
		OptixProgramGroup			   _hitGroupCylinderProgram;
		CUDA::Buffer				   _hitGroupRecordsBuffer;
		std::vector<OptixProgramGroup> _programGroups;

		// shader binding table (SBT)
		OptixShaderBindingTable _shaderBindingTable = {};
		uint					_nbSbtRecords		= 0;
		std::vector<uint>		_sbtIndex;
		CUDA::Buffer			_sbtIndexBuffer;

		// launch parameters host/device
		Optix::LaunchParameters _launchParameters;
		CUDA::Buffer			_launchParametersBuffer;

		CUDA::Buffer _pixelsBuffer;
	};
} // namespace VTX::Renderer::Optix

#endif
#endif
