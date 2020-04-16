#include "optix_ray_tracer.hpp"
#include "tool/chrono.hpp"
#include "util/color.hpp"
#include "util/sampler.hpp"
#include "vtx_app.hpp"
#include <atomic>
#include <fstream>
#include <nvtx3/nvToolsExt.h>
#include <optix_function_table_definition.h>
#include <stb/stb_image_write.h>
#include <thread>
#include <unordered_set>

#define SPHERES

namespace VTX
{
	namespace Renderer
	{
		static void context_log_cb( unsigned int level, const char * tag, const char * message, void * )
		{
			fprintf( stderr, "[%2d][%12s]: %s\n", (int)level, tag, message );
		}

		VTX::Renderer::OptixRayTracer::~OptixRayTracer()
		{
			_spheresDevBuffer.free();
			_cylindersDevBuffer.free();
			_gasOutputBuffer.free();

			_rayGeneratorRecordsBuffer.free();
			_missRecordsBuffer.free();
			_hitGroupRecordsBuffer.free();

			optixPipelineDestroy( _optixPipeline );

			for ( const OptixProgramGroup & g : _rayGeneratorPrograms )
			{
				optixProgramGroupDestroy( g );
			}
			for ( const OptixProgramGroup & g : _missPrograms )
			{
				optixProgramGroupDestroy( g );
			}
			for ( const OptixProgramGroup & g : _hitGroupPrograms )
			{
				optixProgramGroupDestroy( g );
			}

			optixModuleDestroy( _optixModule );
			optixDeviceContextDestroy( _optixContext );
			cuStreamDestroy( _cudaStream );
			cuCtxDestroy( _cudaContext );
		}

		void OptixRayTracer::init( const uint p_width, const uint p_height )
		{
			VTX_INFO( "Initializing OptiX ray tracer (only first molecule)..." );

			resize( p_width, p_height );

			_spheresDevBuffer.free();
			_cylindersDevBuffer.free();
			_gasOutputBuffer.free();

			_rayGeneratorRecordsBuffer.free();
			_missRecordsBuffer.free();
			_hitGroupRecordsBuffer.free();

			// init scene on host and device !!!
			const Model::Molecule * mol		= VTXApp::get().getScene().getMolecules().begin()->first;
			const uint				nbAtoms = mol->getAtomCount();
			const uint				nbBonds = mol->getBondCount();
			std::cout << "nbAtoms " << nbAtoms << "(size on GPU: " << nbAtoms * sizeof( Optix::Sphere ) << ")"
					  << std::endl;
			std::cout << "nbBonds " << nbBonds << "(size on GPU: " << nbBonds * sizeof( Optix::Cylinder ) << ")"
					  << std::endl;
			_spheres.resize( nbAtoms );
			_cylinders.resize( nbBonds );
			const std::vector<Vec3f> & atomPositions = mol->getAtomPositionFrame( 0 );
			std::vector<float3>		   colors;
			for ( uint i = 0; i < nbAtoms; ++i )
			{
				const Vec3f & p		  = atomPositions[ i ];
				const float	  r		  = mol->getAtomRadius( i );
				const Vec3f & c		  = mol->getAtomColor( i );
				_spheres[ i ]._center = make_float3( p.x, p.y, p.z );
				_spheres[ i ]._radius = r;
				uint colorId		  = INVALID_ID;
				for ( uint j = 0; j < uint( colors.size() ); ++j )
				{
					if ( colors[ j ].x == c.x && colors[ j ].y == c.y && colors[ j ].z == c.z )
					{
						colorId = j;
						break;
					}
				}
				if ( colorId == INVALID_ID )
				{
					colorId = uint( colors.size() );
					colors.emplace_back( make_float3( c.x, c.y, c.z ) );
				}
				_spheres[ i ]._colorId = colorId;
			}

			for ( uint i = 0; i < nbBonds; ++i )
			{
				const Model::Bond & bond = mol->getBond( i );
				const Vec3f &		p0	 = atomPositions[ bond.getIndexFirstAtom() ];
				const Vec3f &		p1	 = atomPositions[ bond.getIndexSecondAtom() ];
				_cylinders[ i ]._v0		 = make_float3( p0.x, p0.y, p0.z );
				_cylinders[ i ]._v1		 = make_float3( p1.x, p1.y, p1.z );
				_cylinders[ i ]._colorId = _spheres[ bond.getIndexFirstAtom() ]._colorId;
			}

#ifdef SPHERES
			_spheresDevBuffer.malloc( _spheres.size() * sizeof( Optix::Sphere ) );
			_spheresDevBuffer.memcpyHostToDevice( _spheres.data(), _spheres.size() );
#else
			_cylindersDevBuffer.malloc( _cylinders.size() * sizeof( Optix::Cylinder ) );
			_cylindersDevBuffer.memcpyHostToDevice( _cylinders.data(), _cylinders.size() );
#endif
			_colorsDevBuffer.malloc( colors.size() * sizeof( float3 ) );
			_colorsDevBuffer.memcpyHostToDevice( colors.data(), colors.size() );
			_launchParameters._colors = (float3 *)( _colorsDevBuffer.getDevicePtr() );

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
				// TODO: handle it ! :-)
			}

