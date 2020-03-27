#include "optix_ray_tracer.hpp"
#include "tool/chrono.hpp"
#include "util/sampler.hpp"
#include "vtx_app.hpp"
#include <atomic>
#include <fstream>
#include <optix_function_table_definition.h>
#include <stb/stb_image_write.h>
#include <thread>

namespace VTX
{
	namespace Renderer
	{
		struct __align__( OPTIX_SBT_RECORD_ALIGNMENT ) RayGeneratorRecord
		{
			__align__( OPTIX_SBT_RECORD_ALIGNMENT ) char header[ OPTIX_SBT_RECORD_HEADER_SIZE ];

			void * _data;
		};

		struct __align__( OPTIX_SBT_RECORD_ALIGNMENT ) MissRecord
		{
			__align__( OPTIX_SBT_RECORD_ALIGNMENT ) char header[ OPTIX_SBT_RECORD_HEADER_SIZE ];

			void * _data;
		};

		struct __align__( OPTIX_SBT_RECORD_ALIGNMENT ) HitGroupRecord
		{
			__align__( OPTIX_SBT_RECORD_ALIGNMENT ) char header[ OPTIX_SBT_RECORD_HEADER_SIZE ];

			uint _objectId;
		};

		VTX::Renderer::OptixRayTracer::~OptixRayTracer()
		{
			_rayGeneratorRecordsBuffer.free();
			_missRecordsBuffer.free();
			_hitGroupRecordsBuffer.free();
		}

		void OptixRayTracer::init( const uint p_width, const uint p_height )
		{
			VTX_INFO( "Initializing OptiX ray tracer (only first molecule)..." );

			resize( p_width, p_height );

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
				_createOptixHitGroupPrograms();

				VTX_INFO( "_createOptixPipeline..." );
				_createOptixPipeline();

				VTX_INFO( "_createOptixShaderBindingTable..." );
				_createOptixShaderBindingTable();

				_launchParametersBuffer.malloc( sizeof( _launchParameters ) );
			}
			catch ( const std::exception & e )
			{
				VTX_ERROR( e.what() );
				// TODO: handle it ! :-)
			}

			VTX_INFO( "Optix ray tracer initialized" );
		}

		void OptixRayTracer::renderFrame( const Object3D::Scene & p_scene )
		{
			if ( _launchParameters._frameBufferDim.x == 0 || _launchParameters._frameBufferDim.y == 0 ) return;

			VTX_INFO( "Render Frame" );

			// start rendering
			// TODO: replace by ChronoGPU
			Tool::Chrono chrono;

			chrono.start();
			_launchParametersBuffer.memcpyHostToDevice( &_launchParameters, 1 );
			_launchParameters._frameId++;

			OPTIX_HANDLE_ERROR( optixLaunch( _optixPipeline,
											 _cudaStream,
											 _launchParametersBuffer.getDevicePtr(),
											 _launchParametersBuffer._size,
											 &_shaderBindingTable,
											 _launchParameters._frameBufferDim.x,
											 _launchParameters._frameBufferDim.y,
											 1 ) );

			chrono.stop();

			const double time = chrono.elapsedTime();

			VTX_INFO( "Rendering time: " + std::to_string( time ) );
			VTX_INFO( "Save image as: test RT.png" );

			// sync - make sure the frame is rendered before we download and
			// display (obviously, for a high-performance application you
			// want to use streams and double-buffering, but for this simple
			// example, this will have to do)
			CUDA_SYNCHRONIZE_HANDLE_ERROR();

			std::vector<uchar> pixels( _width * _height * 4 );
			_colorBuffer.memcpyDeviceToHost(
				(uint *)( pixels.data() ), _launchParameters._frameBufferDim.x * _launchParameters._frameBufferDim.y );

			stbi_write_png( "test Optix.png", _width, _height, 4, pixels.data(), 0 );
		}

		void OptixRayTracer::setShading() {}

		void OptixRayTracer::resize( const uint p_width, const uint p_height )
		{
			BaseRenderer::resize( p_width, p_height );
			_colorBuffer.resize( _width * _height * sizeof( uint ) );
			_launchParameters._frameBufferDim = { _width, _height };
			_launchParameters._colorBuffer	  = (uint *)( _colorBuffer.getDevicePtr() );
		}

