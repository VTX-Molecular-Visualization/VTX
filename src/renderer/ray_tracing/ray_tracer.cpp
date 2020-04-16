#include "ray_tracer.hpp"
#include "integrators/ao_integrator.hpp"
#include "integrators/direct_lighting_integrator.hpp"
#include "integrators/raycast_integrator.hpp"
#include "lights/directional_light.hpp"
#include "lights/point_light.hpp"
#include "lights/quad_light.hpp"
#include "materials/diffuse_material.hpp"
#include "materials/flat_color_material.hpp"
#include "primitives/cylinder.hpp"
#include "primitives/molecule_ball_and_stick.hpp"
#include "primitives/plane.hpp"
#include "primitives/sphere.hpp"
#include "primitives/triangleMesh.hpp"
#include "tool/chrono.hpp"
#include "util/sampler.hpp"
#include "vtx_app.hpp"
#include <atomic>
#include <stb/stb_image_write.h>
#include <thread>

namespace VTX
{
	namespace Renderer
	{
		class RayTracer::CameraRayTracing
		{
		  public:
			CameraRayTracing( const Object3D::Camera & p_camera, const uint p_width, const uint p_height ) :
				_pos( p_camera.getPosition() ), _front( p_camera.getFront() ), _up( p_camera.getUp() ),
				_left( p_camera.getLeft() ), _width( p_width ), _height( p_height )
			{
				// spike_closed_glycans_lipids_amarolab
				/*_pos		   = Vec3f( 12.950272f, -375.106812f, 119.278503f );
				Vec3f camFront = Vec3f( -0.016405f, 0.999115f, -0.038744f );
				Vec3f camLeft  = Vec3f( -0.999817f, -0.016773f, -0.009176f );
				Vec3f camUp	   = Vec3f( -0.009818f, 0.038586f, 0.999207f );*/

				// 6vsb
				_pos   = Vec3f( 93.404381f, 176.164490f, 253.466934f );
				_front = Vec3f( 0.938164f, 0.320407f, -0.131098f );
				_left  = Vec3f( 0.112113f, 0.077086f, 0.990701f );
				_up	   = Vec3f( 0.327533f, -0.944138f, 0.036398f );

				// 6m17
				/*_pos = Vec3f( 21.587879f, 209.315125f, 178.231781f );

				Vec3f camFront = Vec3f( 0.920292f, -0.380027f, 0.092962f );
				Vec3f camLeft  = Vec3f( 0.071878f, -0.069334f, -0.995001f );
				Vec3f camUp	   = Vec3f( 0.384572f, 0.922373f, -0.036492f );*/

				//_pos = Vec3f( 302.771790f, 378.963623f, 195.313385f );

				// const Vec3f & camFront = Vec3f( -0.514253f, -0.857623f, -0.005213f );
				////_pos += camFront * 45.f;
				//_pos += camFront * 150.f;
				// const Vec3f & camLeft = Vec3f( 0.857589f, -0.514279f, 0.007590f );
				// const Vec3f & camUp	  = Vec3f( -0.009190f, -0.000568f, 0.999958f );

				// 6VSB_2nd931.obj
				/*_pos = Vec3f( 0.f, 0.f, 200.f );
				const Vec3f & camFront = Util::Math::normalize( Vec3f( -14.541f, 14.0144f, 41.9241f ) - _pos );
				Vec3f		  camUp	   = p_camera.getUp();
				Vec3f		  camLeft  = Util::Math::cross( camUp, camFront );
				camUp				   = Util::Math::cross( camLeft, camUp );*/

				const float camFov	   = p_camera.getFov();
				const float ratio	   = float( _width ) / _height;
				const float halfHeight = tan( Util::Math::radians( camFov ) * 0.5f );
				const float halfWidth  = ratio * halfHeight;

				_du = Util::Math::normalize( Util::Math::cross( _front, _up ) ) * halfWidth;
				_dv = Util::Math::normalize( Util::Math::cross( _left, _front ) ) * halfHeight;
			}

			Ray generateRay( const float p_sx, const float p_sy ) const
			{
				const Vec2f d = Vec2f( p_sx / _width, p_sy / _height ) * 2.f - 1.f;
				return Ray( _pos, Util::Math::normalize( ( _du * d.x + _dv * d.y + _front ) ) );
			}

		  private:
			Vec3f _pos;
			Vec3f _front;
			Vec3f _up;
			Vec3f _left;

			uint _width;
			uint _height;

			Vec3f _du;
			Vec3f _dv;
		};

		const uint RayTracer::TILE_SIZE = 32;