			VTX_INFO( "Optix ray tracer initialized" );
		}

		void OptixRayTracer::renderFrame( const Object3D::Scene & p_scene )
		{
			if ( _launchParameters._frame._width == 0 || _launchParameters._frame._height == 0 ) return;

			VTX_INFO( "Render Frame" );

			cudaEvent_t start, stop;
			float		elapsedTime;
			CUDA_HANDLE_ERROR( cudaEventCreate( &start ) );
			CUDA_HANDLE_ERROR( cudaEventCreate( &stop ) );

			_launchParametersBuffer.memcpyHostToDevice( &_launchParameters, 1 );
			_launchParameters._frame._id++;

			CUDA_HANDLE_ERROR( cudaEventRecord( start, 0 ) );

			nvtxRangePushA( "OptiX" ); // for profiling, when knowing how it works :-p
			OPTIX_HANDLE_ERROR( optixLaunch( _optixPipeline,
											 _cudaStream,
											 _launchParametersBuffer.getDevicePtr(),
											 _launchParametersBuffer._sizeInBytes,
											 &_shaderBindingTable,
											 _launchParameters._frame._width,
											 _launchParameters._frame._height,
											 1 ) );
			nvtxRangePop();

			CUDA_HANDLE_ERROR( cudaEventRecord( stop, 0 ) );
			CUDA_HANDLE_ERROR( cudaEventSynchronize( stop ) );
			CUDA_HANDLE_ERROR( cudaEventElapsedTime( &elapsedTime, start, stop ) );

			VTX_INFO( "Rendering time: " + std::to_string( elapsedTime ) );
			VTX_INFO( "Save image as: test Optix.png" );

			// sync - make sure the frame is rendered before we download and
			// display (obviously, for a high-performance application you
			// want to use streams and double-buffering, but for this simple
			// example, this will have to do)
			CUDA_SYNCHRONIZE_HANDLE_ERROR();

			_pixels.resize( _width * _height );
			_pixelsBuffer.memcpyDeviceToHost( (uchar4 *)( _pixels.data() ),
											  _launchParameters._frame._width * _launchParameters._frame._height );

			VTX_INFO( "Saved" );
		}

		void OptixRayTracer::setShading() {}

		void OptixRayTracer::resize( const uint p_width, const uint p_height )
		{
			BaseRenderer::resize( p_width, p_height );
			_pixelsBuffer.realloc( _width * _height * sizeof( uint ) );
			_launchParameters._frame._width	 = _width;
			_launchParameters._frame._height = _height;
			_launchParameters._frame._pixels = (uchar4 *)( _pixelsBuffer.getDevicePtr() );
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
			OPTIX_HANDLE_ERROR( optixDeviceContextSetLogCallback( _optixContext, context_log_cb, nullptr, 4 ) );
		}