		void OptixRayTracer::_initOptix()
		{
			// check for CUDA capable devices and choose "best" one
			cudaFree( 0 );

			_bestDeviceId = Util::CUDA::chooseBestDevice();

			VTX_INFO( "CUDA device capable found" );
			cudaGetDeviceProperties( &_deviceProperties, _bestDeviceId );

			std::cout << _deviceProperties.name << std::endl;
			std::cout << "Compute capability: " << _deviceProperties.major << "." << _deviceProperties.minor
					  << std::endl;

			if ( false /*TODO: check what compute capability we need*/ )
			{ throw std::runtime_error( "no CUDA capable device is detected with compute capability >= X.Y" ); }

			OPTIX_HANDLE_ERROR( optixInit() );
		}

		void OptixRayTracer::_createOptixContext()
		{
			CUDA_HANDLE_ERROR( cudaSetDevice( _bestDeviceId ) );
			CUDA_HANDLE_ERROR( cudaStreamCreate( &_cudaStream ) );

			CUresult res = cuCtxGetCurrent( &_cudaContext );

			if ( res != CUDA_SUCCESS ) { throw std::runtime_error( "Error getting CUDA context:" + res ); }

			OPTIX_HANDLE_ERROR( optixDeviceContextCreate( _cudaContext, 0, &_optixContext ) );
			// TODO: log ?
		}

		void OptixRayTracer::_createOptixModule()
		{
			// TODO: why 50 ? fix that
			_optixModuleCompileOptions.maxRegisterCount = 50;
			_optixModuleCompileOptions.optLevel			= OPTIX_COMPILE_OPTIMIZATION_LEVEL_3;
			_optixModuleCompileOptions.debugLevel		= OPTIX_COMPILE_DEBUG_LEVEL_NONE;

			_optixPipelineCompileOptions = {};
			// we use only one GAS (geometry acceleration structure)
			_optixPipelineCompileOptions.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_GAS;
			// 3 payload registers (R G B output)
			_optixPipelineCompileOptions.numPayloadValues = 3;
			// How much storage, in 32b words, to make available for the attributes.
			// The minimum number is 2. Values
			// below that will automatically be changed to 2. [2..8]
			_optixPipelineCompileOptions.numAttributeValues = 2;
			_optixPipelineCompileOptions.exceptionFlags		= OPTIX_EXCEPTION_FLAG_NONE;
			// name of the struct used as parameter variable on device code
			_optixPipelineCompileOptions.pipelineLaunchParamsVariableName = "params";

			_optixPipelineLinkOptions.overrideUsesMotionBlur = false;
			_optixPipelineLinkOptions.maxTraceDepth			 = 2;
			_optixPipelineLinkOptions.debugLevel			 = OPTIX_COMPILE_DEBUG_LEVEL_NONE;

#define NVRTC_DOES_NOT_WORK
#ifndef NVRTC_DOES_NOT_WORK
			// compitation to PTX
			// TODO
			// First try to compile with NVRTC, does not work

			// read .cu
			const std::string file = "../src/renderer/optix_ray_tracer/testOptix.cu";
			std::ifstream	  ifs( file, std::ios::binary | std::ios::ate );
			if ( !ifs.is_open() ) { throw std::runtime_error( "Cannot open file " + file ); }

			uint   size = uint( ifs.tellg() );
			char * cu	= new char[ size + 1 ];

			ifs.seekg( 0, std::ios::beg );
			ifs.read( cu, size );
			ifs.close();

			std::cout << "========> " << size << std::endl;

			nvrtcProgram program;
			NVRTC_HANDLE_ERROR( nvrtcCreateProgram( &program, cu, "testOptix.cu", 0, nullptr, nullptr ) );

			const std::string vtxIncludesDir = std::string( "-I ../.." );
			const std::string cudaIncludesDir
				= std::string( "-I C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.2/include" );
			const std::string optixIncludesDir
				= std::string( "-I C:/ProgramData/NVIDIA Corporation/OptiX SDK 7.0.0/include/" );
			const std::string ucrtIncludesDir
				= std::string( "-I C:/Program Files (x86)/Windows Kits/10/Include/10.0.18362.0/ucrt" );
			const std::string vcIncludesDir = std::string(
				"-I C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.25.28610/include" );

			const char * compileOptions[]
				= { "--gpu-architecture=compute_30", // TODO: 3.0 ? 7.5 for rtx but we will need without rtx too I guess
					"--use_fast_math",
					"--std=c++11",
					"-D_WIN64",
					vtxIncludesDir.c_str(),
					cudaIncludesDir.c_str(),
					optixIncludesDir.c_str(),
					ucrtIncludesDir.c_str(),
					vcIncludesDir.c_str() };

			nvrtcResult compileRes = nvrtcCompileProgram( program, 9, compileOptions );

			// get compilation log
			size_t logSize;
			NVRTC_HANDLE_ERROR( nvrtcGetProgramLogSize( program, &logSize ) );
			std::string log;
			log.resize( logSize );
			NVRTC_HANDLE_ERROR( nvrtcGetProgramLog( program, log.data() ) );

			if ( compileRes != NVRTC_SUCCESS )
			{
				nvrtcDestroyProgram( &program );
				throw std::runtime_error( log );
			}

			std::string ptx;
			NVRTC_HANDLE_ERROR( nvrtcGetPTX( program, ptx.data() ) );

			nvrtcDestroyProgram( &program );

			delete cu;
#endif
			// read .ptx
			const std::string file = "../src/renderer/optix_ray_tracer/testOptix.ptx";
			std::ifstream	  ifs( file, std::ios::binary | std::ios::ate );
			if ( !ifs.is_open() ) { throw std::runtime_error( "Cannot open file " + file ); }

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
			if ( sizeof_log > 1 )
			{
				VTX_INFO( "optixModuleCreateFromPTX (_createOptixModule) log :" );
				VTX_INFO( log );
			}
		}

