#ifndef __VTX_BASE_MODEL_3D__
#define __VTX_BASE_MODEL_3D__

#ifdef _MSC_VER
#pragma once
#endif

#include "buffer/base_buffer_opengl.hpp"
#include "event/event.hpp"
#include "generic/base_renderable.hpp"
#include "generic/base_transformable.hpp"
#include "generic/base_visible.hpp"
#include "id.hpp"
#include "math/aabb.hpp"
#include "model/base_model.hpp"
#include "tool/logger.hpp"
#include <unordered_set>
#include <vector>

namespace VTX
{
	namespace Model
	{
		template<typename B, typename = std::enable_if<std::is_base_of<Buffer::BaseBufferOpenGL, B>::value>>
		class BaseModel3D :
			public BaseModel,
			public Generic::BaseTransformable,
			public Generic::BaseRenderable,
			public Generic::BaseVisible
		{
			VTX_MODEL

		  public:
			inline virtual const Math::AABB & getAABB() const
			{
				if ( !_aabb.isValid() )
					_computeAABB();
				return _aabb;
			}
			inline virtual const Math::AABB & getWorldAABB() const
			{
				if ( !_worldAabb.isValid() )
				{
					_worldAabb = _aabb;
					_worldAabb.translate( getTransform().getTranslationVector() );
				}

				return _worldAabb;
			}
			inline const B * const			  getBuffer() const { return _buffer; }
			inline B * const				  getBuffer() { return _buffer; }
			inline const std::vector<Vec3f> & getBufferAABBCorners() const { return _bufferAABBCorners; }
			inline const std::vector<uint> &  getBufferAABBIndices() const { return _bufferAABBIndices; }
			inline bool						  isInit() const { return _isInit; }

			inline void referenceLinkedAABB( Math::AABB * const p_aabb ) { _linkedAABBs.emplace( p_aabb ); }

			void render( const Object3D::Camera & p_camera ) const override
			{
				_buffer->bind();
				for ( Generic::BaseRenderable * const renderable : _renderables )
				{
					renderable->render( p_camera );
				}
				_buffer->unbind();

				if ( _viewBox != nullptr )
				{
					_buffer->bindAABB();
					_viewBox->render( p_camera );
					_buffer->unbindAABB();
				}
			}
			void init( OpenGLFunctions * const p_gl )
			{
				_buffer = new B( p_gl );
				_buffer->generate();

				_init();

				_computeAABB();
				_fillBufferAABB();

				_fillBuffer();
				_instantiate3DViews();

				_isInit = true;
				_notifyViews( new Event::VTXEvent( Event::Model::INIT ) );
			}

		  protected:
			mutable Math::AABB					   _aabb;
			mutable Math::AABB					   _worldAabb;
			std::vector<Generic::BaseRenderable *> _renderables		  = std::vector<Generic::BaseRenderable *>();
			B *									   _buffer			  = nullptr;
			bool								   _isInit			  = false;
			std::vector<Vec3f>					   _bufferAABBCorners = std::vector<Vec3f>();
			std::vector<uint>					   _bufferAABBIndices = std::vector<uint>();
			Generic::BaseRenderable *			   _viewBox			  = nullptr;

			std::unordered_set<Math::AABB *> _linkedAABBs = std::unordered_set<Math::AABB *>();

			BaseModel3D( const ID::VTX_ID & p_typeId ) : BaseModel( p_typeId ) {}
			virtual ~BaseModel3D()
			{
				_linkedAABBs.clear();

				if ( _buffer != nullptr )
				{
					delete _buffer;
				}
			}

			virtual void _transformModifiedEvent() override
			{
				_invalidateWorldAABB();
				_notifyViews( new VTX::Event::VTXEvent( Event::Model::TRANSFORM_CHANGE ) );
			};
			void _invalidateWorldAABB()
			{
				_worldAabb.invalidate();

				for ( Math::AABB * const aabb : _linkedAABBs )
					aabb->invalidate();
			};

			virtual void _init() {};
			virtual void _fillBuffer()		   = 0;
			virtual void _computeAABB() const  = 0;
			virtual void _instantiate3DViews() = 0;
			inline void	 _addRenderable( Generic::BaseRenderable * const p_renderable )
			{
				p_renderable->init();
				_renderables.push_back( p_renderable );
			}

			virtual void _fillBufferAABB()
			{
				const Vec3f & min = _aabb.getMin();
				const Vec3f & max = _aabb.getMax();

				_bufferAABBCorners = std::vector<Vec3f>( { min,
														   Vec3f( max.x, min.y, min.z ),
														   Vec3f( max.x, max.y, min.z ),
														   Vec3f( min.x, max.y, min.z ),
														   Vec3f( min.x, min.y, max.z ),
														   Vec3f( max.x, min.y, max.z ),
														   max,
														   Vec3f( min.x, max.y, max.z ) } );

				_bufferAABBIndices
					= std::vector<uint>( { 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 } );

				_buffer->setAABBCorners( _bufferAABBCorners );
				_buffer->setAABBIndices( _bufferAABBIndices );
			}
		};
	} // namespace Model
} // namespace VTX
#endif
