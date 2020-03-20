#include "ray_tracer.hpp"
#include "ray_tracing/materials/flat_color_material.hpp"
#include "ray_tracing/primitives/cylinder.hpp"
#include "ray_tracing/primitives/sphere.hpp"
#include "tool/chrono.hpp"
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
				_pos( p_camera.getPosition() ), _width( p_width ), _height( p_height )
			{
				const Vec3f & camFront = p_camera.getFront();
				const Vec3f & camLeft  = p_camera.getLeft();
				const Vec3f & camUp	   = p_camera.getUp();
				const float	  camFov   = p_camera.getFov();

				const float ratio	   = float( _width ) / _height;
				const float halfHeight = tan( glm::radians( camFov ) * 0.5f );
				const float halfWidth  = ratio * halfHeight;
				_downLeftCorner		   = _pos + halfWidth * camLeft - halfHeight * camUp + camFront;
				_du					   = -2.f * halfWidth * camLeft;
				_dv					   = 2.f * halfHeight * camUp;
			}

			Ray generateRay( const float p_sx, const float p_sy ) const
			{
				return Ray( _pos,
							glm::normalize( _downLeftCorner + ( p_sx / _width ) * _du
											+ ( ( _height - p_sy ) / _height ) * _dv - _pos ) );
			}

		  private:
			Vec3f _pos;
			uint  _width;
			uint  _height;

			Vec3f _downLeftCorner;
			Vec3f _du;
			Vec3f _dv;
		};

		const uint RayTracer::TILE_SIZE = 64;

		void RayTracer::init( const uint p_width, const uint p_height )
		{
			VTX_INFO( "Initializing ray tracer (only first molecule)..." );

			BaseRenderer::resize( p_width, p_height );

			const Model::Molecule * mol = VTXApp::get().getScene().getMolecules()[ 0 ];

			// TODO: add options for splitMethod and maxPrimsLeaf
			const uint maxPrimsLeaf = 8;
			_bvh.build( mol->getRTPrimitives(), maxPrimsLeaf, BVH::SplitMethod::HLBVH );

			VTX_INFO( "Ray tracer initialized" );
		}

		void RayTracer::renderFrame( const Object3D::Scene & p_scene )
		{
			VTX_INFO( "Render Frame" );
			const CameraRayTracing camera( p_scene.getCamera(), _width, _height );

			const uint nbPixelSamples = 32;

			uint			   size = _width * _height * 3 * sizeof( char );
			std::vector<uchar> pixels( _width * _height * 3 );

			// init data for tiled rendering
			const uint nbTilesX = ( _width + TILE_SIZE - 1 ) / TILE_SIZE;
			const uint nbTilesY = ( _height + TILE_SIZE - 1 ) / TILE_SIZE;
			const uint nbTiles	= nbTilesX * nbTilesY;

			// init parallel
			const uint				 nbThreads = std::thread::hardware_concurrency();
			std::vector<std::thread> threadPool;
			threadPool.reserve( nbThreads );

			VTX_INFO( "Nb threads: " + std::to_string( nbThreads ) );

			// start rendering
			Tool::Chrono chrono;

			chrono.start();

			std::atomic<uint> nextTile = nbThreads;
			for ( uint i = 0; i < nbThreads; ++i )
			{
				threadPool.emplace_back( std::thread(
					[ this, &pixels, &camera, nbPixelSamples, i, nbTilesX, nbTilesY, nbTiles, &nextTile ]() {
						_renderTiles( pixels, camera, nbPixelSamples, i, nbTilesX, nbTilesY, nbTiles, nextTile );
					} ) );
			}
			for ( std::thread & t : threadPool )
			{
				t.join();
			}

			//#pragma omp parallel for
			//			for ( int i = 0; i < int( nbTiles ); ++i )
			//			{
			//				_renderTile( pixels, camera, nbPixelSamples, uint( i ), nbTilesX, nbTilesY );
			//			}

			chrono.stop();

			const double time = chrono.elapsedTime();

			VTX_INFO( "Rendering time: " + std::to_string( time ) );
			VTX_INFO( "Save image as: test RT.png" );

			stbi_write_png( "test RT.png", _width, _height, 3, pixels.data(), 0 );
		}

		void RayTracer::setShading() {}

		void RayTracer::resize( const uint, const uint ) { BaseRenderer::resize( _width, _height ); }

		void RayTracer::_renderTiles( std::vector<uchar> &	   p_image,
									  const CameraRayTracing & p_camera,
									  const uint			   p_nbPixelSamples,
									  const uint			   p_threadId,
									  const uint			   p_nbTilesX,
									  const uint			   p_nbTilesY,
									  const uint			   p_nbTiles,
									  std::atomic<uint> &	   p_nextTile )
		{
			uint taskIndex = p_threadId;

			while ( taskIndex < p_nbTiles )
			{
				const uint tileY = taskIndex / p_nbTilesX;
				const uint tileX = taskIndex - tileY * p_nbTilesX;
				const uint x0	 = tileX * TILE_SIZE;
				const uint x1	 = Util::Math::min( x0 + TILE_SIZE, _width );
				const uint y0	 = tileY * TILE_SIZE;
				const uint y1	 = Util::Math::min( y0 + TILE_SIZE, _height );

				for ( uint y = y0; y < y1; ++y )
				{
					for ( uint x = x0; x < x1; ++x )
					{
						const Vec3f color	   = _renderPixel( p_camera, float( x ), float( y ), p_nbPixelSamples );
						const uint	pixelId	   = ( x + y * _width ) * 3;
						p_image[ pixelId ]	   = uchar( color.r * 255 );
						p_image[ pixelId + 1 ] = uchar( color.g * 255 );
						p_image[ pixelId + 2 ] = uchar( color.b * 255 );
					}
				}
				taskIndex = p_nextTile++;
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
					const Vec3f color	   = _renderPixel( p_camera, float( x ), float( y ), p_nbPixelSamples );
					const uint	pixelId	   = ( x + y * _width ) * 3;
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

			// TODO: make a camera for ray tracing

			// sampling ray within a pixel for anti-aliasing
			for ( uint s = 0; s < p_nbPixelSamples; s++ )
			{
				// first sample in pixel center
				const float sx = s == 0 ? p_x : p_x + Util::Math::randomFloat();
				const float sy = s == 0 ? p_y : p_y + Util::Math::randomFloat();

				const Ray ray = p_camera.generateRay( sx, sy );

				BasePrimitive * primHit = nullptr;

				float		 tMin = 1e-3f;
				float		 tMax = FLT_MAX;
				Intersection intersection;
				if ( _bvh.intersect( ray, tMin, tMax, intersection ) )
				{
					// shade primitive
					// point light on camera
					color += intersection._primitive->getMaterial()->shade( ray, intersection, -ray.getDirection() );
				}
				else
				{
					color += _backgroundColor;
				}
			}
			return glm::clamp( color / float( p_nbPixelSamples ), VEC3F_ZERO, VEC3F_XYZ );
		}

	} // namespace Renderer
} // namespace VTX