		void OptixRayTracer::_createOptixRayGeneratorPrograms()
		{
			// only one ray generator program
			_rayGeneratorPrograms.resize( 1 );

			OptixProgramGroupOptions programOptions		= {};
			OptixProgramGroupDesc	 programDescription = {};
			programDescription.kind						= OPTIX_PROGRAM_GROUP_KIND_RAYGEN;
			programDescription.raygen.module			= _optixModule;
			programDescription.raygen.entryFunctionName = "__raygen__renderFrame";

			char   log[ 2048 ];
			size_t sizeof_log = sizeof( log );
			OPTIX_HANDLE_ERROR( optixProgramGroupCreate( _optixContext,
														 &programDescription,
														 1,
														 &programOptions,
														 log,
														 &sizeof_log,
														 &_rayGeneratorPrograms[ 0 ] ) );

			if ( sizeof_log > 1 )
			{
				VTX_INFO( "optixProgramGroupCreate (_createOptixRayGeneratorPrograms) log :" );
				VTX_INFO( log );
			}
		}

		void OptixRayTracer::_createOptixMissPrograms()
		{
			// only one miss program
			_missPrograms.resize( 1 );

			OptixProgramGroupOptions programOptions		= {};
			OptixProgramGroupDesc	 programDescription = {};
			programDescription.kind						= OPTIX_PROGRAM_GROUP_KIND_MISS;
			programDescription.miss.module				= _optixModule;
			programDescription.miss.entryFunctionName	= "__miss__radiance";

			char   log[ 2048 ];
			size_t sizeof_log = sizeof( log );
			OPTIX_HANDLE_ERROR( optixProgramGroupCreate(
				_optixContext, &programDescription, 1, &programOptions, log, &sizeof_log, &_missPrograms[ 0 ] ) );

			if ( sizeof_log > 1 )
			{
				VTX_INFO( "optixProgramGroupCreate (_createOptixMissPrograms) log :" );
				VTX_INFO( log );
			}
		}

		void OptixRayTracer::_createOptixHitGroupPrograms()
		{
			// only one hit group program
			_hitGroupPrograms.resize( 1 );

			OptixProgramGroupOptions programOptions		= {};
			OptixProgramGroupDesc	 programDescription = {};
			programDescription.kind						= OPTIX_PROGRAM_GROUP_KIND_HITGROUP;
			// closest hit
			programDescription.hitgroup.moduleCH			= _optixModule;
			programDescription.hitgroup.entryFunctionNameCH = "__closesthit__radiance";
			// any hit
			programDescription.hitgroup.moduleAH			= _optixModule;
			programDescription.hitgroup.entryFunctionNameAH = "__anyhit__radiance";

			char   log[ 2048 ];
			size_t sizeof_log = sizeof( log );
			OPTIX_HANDLE_ERROR( optixProgramGroupCreate(
				_optixContext, &programDescription, 1, &programOptions, log, &sizeof_log, &_hitGroupPrograms[ 0 ] ) );

			if ( sizeof_log > 1 )
			{
				VTX_INFO( "optixProgramGroupCreate (_createOptixHitGroupPrograms) log :" );
				VTX_INFO( log );
			}
		}