		void RayTracer::init( const uint p_width, const uint p_height )
		{
			VTX_INFO( "Initializing ray tracer..." );

			resize( p_width, p_height );

			//_integrator = new AOIntegrator;
			_integrator = new RayCastIntegrator;
			//_integrator = new DirectLightingIntegrator;

			VTX_INFO( "Ray tracer initialized" );
		}

		void RayTracer::renderFrame( const Object3D::Scene & p_scene )
		{
			VTX_INFO( "Render Frame" );
			_initScene( p_scene );
			const CameraRayTracing camera( p_scene.getCamera(), _width, _height );

			const uint nbPixelSamples = 1;

			uint size = _width * _height * 3 * sizeof( char );
			_pixels.resize( _width * _height * 3 );

			// init data for tiled rendering
			const uint nbTilesX = ( _width + TILE_SIZE - 1 ) / TILE_SIZE;
			const uint nbTilesY = ( _height + TILE_SIZE - 1 ) / TILE_SIZE;
			const uint nbTiles	= nbTilesX * nbTilesY;

			// init parallel
#ifdef _DEBUG
			const uint nbThreads = 1;
#else
			const uint nbThreads = std::thread::hardware_concurrency();
#endif
			std::vector<std::thread> threadPool;
			threadPool.reserve( nbThreads );

			VTX_INFO( "Nb threads: " + std::to_string( nbThreads ) );

			// start rendering
			Tool::Chrono chrono;

			chrono.start();

			//#pragma omp parallel for
			//			for ( int i = 0; i < int( nbTiles ); ++i )
			//			{
			//				_renderTile( pixels, camera, nbPixelSamples, uint( i ), nbTilesX, nbTilesY );
			//			}

			std::atomic<uint> nextTileId = nbThreads;

			for ( uint i = 0; i < nbThreads; ++i )
			{
				threadPool.emplace_back( std::thread(
					[ this, nbThreads, &camera, nbPixelSamples, i, nbTilesX, nbTilesY, nbTiles, &nextTileId ]() {
						_renderTiles( _pixels, camera, nbPixelSamples, i, nbTilesX, nbTilesY, nbTiles, nextTileId );
					} ) );
			}
			for ( std::thread & t : threadPool )
			{
				t.join();
			}

			chrono.stop();

			const double time = chrono.elapsedTime();

			VTX_INFO( "Rendering time: " + std::to_string( time * 1000. ) + "ms" );
		}

		void RayTracer::setShading() {}

		void RayTracer::resize( const uint p_width, const uint p_height ) { BaseRenderer::resize( p_width, p_height ); }

		void RayTracer::_initScene( const Object3D::Scene & p_scene )
		{
			_scene.clean();
			for ( std::pair<const Model::Molecule *, float> pairMol : p_scene.getMolecules() )
			{
				_scene.addObject( new MoleculeBallAndStick( pairMol.first ) );
			}

			// 6VSB_2nd931.obj
			//_scene.addObject( new TriangleMesh( DATA_DIR + "6VSB_2nd931.obj" ) );
			//_scene.addObject( new Plane( VEC3F_Y,
			//							 -90.f, //
			//							 new DiffuseMaterial( Vec3f( 0.5f, 0.6f, 0.8f ) ) ) );
			//_scene.addLight(
			//	new QuadLight( Vec3f( 200.f, 400.f, 300.f ), VEC3F_X * 80.f, VEC3F_Z * 80.f, VEC3F_XYZ, 100.f ) );

			//_scene.addObject( new Sphere( VEC3F_ZERO, 10.f, new DiffuseMaterial( Vec3f( 0.8f, 0.f, 0.f ) ) ) );

			//_scene.addObject( new Plane( -Vec3f( -0.009190f, -0.000568f, 0.999958f ),
			//							 300.f, //
			//							 new DiffuseMaterial( Vec3f( 1.5f, 0.6f, 0.8f ) ) ) );
			//

			//_scene.addLight( new PointLight( Vec3f( 316.f, 192.f, 190.f ), VEC3F_XYZ, 5000.f ) );

			//_scene.addObject( new Plane( Vec3f( 0.327533f, -0.944138f, 0.036398f ),
			//							 0.f, //
			//							 new DiffuseMaterial( Vec3f( 0.5f, 0.6f, 0.8f ) ) ) );

			//_scene.addLight( new PointLight( Vec3f( 121.587879f, 209.315125f, 171.231781f ), VEC3F_XYZ, 30000.f ) );
			//_scene.addLight( new PointLight( Vec3f( 600.f, 200.f, 400.f ), VEC3F_XYZ, 180000.f ) );
			////_scene.addLight( new DirectionalLight( Vec3f( 10.f, 1.f, 3.f ), VEC3F_XYZ ) );
			//_scene.addLight(
			//	new QuadLight( Vec3f( 200.f, 400.f, 400.f ), VEC3F_Y * 60.f, VEC3F_X * 60.f, VEC3F_XYZ, 50.f ) );

			// 6VSB
			/*_scene.addLight( new QuadLight( Vec3f( -450.f, -200.f, -38.f ),
											Vec3f( 0.327533f, -0.944138f, 0.036398f ) * 80.f,
											-Vec3f( 0.112113f, 0.077086f, 0.990701f ) * 80.f,
											VEC3F_XYZ,
											200.f ) );*/

			//// 6M17
			//_scene.addLight( new QuadLight( Vec3f( 0.f, 500.f, 188.f ),
			//								Vec3f( 0.920292f, -0.680027f, 0.092962f ) * 60.f,
			//								-Vec3f( 0.071878f, -0.069334f, -0.995001f ) * 60.f,
			//								VEC3F_XYZ,
			//								50.f ) );
		}

