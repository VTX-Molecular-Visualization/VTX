#include "ray_tracer.hpp"
#include "integrators/ao_integrator.hpp"
#include "integrators/direct_lighting_integrator.hpp"
#include "integrators/raycast_integrator.hpp"
#include "lights/directional_light.hpp"
#include "lights/point_light.hpp"
#include "lights/quad_light.hpp"
#include "materials/flat_color_material.hpp"
#include "materials/matte.hpp"
#include "primitives/cylinder.hpp"
#include "primitives/molecule_rt.hpp"
#include "primitives/plane.hpp"
#include "primitives/sphere.hpp"
#include "primitives/triangle_mesh.hpp"
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
				//
				//
				// ====================================================
				// Spike_ACE2.pdb
				/*_pos   = Vec3f( -62.360699f, 270.402008f, -6.388180f );
				_front = Vec3f( 0.216708f, -0.974968f, 0.048667f );
				_left  = Vec3f( -0.976223f, -0.216183f, 0.015855f );
				_up	   = Vec3f( -0.004926f, -0.050945f, -0.998636f );*/
				// ====================================================
				//
				//

				//
				//
				// ====================================================
				// Molecule of the month may 20 : 3jb9
				/*_pos   = Vec3f( 491.377930f, 375.845001f, 223.546844f );
				_front = Vec3f( -0.844256f, -0.535781f, 0.013080f );
				_left  = Vec3f( 0.532688f, -0.836197f, 0.130456f );
				_up	   = Vec3f( -0.058959f, 0.117106f, 0.991368f );*/
				// ====================================================
				//
				//

				// spike_closed_glycans_lipids_amarolab
				/*_pos   = Vec3f( 12.950272f, -375.106812f, 119.278503f );
				_front = Vec3f( -0.016405f, 0.999115f, -0.038744f );
				_left  = Vec3f( -0.999817f, -0.016773f, -0.009176f );
				_up	   = Vec3f( -0.009818f, 0.038586f, 0.999207f );*/

				//
				//
				// ====================================================
				// spike_closed_cleaved_full_amarolab
				//_pos   = Vec3f( 0.f, -370.006958f, 215.602661f );
				//_front = Vec3f( 0.000138f, 0.954886f, -0.296973f );
				//_left  = Vec3f( -0.999969f, 0.002475f, 0.007492f );
				//_up	   = Vec3f( 0.007889f, 0.296962f, 0.954857f );
				// ====================================================
				//
				//

				/*_pos   = Vec3f( 0.f, -390.106812f, 110.278503f );
				_front = Vec3f( 0.f, 1.f, 0.f );
				_left  = Vec3f( -1.f, 0.f, 0.f );
				_up	   = Vec3f( 0.f, 0.f, 1.f );*/
				//_pos += 150.f * _front;

				/*Position:
				vec3(421.417175, 245.391190, 208.967499)[08:45:19][ INFO ] Rotation :
					dquat(0.467604, { 0.477807, 0.537256, 0.514095 })[08:45:19][ INFO ] Euler angles :
					dvec3(0.000000, 0.000000, 0.000000)[08:45:19][ INFO ] Front :
					vec3(-0.993723, -0.105553, 0.033887)[08:45:19][ INFO ] Left :
					vec3(0.105876, -0.994196, 0.011171)[08:45:19][ INFO ] Up :
					vec3( 0.032623, 0.014814, 0.999251 )*/

				//
				//
				// ====================================================
				// 6VSB
				/*_pos   = Vec3f( 421.417175f, 245.391190f, 208.967499f );
				_front = Vec3f( -0.993723f, -0.105553f, 0.033887f );
				_left  = Vec3f( 0.105876f, -0.994196f, 0.011171f );
				_up	   = Vec3f( 0.032623f, 0.014814f, 0.999251f );*/
				// ====================================================
				//
				//

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

				//_pos.z += 10.f;

				const float camFov	   = p_camera.getFov();
				const float ratio	   = float( _width ) / _height;
				const float halfHeight = tan( Util::Math::radians( camFov ) * 0.5f );
				const float halfWidth  = ratio * halfHeight;

				_du = Util::Math::normalize( Util::Math::cross( _front, _up ) ) * halfWidth;
				_dv = Util::Math::normalize( Util::Math::cross( _left, _front ) ) * halfHeight;

				/*
				std::cout << "Camera RT" << std::endl;
				std::cout << "- pos   : " << _pos.x << " / " << _pos.y << " / " << _pos.z << std::endl;
				std::cout << "- front : " << _front.x << " / " << _front.y << " / " << _front.z << std::endl;
				std::cout << "- up    : " << _up.x << " / " << _up.y << " / " << _up.z << std::endl;
				std::cout << "- left  : " << _left.x << " / " << _left.y << " / " << _left.z << std::endl;
				*/
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

			_integrator = new RayCastIntegrator;
			//_integrator	  = new DirectLightingIntegrator;
			//_aoIntegrator = new AOIntegrator( 50.f, 12 );

			VTX_INFO( "Init Scene" );
			_initScene( VTXApp::get().getScene() );

			glGenTextures( 1, &_texture );
			glBindTexture( GL_TEXTURE_2D, _texture );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );

			VTX_INFO( "Ray tracer initialized" );
		}

		void RayTracer::renderFrame( const Object3D::Scene & p_scene )
		{
			const CameraRayTracing camera( p_scene.getCamera(), _width, _height );

			VTX_DEBUG( "Render frame" );

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

			VTX_DEBUG( "Nb threads: " + std::to_string( nbThreads ) );

			// start rendering
			//_progressBar.start( nbTiles, 50 );
			Tool::Chrono chrono;

			chrono.start();

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

			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _pixels.data() );

			chrono.stop();
			//_progressBar.stop();

			const double time = chrono.elapsedTime();

			VTX_DEBUG( "Rendering time: " + std::to_string( time * 1000. ) + "ms" );
		}

		void RayTracer::setShading() {}

		void RayTracer::resize( const uint p_width, const uint p_height ) { BaseRenderer::resize( p_width, p_height ); }

		void RayTracer::_initScene( const Object3D::Scene & p_scene )
		{
			_scene.clean();
			//_scene.addObject( new TriangleMesh( DATA_DIR + "Bunny.obj" ) );

//#define RIBBON_TEST
#ifdef RIBBON_TEST
			for ( std::pair<const Model::Molecule *, float> pairMol : p_scene.getMolecules() )
			{
				_scene.addObject( new TriangleMesh( pairMol.first ) );
			}
#else
			for ( std::pair<const Model::Molecule *, float> pairMol : p_scene.getMolecules() )
			{
				_scene.addObject( new MoleculeRT( pairMol.first ) );
			}
#endif

			//
			//
			// ====================================================
			// Spike_ACE2.pdb
			//_scene.addObject( new Plane( Vec3f( -0.004926f, -0.050945f, -0.998636f ),
			//							 -110.f, //
			//							 new MatteMaterial( Color::Rgb( 0.5f, 0.6f, 0.8f ), 12.f ) ) );
			//_scene.addLight( new QuadLight( //
			//	Vec3f( -0.495502f, -0.856499f, -3.2335f ) + Vec3f( -280.f, 300.f, -300.f ),
			//	-VEC3F_X * 80.f,
			//	-VEC3F_Y * 80.f,
			//	Color::Rgb::WHITE,
			//	80.f * PIf ) );
			//_scene.addLight( new QuadLight( //
			//	Vec3f( -0.495502f, -0.856499f, -3.2335f ) + Vec3f( 200.f, 300.f, -300.f ),
			//	VEC3F_X * 80.f,
			//	VEC3F_Y * 80.f,
			//	Color::Rgb::WHITE,
			//	80.f * PIf ) );

			// ====================================================
			//
			//

			//
			//
			// ====================================================
			// Molecule of the month may 20 : 3jb9
			//_scene.addObject( new Plane( Vec3f( -0.058959f, 0.117106f, 0.991368f ),
			//							 -400.f, //
			//							 new MatteMaterial( Color::Rgb( 0.5f, 0.6f, 0.8f ), 12.f ) ) );
			//_scene.addLight( new QuadLight( //
			//	Vec3f( 400.f, 800.f, 550.f ),
			//	VEC3F_Y * 80.f,
			//	VEC3F_X * 80.f,
			//	Color::Rgb::WHITE,
			//	70.f * PIf ) );
			//_scene.addLight( new QuadLight( //
			//	Vec3f( 800.f, 120.f, 550.f ),
			//	VEC3F_Y * 80.f,
			//	VEC3F_X * 80.f,
			//	Color::Rgb::WHITE,
			//	70.f * PIf ) );
			// ====================================================
			//
			//

			//
			//
			// ====================================================
			// spike_closed_cleaved_full_amarolab
			//_scene.addObject( new Plane( Vec3f( 0.f, 0.f, 0.999207f ),
			//							 -300.f, //
			//							 new MatteMaterial( Vec3f( 0.5f, 0.6f, 0.8f ), 0.5f ) ) );
			//_scene.addLight( new QuadLight(
			//	Vec3f( -400.f, -800.f, 600.f ), VEC3F_Y * 80.f, VEC3F_X * 80.f, Color::Rgb::WHITE, 170.f * PIf ) );
			// ====================================================
			//
			//

			//
			//
			// ====================================================
			// 6VSB
			//_scene.addObject( new Plane( Vec3f( 0.032623f, 0.014814f, 0.999251f ),
			//							 -315.f, //
			//							 new MatteMaterial( Color::Rgb( 0.5f, 0.6f, 0.8f ), 12.f ) ) );
			//_scene.addLight( new QuadLight(
			//	Vec3f( 500.f, 400.f, 450.f ), VEC3F_Y * 80.f, VEC3F_X * 80.f, Color::Rgb::WHITE, 80.f * PIf ) );
			// ====================================================
			//
			//

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

			/*	_scene.addLight(
					new QuadLight( Vec3f( 200.f, 400.f, 400.f ), VEC3F_Y * 60.f, VEC3F_X * 60.f, VEC3F_XYZ, 50.f ) );*/

			// 6VSB
			//_scene.addLight( new PointLight( Vec3f( 150.f, -200.f, 90.f ), VEC3F_XYZ, 1000000.f ) );
			/*_scene.addLight( new PointLight( Vec3f( 150.f, -200.f, 300.f ), VEC3F_XYZ, 1000000.f ) );*/
			//_scene.addLight( new PointLight( Vec3f( -450.f, -200.f, -38.f ), VEC3F_XYZ, 1000000.f ) );
			/*_scene.addLight( new QuadLight( Vec3f( -450.f, -200.f, -38.f ),
											Vec3f( 0.327533f, -0.944138f, 0.036398f ) * 80.f,
											-Vec3f( 0.112113f, 0.077086f, 0.990701f ) * 80.f,
											VEC3F_XYZ,
											200.f ) );*/
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
						Color::Rgb color = _renderPixel( p_camera, float( x ), float( y ), p_nbPixelSamples );
						color.applyGamma( _gamma );
						const uint pixelId	   = ( x + y * _width ) * 3;
						p_image[ pixelId ]	   = uchar( color.getR() * 255 );
						p_image[ pixelId + 1 ] = uchar( color.getG() * 255 );
						p_image[ pixelId + 2 ] = uchar( color.getB() * 255 );
					}
				}

				taskId = p_nextTileId++;
				//_progressBar.next();
			}
		}

		Color::Rgb RayTracer::_renderPixel( const CameraRayTracing & p_camera,
											const float				 p_x,
											const float				 p_y,
											const uint				 p_nbPixelSamples )
		{
			Color::Rgb color = Color::Rgb::BLACK;

			// sampling ray within a pixel for anti-aliasing
			for ( uint s = 0; s < p_nbPixelSamples; s++ )
			{
				// first sample in pixel center
				const float sx = s == 0 ? p_x : p_x + Util::Math::randomFloat();
				const float sy = s == 0 ? p_y : p_y + Util::Math::randomFloat();

				const Ray ray = p_camera.generateRay( sx, sy );

				const Color::Rgb Li = _integrator->Li( ray, _scene, 0.f, FLOAT_INF );
				// const Color::Rgb ao = _aoIntegrator->Li( ray, _scene, 0.f, FLOAT_INF );

				const float directFactor = 0.4f;
				const float aoFactor	 = 1.f - directFactor;

				// color += Li * directFactor + ao * aoFactor;

				color += Li;
			}
			color /= float( p_nbPixelSamples );

			color.saturate();
			return color;
		}

	} // namespace Renderer
} // namespace VTX
