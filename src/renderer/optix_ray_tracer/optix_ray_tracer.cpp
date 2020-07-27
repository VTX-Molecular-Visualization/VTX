#ifdef OPTIX_DEFINED

#include "optix_ray_tracer.hpp"
#include "color/rgb.hpp"
#include "tool/chrono.hpp"
#include "util/sampler.hpp"
#include "vtx_app.hpp"
#include <atomic>
#include <fstream>
#include <optix_function_table_definition.h>
#include <stb/stb_image_write.h>
#include <thread>
#include <unordered_set>

//#define SPHERES

namespace VTX::Renderer::Optix
{
	static void context_log_cb( unsigned int level, const char * tag, const char * message, void * )
	{
		fprintf( stderr, "[%2d][%12s]: %s\n", (int)level, tag, message );
	}

	OptixRayTracer::~OptixRayTracer()
	{
		_rayGeneratorRecordBuffer.free();
		_missRecordBuffer.free();
		_hitGroupRecordsBuffer.free();
		_gasOutputBuffer.free();
		_launchParametersBuffer.free();

		optixPipelineDestroy( _optixPipeline );

		optixProgramGroupDestroy( _rayGeneratorProgram );
		optixProgramGroupDestroy( _missProgram );
		optixProgramGroupDestroy( _hitGroupSphereProgram );
		optixProgramGroupDestroy( _hitGroupCylinderProgram );

		optixModuleDestroy( _optixModule );
		optixDeviceContextDestroy( _optixContext );
		cuStreamDestroy( _cudaStream );
		cuCtxDestroy( _cudaContext );
	}

	void OptixRayTracer::init( const uint p_width, const uint p_height )
	{
		// VTX_INFO( "Initializing OptiX ray tracer (only first molecule)..." );

		resize( p_width, p_height );

		_gasOutputBuffer.free();

		_rayGeneratorRecordBuffer.free();
		_missRecordBuffer.free();
		_hitGroupRecordsBuffer.free();

		// TODO: check scene size.
		// TODO: re-init when scene is updated.
		const Model::Molecule * mol = VTXApp::get().getScene().getMolecules().begin()->first;
		_scene.add( mol, VTX_SETTING().representation );

		_launchParameters._colors = (float3 *)( _scene.getColorsDevPtr() );

		try
		{
			VTX_INFO( "_initOptix..." );
			_initOptix();

			VTX_INFO( "_createOptixContext..." );
			_createOptixContext();

			VTX_INFO( "_createOptixModule..." );
			_createOptixModule();

			VTX_INFO( "_createOptixRayGeneratorPrograms..." );
			_createOptixRayGeneratorPrograms();

			VTX_INFO( "_createOptixMissPrograms..." );
			_createOptixMissPrograms();

			VTX_INFO( "_createOptixHitGroupPrograms..." );
			_createOptixHitGroupSpherePrograms();
			_createOptixHitGroupCylinderPrograms();

			VTX_INFO( "_buildGAS..." );
			_launchParameters._traversable = _buildGAS();

			VTX_INFO( "_createOptixPipeline..." );
			_createOptixPipeline();

			VTX_INFO( "_createOptixShaderBindingTable..." );
			_createOptixShaderBindingTable();

			_launchParametersBuffer.malloc( sizeof( _launchParameters ) );
		}
		catch ( const std::exception & e )
		{
			VTX_ERROR( e.what() );
			exit( EXIT_FAILURE );
			// TODO: handle it ! :-)
		}

		glGenTextures( 1, &_texture );
		glBindTexture( GL_TEXTURE_2D, _texture );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );

		VTX_INFO( "Optix ray tracer initialized" );
	}

	void OptixRayTracer::renderFrame( const Object3D::Scene & p_scene )
	{
		if ( _launchParameters._frame._width == 0 || _launchParameters._frame._height == 0 )
			return;

		// VTX_INFO( "Render Frame" );

		_updateCam();
		//_createOptixShaderBindingTable();

		// cudaEvent_t start, stop;
		// float		elapsedTime;
		// CUDA_HANDLE_ERROR( cudaEventCreate( &start ) );
		// CUDA_HANDLE_ERROR( cudaEventCreate( &stop ) );

		// CUDA_HANDLE_ERROR( cudaEventRecord( start, 0 ) );

		_launchParametersBuffer.memcpyHostToDevice( &_launchParameters, 1 );
		_launchParameters._frame._id++;

		OPTIX_HANDLE_ERROR( optixLaunch( _optixPipeline,
										 _cudaStream,
										 _launchParametersBuffer.getDevicePtr(),
										 _launchParametersBuffer._sizeInBytes,
										 &_shaderBindingTable,
										 _launchParameters._frame._width,
										 _launchParameters._frame._height,
										 1 ) );

		// CUDA_HANDLE_ERROR( cudaEventRecord( stop, 0 ) );
		// CUDA_HANDLE_ERROR( cudaEventSynchronize( stop ) );
		// CUDA_HANDLE_ERROR( cudaEventElapsedTime( &elapsedTime, start, stop ) );

		// VTX_INFO( "Rendering time: " + std::to_string( elapsedTime ) );
		// VTX_INFO( "Save image as: test Optix.png" );

		// sync - make sure the frame is rendered before we download and
		// display (obviously, for a high-performance application you
		// want to use streams and double-buffering, but for this simple
		// example, this will have to do)
		CUDA_SYNCHRONIZE_HANDLE_ERROR();

		_pixelsBuffer.memcpyDeviceToHost( (uchar4 *)( _pixels.data() ),
										  _launchParameters._frame._width * _launchParameters._frame._height );

		glBindTexture( GL_TEXTURE_2D, _texture );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixels.data() );

		// VTX_INFO( "Saved" );
	}

	void OptixRayTracer::setShading() {}

	void OptixRayTracer::resize( const uint p_width, const uint p_height )
	{
		BaseRenderer::resize( p_width, p_height );
		_pixels.resize( _width * _height );
		_pixelsBuffer.realloc( _width * _height * sizeof( uint ) );
		_launchParameters._frame._width	 = uint16_t( _width );
		_launchParameters._frame._height = uint16_t( _height );
		_launchParameters._frame._pixels = (uchar4 *)( _pixelsBuffer.getDevicePtr() );
	}

	void OptixRayTracer::_initOptix()
	{
		// check for CUDA capable devices and choose "best" one
		cudaFree( 0 );

		_bestDeviceId = CUDA::chooseBestDevice();

		VTX_INFO( "CUDA device capable found" );
		cudaGetDeviceProperties( &_deviceProperties, _bestDeviceId );

		std::cout << _deviceProperties.name << std::endl;
		std::cout << "Compute capability: " << _deviceProperties.major << "." << _deviceProperties.minor << std::endl;

		if ( false /*TODO: check what compute capability we need*/ )
		{
			throw std::runtime_error( "no CUDA capable device is detected with compute capability >= X.Y" );
		}

		OPTIX_HANDLE_ERROR( optixInit() );
	}

	void OptixRayTracer::_createOptixContext()
	{
		CUDA_HANDLE_ERROR( cudaSetDevice( _bestDeviceId ) );
		CUDA_HANDLE_ERROR( cudaStreamCreate( &_cudaStream ) );

		CUresult res = cuCtxGetCurrent( &_cudaContext );

		if ( res != CUDA_SUCCESS )
		{
			throw std::runtime_error( "Error getting CUDA context:" + res );
		}

		OPTIX_HANDLE_ERROR( optixDeviceContextCreate( _cudaContext, 0, &_optixContext ) );
		OPTIX_HANDLE_ERROR( optixDeviceContextSetLogCallback( _optixContext, context_log_cb, nullptr, 4 ) );
	}

	void OptixRayTracer::_createOptixModule()
	{
		// TODO: why 50 ? fix that
		_optixModuleCompileOptions					= {};
		_optixModuleCompileOptions.maxRegisterCount = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT;
#ifdef _DEBUG
		_optixModuleCompileOptions.optLevel	  = OPTIX_COMPILE_OPTIMIZATION_LEVEL_0;
		_optixModuleCompileOptions.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_FULL;
#else
		_optixModuleCompileOptions.optLevel			= OPTIX_COMPILE_OPTIMIZATION_LEVEL_3;
		_optixModuleCompileOptions.debugLevel		= OPTIX_COMPILE_DEBUG_LEVEL_NONE;
#endif

		_optixPipelineCompileOptions = {};
		// we use only one GAS (geometry acceleration structure)
		_optixPipelineCompileOptions.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_GAS;
		// 3 payload registers (R G B output)
		_optixPipelineCompileOptions.numPayloadValues = 3;
		_optixPipelineCompileOptions.usesMotionBlur	  = false;
		// How much storage, in 32b words, to make available for the attributes.
		// Here 3 for normal + 1 for color id
		_optixPipelineCompileOptions.numAttributeValues = 4;
#ifdef _DEBUG
		_optixPipelineCompileOptions.exceptionFlags = OPTIX_EXCEPTION_FLAG_DEBUG;
#else
		_optixPipelineCompileOptions.exceptionFlags = OPTIX_EXCEPTION_FLAG_NONE;
#endif
		// name of the struct used as parameter variable on device code
		_optixPipelineCompileOptions.pipelineLaunchParamsVariableName = "params";

		_optixPipelineLinkOptions						 = {};
		_optixPipelineLinkOptions.overrideUsesMotionBlur = false;
		_optixPipelineLinkOptions.maxTraceDepth			 = 1;
		_optixPipelineLinkOptions.debugLevel			 = OPTIX_COMPILE_DEBUG_LEVEL_NONE;

#define NVRTC_DOES_NOT_WORK
#ifndef NVRTC_DOES_NOT_WORK
		// compitation to PTX
		// TODO
		// First try to compile with NVRTC, does not work

		// read .cu
		const std::string file = "../src/renderer/optix_ray_tracer/test_optix.cu";
		std::ifstream	  ifs( file, std::ios::binary | std::ios::ate );
		if ( !ifs.is_open() )
		{
			throw std::runtime_error( "Cannot open file " + file );
		}

		uint   size = uint( ifs.tellg() );
		char * cu	= new char[ size + 1 ];

		ifs.seekg( 0, std::ios::beg );
		ifs.read( cu, size );
		ifs.close();

		std::cout << "========> " << size << std::endl;

		nvrtcProgram program;
		NVRTC_HANDLE_ERROR( nvrtcCreateProgram( &program, cu, "test_optix.cu", 0, nullptr, nullptr ) );

		const std::string vtxIncludesDir = std::string( "-I ../.." );
		const std::string cudaIncludesDir
			= std::string( "-I C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.2/include/" );
		const std::string optixIncludesDir
			= std::string( "-I C:/ProgramData/NVIDIA Corporation/OptiX SDK 7.0.0/include/" );
		/*	const std::string ucrtIncludesDir
				= std::string( "-I C:/Program Files (x86)/Windows Kits/10/Include/10.0.18362.0/ucrt" );
			const std::string vcIncludesDir = std::string(
				"-I C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.25.28610/include"
		   );*/

		const std::vector<const char *> compileOptions = {
			"--gpu-architecture=compute_30", // TODO: 3.0 ? 7.5 for rtx but we will need without rtx too I guess
			"--use_fast_math",
			"--std=c++11",
			"-DWin_64",
			vtxIncludesDir.c_str(),
			cudaIncludesDir.c_str(),
			optixIncludesDir.c_str() /*,
			 ucrtIncludesDir.c_str(),
			 vcIncludesDir.c_str()*/
		};

		nvrtcResult compileRes = nvrtcCompileProgram( program, int( compileOptions.size() ), compileOptions.data() );

		// get compilation log
		size_t logSize;
		NVRTC_HANDLE_ERROR( nvrtcGetProgramLogSize( program, &logSize ) );
		std::string log;
		log.resize( logSize );
		NVRTC_HANDLE_ERROR( nvrtcGetProgramLog( program, log.data() ) );

		std::cout << "================== LOG NVRTC" << std::endl;
		std::cout << log << std::endl;
		std::cout << "============================" << std::endl;

		if ( compileRes != NVRTC_SUCCESS )
		{
			nvrtcDestroyProgram( &program );
			throw std::runtime_error( log );
		}

		std::string ptx;
		NVRTC_HANDLE_ERROR( nvrtcGetPTX( program, ptx.data() ) );
		uint ptxSize = uint( ptx.size() );

		char   logOptix[ 2048 ];
		size_t sizeof_log = sizeof( logOptix );
		OPTIX_HANDLE_ERROR( optixModuleCreateFromPTX( _optixContext,
													  &_optixModuleCompileOptions,
													  &_optixPipelineCompileOptions,
													  ptx.c_str(),
													  ptxSize,
													  logOptix,
													  &sizeof_log,
													  &_optixModule ) );

		nvrtcDestroyProgram( &program );

		delete[] cu;
#else
		// read .ptx
		const std::string file = "../src/renderer/optix_ray_tracer/test_optix.ptx";
		std::ifstream	  ifs( file, std::ios::binary | std::ios::ate );
		if ( !ifs.is_open() )
		{
			throw std::runtime_error( "Cannot open file " + file );
		}

		uint   ptxSize = uint( ifs.tellg() );
		char * ptx	   = new char[ ptxSize + 1 ];

		ifs.seekg( 0, std::ios::beg );
		ifs.read( ptx, ptxSize );
		ifs.close();

		char   log[ 2048 ];
		size_t sizeof_log = sizeof( log );
		OPTIX_HANDLE_ERROR( optixModuleCreateFromPTX( _optixContext,
													  &_optixModuleCompileOptions,
													  &_optixPipelineCompileOptions,
													  ptx,
													  ptxSize,
													  log,
													  &sizeof_log,
													  &_optixModule ) );

		delete[] ptx;
#endif

		if ( sizeof_log > 1 )
		{
			VTX_INFO( "optixModuleCreateFromPTX (_createOptixModule) log :" );
			VTX_INFO( log );
		}
	}

	void OptixRayTracer::_createOptixRayGeneratorPrograms()
	{
		OptixProgramGroupOptions programOptions		= {};
		OptixProgramGroupDesc	 programDescription = {};
		programDescription.kind						= OPTIX_PROGRAM_GROUP_KIND_RAYGEN;
		programDescription.raygen.module			= _optixModule;
		programDescription.raygen.entryFunctionName = "__raygen__";

		char   log[ 2048 ];
		size_t sizeof_log = sizeof( log );
		OPTIX_HANDLE_ERROR( optixProgramGroupCreate(
			_optixContext, &programDescription, 1, &programOptions, log, &sizeof_log, &_rayGeneratorProgram ) );

		if ( sizeof_log > 1 )
		{
			VTX_INFO( "optixProgramGroupCreate (_createOptixRayGeneratorPrograms) log :" );
			VTX_INFO( log );
		}
	}

	void OptixRayTracer::_createOptixMissPrograms()
	{
		OptixProgramGroupOptions programOptions		= {};
		OptixProgramGroupDesc	 programDescription = {};
		programDescription.kind						= OPTIX_PROGRAM_GROUP_KIND_MISS;
		programDescription.miss.module				= _optixModule;
		programDescription.miss.entryFunctionName	= "__miss__";

		char   log[ 2048 ];
		size_t sizeof_log = sizeof( log );
		OPTIX_HANDLE_ERROR( optixProgramGroupCreate(
			_optixContext, &programDescription, 1, &programOptions, log, &sizeof_log, &_missProgram ) );

		if ( sizeof_log > 1 )
		{
			VTX_INFO( "optixProgramGroupCreate (_createOptixMissPrograms) log :" );
			VTX_INFO( log );
		}
	}

	void OptixRayTracer::_createOptixHitGroupSpherePrograms()
	{
		OptixProgramGroupOptions programOptions		= {};
		OptixProgramGroupDesc	 programDescription = {};
		programDescription.kind						= OPTIX_PROGRAM_GROUP_KIND_HITGROUP;
		// closest hit
		programDescription.hitgroup.moduleCH			= _optixModule;
		programDescription.hitgroup.entryFunctionNameCH = "__closesthit__sphere";
		// any hit
		programDescription.hitgroup.moduleAH			= nullptr; //_optixModule;
		programDescription.hitgroup.entryFunctionNameAH = nullptr; //"__anyhit__";
		// intersection
		programDescription.hitgroup.moduleIS			= _optixModule;
		programDescription.hitgroup.entryFunctionNameIS = "__intersection__sphere";

		char   log[ 2048 ];
		size_t sizeof_log = sizeof( log );
		OPTIX_HANDLE_ERROR( optixProgramGroupCreate(
			_optixContext, &programDescription, 1, &programOptions, log, &sizeof_log, &_hitGroupSphereProgram ) );

		if ( sizeof_log > 1 )
		{
			VTX_INFO( "optixProgramGroupCreate (_createOptixHitGroupPrograms) log :" );
			VTX_INFO( log );
		}
	}

	void OptixRayTracer::_createOptixHitGroupCylinderPrograms()
	{
		OptixProgramGroupOptions programOptions		= {};
		OptixProgramGroupDesc	 programDescription = {};
		programDescription.kind						= OPTIX_PROGRAM_GROUP_KIND_HITGROUP;
		// closest hit
		programDescription.hitgroup.moduleCH			= _optixModule;
		programDescription.hitgroup.entryFunctionNameCH = "__closesthit__cylinder";
		// any hit
		programDescription.hitgroup.moduleAH			= nullptr; //_optixModule;
		programDescription.hitgroup.entryFunctionNameAH = nullptr; //"__anyhit__";
		// intersection
		programDescription.hitgroup.moduleIS			= _optixModule;
		programDescription.hitgroup.entryFunctionNameIS = "__intersection__cylinder";

		char   log[ 2048 ];
		size_t sizeof_log = sizeof( log );
		OPTIX_HANDLE_ERROR( optixProgramGroupCreate(
			_optixContext, &programDescription, 1, &programOptions, log, &sizeof_log, &_hitGroupCylinderProgram ) );

		if ( sizeof_log > 1 )
		{
			VTX_INFO( "optixProgramGroupCreate (_createOptixHitGroupPrograms) log :" );
			VTX_INFO( log );
		}
	}

	OptixTraversableHandle OptixRayTracer::_buildGAS()
	{
#define TEST
#ifdef TEST
		// TODO: compact gas ?

		std::vector<OptixBuildInput> buildInputs;

		OptixAccelBuildOptions accelOptions = {};
		accelOptions.buildFlags				= OPTIX_BUILD_FLAG_NONE;
		accelOptions.operation				= OPTIX_BUILD_OPERATION_BUILD;
		accelOptions.motionOptions.numKeys	= 0;

		// Set up spheres input if any.
		const std::vector<Sphere> & spheres = _scene.getSpheres();
		if ( !spheres.empty() )
		{
			std::vector<OptixAabb> spheresAabb;
			spheresAabb.reserve( spheres.size() );

			for ( uint i = 0; i < uint( spheres.size() ); ++i )
			{
				spheresAabb.emplace_back( spheres[ i ].aabb() );
			}

			CUDA::Buffer d_spheresAabb;
			d_spheresAabb.malloc( spheresAabb.size() * sizeof( OptixAabb ) );
			d_spheresAabb.memcpyHostToDevice( spheresAabb.data(), spheresAabb.size() );
			CUdeviceptr d_spheresAabbPtr = d_spheresAabb.getDevicePtr();

			buildInputs.emplace_back( OptixBuildInput() );
			OptixBuildInputCustomPrimitiveArray & spheresInput		= buildInputs.back().aabbArray;
			uint32_t							  spheresInputFlags = { OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT };

			buildInputs.back().type					 = OPTIX_BUILD_INPUT_TYPE_CUSTOM_PRIMITIVES;
			spheresInput.flags						 = &spheresInputFlags;
			spheresInput.aabbBuffers				 = &d_spheresAabbPtr;
			spheresInput.numPrimitives				 = uint( spheres.size() );
			spheresInput.numSbtRecords				 = 1;
			spheresInput.sbtIndexOffsetSizeInBytes	 = sizeof( int );
			spheresInput.sbtIndexOffsetStrideInBytes = sizeof( int );
			spheresInput.sbtIndexOffsetBuffer		 = 0;
		}

		// Set up cylinders input if any.
		const std::vector<Cylinder> & cylinders = _scene.getCylinders();
		if ( !cylinders.empty() )
		{
			std::vector<OptixAabb> cylindersAabb;
			cylindersAabb.reserve( cylinders.size() );

			for ( uint i = 0; i < uint( cylinders.size() ); ++i )
			{
				cylindersAabb.emplace_back( cylinders[ i ].aabb() );
			}

			CUDA::Buffer d_cylindersAabb;
			d_cylindersAabb.malloc( cylindersAabb.size() * sizeof( OptixAabb ) );
			d_cylindersAabb.memcpyHostToDevice( cylindersAabb.data(), cylindersAabb.size() );
			CUdeviceptr d_cylindersAabbPtr = d_cylindersAabb.getDevicePtr();

			buildInputs.emplace_back( OptixBuildInput() );
			OptixBuildInputCustomPrimitiveArray & cylindersInput	  = buildInputs.back().aabbArray;
			uint32_t							  cylindersInputFlags = { OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT };

			buildInputs.back().type					   = OPTIX_BUILD_INPUT_TYPE_CUSTOM_PRIMITIVES;
			cylindersInput.flags					   = &cylindersInputFlags;
			cylindersInput.aabbBuffers				   = &d_cylindersAabbPtr;
			cylindersInput.numPrimitives			   = uint( cylinders.size() );
			cylindersInput.numSbtRecords			   = 1;
			cylindersInput.sbtIndexOffsetSizeInBytes   = sizeof( int );
			cylindersInput.sbtIndexOffsetStrideInBytes = sizeof( int );
			cylindersInput.sbtIndexOffsetBuffer		   = 0;
		}

		// Compute GAS memory.
		OptixAccelBufferSizes bufferSizes;
		OPTIX_HANDLE_ERROR( optixAccelComputeMemoryUsage(
			_optixContext, &accelOptions, buildInputs.data(), uint( buildInputs.size() ), &bufferSizes ) );
		CUDA::Buffer d_gasOutput;
		d_gasOutput.malloc( bufferSizes.outputSizeInBytes );
		CUDA::Buffer d_temp;
		d_temp.malloc( bufferSizes.tempSizeInBytes );

		// Get GAS AABB.
		CUDA::Buffer d_gasAABB;
		d_gasAABB.malloc( sizeof( OptixAabb ) );

		// Build GAS.
		OptixAccelEmitDesc emitProperty;
		emitProperty.type	= OPTIX_PROPERTY_TYPE_AABBS;
		emitProperty.result = d_gasAABB.getDevicePtr();

		OptixTraversableHandle gasHandle;
		OPTIX_HANDLE_ERROR( optixAccelBuild( _optixContext,
											 0,
											 &accelOptions,
											 buildInputs.data(),
											 uint( buildInputs.size() ),
											 d_temp.getDevicePtr(),
											 bufferSizes.tempSizeInBytes,
											 d_gasOutput.getDevicePtr(),
											 bufferSizes.outputSizeInBytes,
											 &gasHandle,
											 &emitProperty,
											 1 ) );

		d_gasAABB.memcpyDeviceToHost( &_gasAABB, 1 );

		return gasHandle;
#else
		// create AABB buffer from sphere
		std::vector<OptixAabb> aabbs;

		const std::vector<Sphere> & spheres = _scene.getSpheres();
		if ( !spheres.empty() )
		{
			for ( uint i = 0; i < uint( spheres.size() ); ++i )
			{
				aabbs.emplace_back( spheres[ i ].aabb() );
			}
			_nbSbtRecords++;
			_sbtIndex.emplace_back( uint( _sbtIndex.size() ) );
		}

		const std::vector<Cylinder> & cylinders = _scene.getCylinders();
		if ( !cylinders.empty() )
		{
			for ( uint i = 0; i < uint( cylinders.size() ); ++i )
			{
				aabbs.emplace_back( cylinders[ i ].aabb() );
			}
			_nbSbtRecords++;
			_sbtIndex.emplace_back( uint( _sbtIndex.size() ) );
		}

		CUDA::Buffer aabbsBuffer;
		aabbsBuffer.malloc( aabbs.size() * sizeof( OptixAabb ) );
		aabbsBuffer.memcpyHostToDevice( aabbs.data(), aabbs.size() );

		_sbtIndexBuffer.malloc( _sbtIndex.size() * sizeof( uint ) );
		_sbtIndexBuffer.memcpyHostToDevice( _sbtIndex.data(), _sbtIndex.size() );

		OptixBuildInput aabbInput		= {};
		uint32_t	aabbInputFlags[ 2 ] = { OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT, OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT };
		CUdeviceptr devPtr				= aabbsBuffer.getDevicePtr();

		aabbInput.type								  = OPTIX_BUILD_INPUT_TYPE_CUSTOM_PRIMITIVES;
		aabbInput.aabbArray.aabbBuffers				  = &devPtr;
		aabbInput.aabbArray.flags					  = aabbInputFlags;
		aabbInput.aabbArray.numSbtRecords			  = _nbSbtRecords;
		aabbInput.aabbArray.numPrimitives			  = uint( aabbs.size() );
		aabbInput.aabbArray.sbtIndexOffsetBuffer	  = _sbtIndexBuffer.getDevicePtr();
		aabbInput.aabbArray.sbtIndexOffsetSizeInBytes = sizeof( uint32_t );
		aabbInput.aabbArray.primitiveIndexOffset	  = 0;

		// build gas
		OptixTraversableHandle gasHandle;
		OptixAccelBuildOptions accelOptions = {};
		accelOptions.buildFlags				= OPTIX_BUILD_FLAG_ALLOW_COMPACTION;
		accelOptions.operation				= OPTIX_BUILD_OPERATION_BUILD;
		accelOptions.motionOptions.numKeys	= 0;

		OptixAccelBufferSizes gasBufferSizes;
		OPTIX_HANDLE_ERROR(
			optixAccelComputeMemoryUsage( _optixContext, &accelOptions, &aabbInput, 1, &gasBufferSizes ) );
		CUDA::Buffer tempBufferGAS;
		tempBufferGAS.malloc( gasBufferSizes.tempSizeInBytes );

		// non-compact output
		CUDA::Buffer tempOutputGasCompacted;
		size_t		 compactSizeOffset = ( ( gasBufferSizes.outputSizeInBytes + 8ull - 1ull ) / 8ull ) * 8ull;
		tempOutputGasCompacted.malloc( compactSizeOffset + 8 );

		OptixAccelEmitDesc emitProperties = {};
		emitProperties.type				  = OPTIX_PROPERTY_TYPE_COMPACTED_SIZE;
		emitProperties.result
			= ( CUdeviceptr )( (char *)( tempOutputGasCompacted.getDevicePtr() ) + compactSizeOffset );

		OPTIX_HANDLE_ERROR( optixAccelBuild( _optixContext,
											 0,
											 &accelOptions,
											 &aabbInput,
											 1,
											 tempBufferGAS.getDevicePtr(),
											 gasBufferSizes.tempSizeInBytes,
											 tempOutputGasCompacted.getDevicePtr(),
											 gasBufferSizes.outputSizeInBytes,
											 &gasHandle,
											 &emitProperties,
											 1 ) );

		tempBufferGAS.free();
		aabbsBuffer.free();

		size_t compactedGasSize;
		CUDA_HANDLE_ERROR(
			cudaMemcpy( &compactedGasSize, (void *)emitProperties.result, sizeof( size_t ), cudaMemcpyDeviceToHost ) );

		if ( compactedGasSize < gasBufferSizes.outputSizeInBytes )
		{
			_gasOutputBuffer.malloc( compactedGasSize );
			OPTIX_HANDLE_ERROR( optixAccelCompact(
				_optixContext, 0, gasHandle, _gasOutputBuffer.getDevicePtr(), compactedGasSize, &gasHandle ) );
			tempOutputGasCompacted.free();
		}
		else
		{
			_gasOutputBuffer = tempOutputGasCompacted;
		}

		return gasHandle;
#endif
	}

	void OptixRayTracer::_createOptixPipeline()
	{
		_programGroups = { _rayGeneratorProgram, _missProgram };

		if ( !_scene.getSpheres().empty() )
			_programGroups.emplace_back( _hitGroupSphereProgram );
		if ( !_scene.getCylinders().empty() )
			_programGroups.emplace_back( _hitGroupCylinderProgram );

		char   log[ 2048 ];
		size_t sizeof_log = sizeof( log );
		OPTIX_HANDLE_ERROR( optixPipelineCreate( _optixContext,
												 &_optixPipelineCompileOptions,
												 &_optixPipelineLinkOptions,
												 _programGroups.data(),
												 int( _programGroups.size() ),
												 log,
												 &sizeof_log,
												 &_optixPipeline ) );

		if ( sizeof_log > 1 )
		{
			VTX_INFO( "optixPipelineCreate (_createOptixHitGroupPrograms) log :" );
			VTX_INFO( log );
		}

		OPTIX_HANDLE_ERROR( optixPipelineSetStackSize( /* [in] The pipeline to configure the stack size for */
													   _optixPipeline,
													   /* [in] The direct stack size requirement for direct
														  callables invoked from IS or AH. */
													   2 * 1024,
													   /* [in] The direct stack size requirement for direct
														  callables invoked from RG, MS, or CH.  */
													   2 * 1024,
													   /* [in] The continuation stack requirement. */
													   2 * 1024,
													   /* [in] The maximum depth of a traversable graph
														  passed to trace. */
													   1 ) );
	}

	void OptixRayTracer::_createOptixShaderBindingTable()
	{
		_shaderBindingTable = {};
		// create ray generator records buffer on device
		{
			RayGeneratorRecord r;

			const Object3D::Camera & cam		= VTXApp::get().getScene().getCamera();
			Vec3f					 camPos		= cam.getPosition();
			Vec3f					 camFront	= cam.getFront();
			Vec3f					 camLeft	= cam.getLeft();
			Vec3f					 camUp		= cam.getUp();
			float					 fov		= cam.getFov();
			float					 ratio		= cam.getAspectRatio();
			const float				 halfHeight = tan( Util::Math::radians( fov ) * 0.5f );
			const float				 halfWidth	= ratio * halfHeight;

			Vec3f u = Util::Math::normalize( Util::Math::cross( camFront, camUp ) ) * halfWidth;
			Vec3f v = Util::Math::normalize( Util::Math::cross( camLeft, camFront ) ) * halfHeight;

			r._data._camera._position = make_float3( camPos.x, camPos.y, camPos.z );
			r._data._camera._front	  = make_float3( camFront.x, camFront.y, camFront.z );
			r._data._camera._du		  = make_float3( u.x, u.y, u.z );
			r._data._camera._dv		  = make_float3( v.x, v.y, v.z );
			r._data._nbSamples		  = 1;

			OPTIX_HANDLE_ERROR( optixSbtRecordPackHeader( _rayGeneratorProgram, &r ) );

			_rayGeneratorRecordBuffer.malloc( sizeof( RayGeneratorRecord ) );
			_rayGeneratorRecordBuffer.memcpyHostToDevice( &r, 1 );

			_shaderBindingTable.raygenRecord = _rayGeneratorRecordBuffer.getDevicePtr();
		}

		// create miss records buffer on device
		{
			MissRecord r;
			r._data._colorBackground = make_float3( VTX_SETTING().backgroundColor.getR(),
													VTX_SETTING().backgroundColor.getG(),
													VTX_SETTING().backgroundColor.getB() );

			OPTIX_HANDLE_ERROR( optixSbtRecordPackHeader( _missProgram, &r ) );

			_missRecordBuffer.malloc( sizeof( MissRecord ) );
			_missRecordBuffer.memcpyHostToDevice( &r, 1 );

			_shaderBindingTable.missRecordBase			= _missRecordBuffer.getDevicePtr();
			_shaderBindingTable.missRecordStrideInBytes = sizeof( MissRecord );
			_shaderBindingTable.missRecordCount			= 1;
		}

		// create hitgroup records buffer on device
		{
			// TODO: warning 0/1 for test
			std::vector<HitGroupRecord> records;
			if ( !_scene.getSpheres().empty() )
			{
				HitGroupRecord r;
				r._data._spheres = (Sphere *)( _scene.getSpheresDevPtr() );
				records.emplace_back( r );
				OPTIX_HANDLE_ERROR( optixSbtRecordPackHeader( _hitGroupSphereProgram, &records[ 0 ] ) );
			}
			if ( !_scene.getCylinders().empty() )
			{
				HitGroupRecord r;
				r._data._cylinders = (Cylinder *)( _scene.getCylindersDevPtr() );
				records.emplace_back( r );
				OPTIX_HANDLE_ERROR( optixSbtRecordPackHeader( _hitGroupCylinderProgram, &records[ 1 ] ) );
			}
			_hitGroupRecordsBuffer.malloc( records.size() * sizeof( HitGroupRecord ) );
			_hitGroupRecordsBuffer.memcpyHostToDevice( records.data(), records.size() );

			_shaderBindingTable.hitgroupRecordBase			= _hitGroupRecordsBuffer.getDevicePtr();
			_shaderBindingTable.hitgroupRecordStrideInBytes = sizeof( HitGroupRecord );
			_shaderBindingTable.hitgroupRecordCount			= uint( records.size() );
		}
	}

	void OptixRayTracer::_updateCam()
	{
		RayGeneratorRecord r;

		const Object3D::Camera & cam		= VTXApp::get().getScene().getCamera();
		Vec3f					 camPos		= cam.getPosition();
		Vec3f					 camFront	= cam.getFront();
		Vec3f					 camLeft	= cam.getLeft();
		Vec3f					 camUp		= cam.getUp();
		float					 fov		= cam.getFov();
		float					 ratio		= cam.getAspectRatio();
		const float				 halfHeight = tan( Util::Math::radians( fov ) * 0.5f );
		const float				 halfWidth	= ratio * halfHeight;

		Vec3f u = Util::Math::normalize( Util::Math::cross( camFront, camUp ) ) * halfWidth;
		Vec3f v = Util::Math::normalize( Util::Math::cross( camLeft, camFront ) ) * halfHeight;

		r._data._camera._position = make_float3( camPos.x, camPos.y, camPos.z );
		r._data._camera._front	  = make_float3( camFront.x, camFront.y, camFront.z );
		r._data._camera._du		  = make_float3( u.x, u.y, u.z );
		r._data._camera._dv		  = make_float3( v.x, v.y, v.z );
		r._data._nbSamples		  = 1;

		OPTIX_HANDLE_ERROR( optixSbtRecordPackHeader( _rayGeneratorProgram, &r ) );
		_rayGeneratorRecordBuffer.memcpyHostToDevice( &r, 1 );
	}
} // namespace VTX::Renderer::Optix

#endif