		void OptixRayTracer::_createOptixModule()
		{
			// TODO: why 50 ? fix that
			_optixModuleCompileOptions					= {};
			_optixModuleCompileOptions.maxRegisterCount = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT;
			_optixModuleCompileOptions.optLevel			= OPTIX_COMPILE_OPTIMIZATION_LEVEL_3;
			_optixModuleCompileOptions.debugLevel		= OPTIX_COMPILE_DEBUG_LEVEL_NONE;

			_optixPipelineCompileOptions = {};
			// we use only one GAS (geometry acceleration structure)
			_optixPipelineCompileOptions.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_GAS;
			// 3 payload registers (R G B output)
			_optixPipelineCompileOptions.numPayloadValues = 3;
			_optixPipelineCompileOptions.usesMotionBlur	  = false;
			// How much storage, in 32b words, to make available for the attributes.
			// Here 3 for normal
			_optixPipelineCompileOptions.numAttributeValues = 3;
			_optixPipelineCompileOptions.exceptionFlags		= OPTIX_EXCEPTION_FLAG_NONE;
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
			if ( !ifs.is_open() ) { throw std::runtime_error( "Cannot open file " + file ); }

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
#else
			// read .ptx
			const std::string file = "../src/renderer/optix_ray_tracer/test_optix.ptx";
			std::ifstream	  ifs( file, std::ios::binary | std::ios::ate );
			if ( !ifs.is_open() ) { throw std::runtime_error( "Cannot open file " + file ); }

			uint   ptxSize = uint( ifs.tellg() );
			char * ptx	   = new char[ ptxSize + 1 ];

			ifs.seekg( 0, std::ios::beg );
			ifs.read( ptx, ptxSize );
			ifs.close();
#endif

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
			programDescription.raygen.entryFunctionName = "__raygen__";

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
			programDescription.miss.entryFunctionName	= "__miss__";

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
			programDescription.hitgroup.moduleCH = _optixModule;
			// any hit
#ifdef SPHERES
			programDescription.hitgroup.entryFunctionNameCH = "__closesthit__sphere";
#else
			programDescription.hitgroup.entryFunctionNameCH = "__closesthit__cylinder";
#endif
			programDescription.hitgroup.moduleAH			= nullptr; //_optixModule;
			programDescription.hitgroup.entryFunctionNameAH = nullptr; //"__anyhit__";
			// intersection
			programDescription.hitgroup.moduleIS = _optixModule;
#ifdef SPHERES
			programDescription.hitgroup.entryFunctionNameIS = "__intersection__sphere";
#else
			programDescription.hitgroup.entryFunctionNameIS = "__intersection__cylinder";
#endif

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

		OptixTraversableHandle Renderer::OptixRayTracer::_buildGAS()
		{
			// create AABB buffer from sphere
			std::vector<OptixAabb> aabbs;

#ifdef SPHERES
			aabbs.resize( _spheres.size() );
			for ( uint i = 0; i < uint( _spheres.size() ); ++i )
			{
				aabbs[ i ] = _spheres[ i ].aabb();
			}
#else
			aabbs.resize( _cylinders.size() );
			for ( uint i = 0; i < uint( _cylinders.size() ); ++i )
			{
				aabbs[ i ] = _cylinders[ i ].aabb();
			}
#endif
			CudaBuffer aabbsBuffer;
			aabbsBuffer.malloc( aabbs.size() * sizeof( OptixAabb ) );
			aabbsBuffer.memcpyHostToDevice( aabbs.data(), aabbs.size() );

			OptixBuildInput aabbInput		  = {};
			CUdeviceptr		devPtr			  = aabbsBuffer.getDevicePtr();
			aabbInput.type					  = OPTIX_BUILD_INPUT_TYPE_CUSTOM_PRIMITIVES;
			aabbInput.aabbArray.aabbBuffers	  = &devPtr;
			aabbInput.aabbArray.numPrimitives = uint( aabbs.size() );

			uint32_t aabbInputFlags[ 1 ]	  = { OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT };
			aabbInput.aabbArray.flags		  = aabbInputFlags;
			aabbInput.aabbArray.numSbtRecords = 1;

			// build gas
			OptixTraversableHandle gasHandle;
			OptixAccelBuildOptions accelOptions = {};
			accelOptions.buildFlags				= OPTIX_BUILD_FLAG_ALLOW_COMPACTION;
			accelOptions.operation				= OPTIX_BUILD_OPERATION_BUILD;
			accelOptions.motionOptions.numKeys	= 0;

			OptixAccelBufferSizes gasBufferSizes;
			OPTIX_HANDLE_ERROR(
				optixAccelComputeMemoryUsage( _optixContext, &accelOptions, &aabbInput, 1, &gasBufferSizes ) );
			CudaBuffer tempBufferGAS;
			tempBufferGAS.malloc( gasBufferSizes.tempSizeInBytes );

			// non-compact output
			CudaBuffer tempOutputGasCompacted;
			size_t	   compactSizeOffset = ( ( gasBufferSizes.outputSizeInBytes + 8ull - 1ull ) / 8ull ) * 8ull;
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
			CUDA_HANDLE_ERROR( cudaMemcpy(
				&compactedGasSize, (void *)emitProperties.result, sizeof( size_t ), cudaMemcpyDeviceToHost ) );

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
			_shaderBindingTable = {};
			// create ray generator records buffer on device
			{
				std::vector<Optix::RayGeneratorRecord> rayGeneratorRecords;
				for ( const OptixProgramGroup & g : _rayGeneratorPrograms )
				{
					Optix::RayGeneratorRecord r;

					// spike_closed_glycans_lipids_amarolab
					/*Vec3f camPos   = Vec3f( 12.950272f, -375.106812f, 119.278503f );
					Vec3f camFront = Vec3f( -0.016405f, 0.999115f, -0.038744f );
					Vec3f camUp	   = Vec3f( -0.009818f, 0.038586f, 0.999207f );*/
					// 6vsb
					Vec3f camPos   = Vec3f( 93.404381f, 176.164490f, 253.466934f );
					Vec3f camFront = Vec3f( 0.938164f, 0.320407f, -0.131098f );
					Vec3f camLeft  = Vec3f( 0.112113f, 0.077086f, 0.990701f );
					Vec3f camUp	   = Vec3f( 0.327533f, -0.944138f, 0.036398f );

					float		fov		   = 60.f;
					float		ratio	   = float( _width ) / _height;
					const float halfHeight = tan( Util::Math::radians( fov ) * 0.5f );
					const float halfWidth  = ratio * halfHeight;

					Vec3f u = Util::Math::normalize( Util::Math::cross( camFront, camUp ) ) * halfWidth;
					Vec3f v = Util::Math::normalize( Util::Math::cross( camLeft, camFront ) ) * halfHeight;

					r._data._camera._position = make_float3( camPos.x, camPos.y, camPos.z );
					r._data._camera._front	  = make_float3( camFront.x, camFront.y, camFront.z );
					r._data._camera._du		  = make_float3( u.x, u.y, u.z );
					r._data._camera._dv		  = make_float3( v.x, v.y, v.z );

					OPTIX_HANDLE_ERROR( optixSbtRecordPackHeader( g, &r ) );
					rayGeneratorRecords.emplace_back( r );
				}
				_rayGeneratorRecordsBuffer.malloc( rayGeneratorRecords.size() * sizeof( Optix::RayGeneratorRecord ) );
				_rayGeneratorRecordsBuffer.memcpyHostToDevice( rayGeneratorRecords.data(), rayGeneratorRecords.size() );

				_shaderBindingTable.raygenRecord = _rayGeneratorRecordsBuffer.getDevicePtr();
			}

			// create miss records buffer on device
			{
				std::vector<Optix::MissRecord> missRecords;
				for ( const OptixProgramGroup & g : _missPrograms )
				{
					Optix::MissRecord r;
					r._data._colorBackground
						= make_float3( _backgroundColor.r, _backgroundColor.g, _backgroundColor.b );
					OPTIX_HANDLE_ERROR( optixSbtRecordPackHeader( g, &r ) );
					missRecords.emplace_back( r );
				}
				_missRecordsBuffer.malloc( missRecords.size() * sizeof( Optix::MissRecord ) );
				_missRecordsBuffer.memcpyHostToDevice( missRecords.data(), missRecords.size() );

				_shaderBindingTable.missRecordBase			= _missRecordsBuffer.getDevicePtr();
				_shaderBindingTable.missRecordStrideInBytes = sizeof( Optix::MissRecord );
				_shaderBindingTable.missRecordCount			= int( missRecords.size() );
			}

			// create hitgroup records buffer on device
			{
				// in this test no object
				// but create dummy one to avoid null pointer
				uint							   nbObjects = 1;
				std::vector<Optix::HitGroupRecord> hitGroupRecords;
				for ( uint i = 0; i < nbObjects; ++i )
				{
					int objectType = 0;

					Optix::HitGroupRecord r;
#ifdef SPHERES
					r._data._spheres = (Optix::Sphere *)( _spheresDevBuffer.getDevicePtr() );
#else
					r._data._cylinders = (Optix::Cylinder *)( _cylindersDevBuffer.getDevicePtr() );
#endif
					OPTIX_HANDLE_ERROR( optixSbtRecordPackHeader( _hitGroupPrograms[ objectType ], &r ) );
					hitGroupRecords.push_back( r );
				}
				_hitGroupRecordsBuffer.malloc( hitGroupRecords.size() * sizeof( Optix::HitGroupRecord ) );
				_hitGroupRecordsBuffer.memcpyHostToDevice( hitGroupRecords.data(), hitGroupRecords.size() );

				_shaderBindingTable.hitgroupRecordBase			= _hitGroupRecordsBuffer.getDevicePtr();
				_shaderBindingTable.hitgroupRecordStrideInBytes = sizeof( Optix::HitGroupRecord );
				_shaderBindingTable.hitgroupRecordCount			= int( hitGroupRecords.size() );
			}
		}

	} // namespace Renderer
} // namespace VTX