		void OptixRayTracer::_createOptixPipeline()
		{
			std::vector<OptixProgramGroup> programGroups;

			for ( const OptixProgramGroup & g : _rayGeneratorPrograms )
			{
				programGroups.emplace_back( g );
			}
			for ( const OptixProgramGroup & g : _missPrograms )
			{
				programGroups.emplace_back( g );
			}
			for ( const OptixProgramGroup & g : _hitGroupPrograms )
			{
				programGroups.emplace_back( g );
			}

			char   log[ 2048 ];
			size_t sizeof_log = sizeof( log );
			OPTIX_HANDLE_ERROR( optixPipelineCreate( _optixContext,
													 &_optixPipelineCompileOptions,
													 &_optixPipelineLinkOptions,
													 programGroups.data(),
													 int( programGroups.size() ),
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

			if ( sizeof_log > 1 )
			{
				VTX_INFO( "optixPipelineSetStackSize (_createOptixHitGroupPrograms) log :" );
				VTX_INFO( log );
			}
		}

		void OptixRayTracer::_createOptixShaderBindingTable()
		{
			// create ray generator records buffer on device
			{
				std::vector<RayGeneratorRecord> rayGeneratorRecords;
				for ( const OptixProgramGroup & g : _rayGeneratorPrograms )
				{
					RayGeneratorRecord r;
					OPTIX_HANDLE_ERROR( optixSbtRecordPackHeader( g, &r ) );
					r._data = nullptr; // nothing for this test
					rayGeneratorRecords.emplace_back( r );
				}
				_rayGeneratorRecordsBuffer.malloc( rayGeneratorRecords.size() * sizeof( RayGeneratorRecord ) );
				_rayGeneratorRecordsBuffer.memcpyHostToDevice( rayGeneratorRecords.data(), rayGeneratorRecords.size() );

				_shaderBindingTable.raygenRecord = _rayGeneratorRecordsBuffer.getDevicePtr();
			}

			// create miss records buffer on device
			{
				std::vector<MissRecord> missRecords;
				for ( const OptixProgramGroup & g : _missPrograms )
				{
					MissRecord r;
					OPTIX_HANDLE_ERROR( optixSbtRecordPackHeader( g, &r ) );
					r._data = nullptr; // nothing for this test
					missRecords.emplace_back( r );
				}
				_missRecordsBuffer.malloc( missRecords.size() * sizeof( MissRecord ) );
				_missRecordsBuffer.memcpyHostToDevice( missRecords.data(), missRecords.size() );

				_shaderBindingTable.missRecordBase			= _missRecordsBuffer.getDevicePtr();
				_shaderBindingTable.missRecordStrideInBytes = sizeof( MissRecord );
				_shaderBindingTable.missRecordCount			= int( missRecords.size() );
			}

			// create hitgroup records buffer on device
			{
				// in this test no object
				// but create dummy one to avoid null pointer
				uint						nbObjects = 1;
				std::vector<HitGroupRecord> hitGroupRecords;
				for ( uint i = 0; i < nbObjects; ++i )
				{
					int			   objectType = 0;
					HitGroupRecord r;
					OPTIX_HANDLE_ERROR( optixSbtRecordPackHeader( _hitGroupPrograms[ objectType ], &r ) );
					r._objectId = i;
					hitGroupRecords.push_back( r );
				}
				_hitGroupRecordsBuffer.malloc( hitGroupRecords.size() * sizeof( HitGroupRecord ) );
				_hitGroupRecordsBuffer.memcpyHostToDevice( hitGroupRecords.data(), hitGroupRecords.size() );

				_shaderBindingTable.hitgroupRecordBase			= _hitGroupRecordsBuffer.getDevicePtr();
				_shaderBindingTable.hitgroupRecordStrideInBytes = sizeof( HitGroupRecord );
				_shaderBindingTable.hitgroupRecordCount			= int( hitGroupRecords.size() );
			}
		}

	} // namespace Renderer
} // namespace VTX