		void RayTracer::_renderTiles( std::vector<uchar> &	   p_image,
									  const CameraRayTracing & p_camera,
									  const uint			   p_nbPixelSamples,
									  const uint			   p_threadId,
									  const uint			   p_nbTilesX,
									  const uint			   p_nbTilesY,
									  const uint			   p_nbTiles,
									  std::atomic<uint> &	   p_nextTileId )
		{
			uint taskId = p_threadId;
			while ( taskId < p_nbTiles )
			{
				std::cout << taskId << " / " << p_nbTiles << std::endl;
				const uint tileY = taskId / p_nbTilesX;
				const uint tileX = taskId - tileY * p_nbTilesX;
				const uint x0	 = tileX * TILE_SIZE;
				const uint x1	 = Util::Math::min( x0 + TILE_SIZE, _width );
				const uint y0	 = tileY * TILE_SIZE;
				const uint y1	 = Util::Math::min( y0 + TILE_SIZE, _height );

				for ( uint y = y0; y < y1; ++y )
				{
					for ( uint x = x0; x < x1; ++x )
					{
						const Vec3f color = Util::Math::pow(
							_renderPixel( p_camera, float( x ), float( y ), p_nbPixelSamples ), 1.f / _gamma );
						const uint pixelId	   = ( x + y * _width ) * 3;
						p_image[ pixelId ]	   = uchar( color.r * 255 );
						p_image[ pixelId + 1 ] = uchar( color.g * 255 );
						p_image[ pixelId + 2 ] = uchar( color.b * 255 );
					}
				}

				taskId = p_nextTileId++;
			}
		}

		void RayTracer::_renderTile( std::vector<uchar> &	  p_image,
									 const CameraRayTracing & p_camera,
									 const uint				  p_nbPixelSamples,
									 const uint				  p_taskIndex,
									 const uint				  p_nbTilesX,
									 const uint				  p_nbTilesY )
		{
			const uint tileY = p_taskIndex / p_nbTilesX;
			const uint tileX = p_taskIndex - tileY * p_nbTilesX;
			const uint x0	 = tileX * TILE_SIZE;
			const uint x1	 = Util::Math::min( x0 + TILE_SIZE, _width );
			const uint y0	 = tileY * TILE_SIZE;
			const uint y1	 = Util::Math::min( y0 + TILE_SIZE, _height );

			for ( uint y = y0; y < y1; ++y )
			{
				for ( uint x = x0; x < x1; ++x )
				{
					const Vec3f color = Util::Math::pow(
						_renderPixel( p_camera, float( x ), float( y ), p_nbPixelSamples ), 1.f / _gamma );
					const uint pixelId	   = ( x + y * _width ) * 3;
					p_image[ pixelId ]	   = uchar( color.r * 255 );
					p_image[ pixelId + 1 ] = uchar( color.g * 255 );
					p_image[ pixelId + 2 ] = uchar( color.b * 255 );
				}
			}
		}

		Vec3f RayTracer::_renderPixel( const CameraRayTracing & p_camera,
									   const float				p_x,
									   const float				p_y,
									   const uint				p_nbPixelSamples )
		{
			Vec3f color = VEC3F_ZERO;

			// sampling ray within a pixel for anti-aliasing
			for ( uint s = 0; s < p_nbPixelSamples; s++ )
			{
				// first sample in pixel center
				const float sx = s == 0 ? p_x : p_x + Util::Math::randomFloat();
				const float sy = s == 0 ? p_y : p_y + Util::Math::randomFloat();

				const Ray ray = p_camera.generateRay( sx, sy );

				const Vec3f Li = _integrator->Li( ray, _scene, 1e-3f, FLOAT_MAX );

				color += Li;
			}
			return Util::Math::clamp( color / float( p_nbPixelSamples ), VEC3F_ZERO, VEC3F_XYZ );
		}

	} // namespace Renderer
} // namespace VTX
