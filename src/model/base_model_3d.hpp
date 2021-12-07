#ifndef __VTX_BASE_MODEL_3D__
#define __VTX_BASE_MODEL_3D__

#include "buffer/base_buffer_opengl.hpp"
#include "define.hpp"
#include "event/event.hpp"
#include "generic/base_auto_rotate.hpp"
#include "generic/base_renderable.hpp"
#include "generic/base_transformable.hpp"
#include "generic/base_visible.hpp"
#include "id.hpp"
#include "math/aabb.hpp"
#include "model/base_model.hpp"
#include "tool/logger.hpp"
#include "util/math.hpp"
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
			public Generic::BaseVisible,
			public Generic::BaseAutoRotate
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
					_computeWorldAABB();

				return _worldAabb;
			}
			inline const B * const getBuffer() const { return _buffer; }
			inline B * const	   getBuffer() { return _buffer; }
			inline bool			   isInit() const { return _isInit; }

			inline void referenceLinkedAABB( Math::AABB * const p_aabb ) { _linkedAABBs.emplace( p_aabb ); }

			void setAutoRotationVector( const Vec3f p_value ) override
			{
				Generic::BaseAutoRotate::setAutoRotationVector( p_value );
				_notifyViews( new Event::VTXEvent( Event::Model::AUTO_ROTATE_DATA_CHANGE ) );
			}
			void setAutoRotationNormalizedVector( const Vec3f p_value ) override
			{
				Generic::BaseAutoRotate::setAutoRotationNormalizedVector( p_value );
				_notifyViews( new Event::VTXEvent( Event::Model::AUTO_ROTATE_DATA_CHANGE ) );
			}
			void setAutoRotationMagnitude( const float p_speed ) override
			{
				Generic::BaseAutoRotate::setAutoRotationMagnitude( p_speed );
				_notifyViews( new Event::VTXEvent( Event::Model::AUTO_ROTATE_DATA_CHANGE ) );
			}
			void setAutoRotationPlaying( const bool p_play ) override
			{
				Generic::BaseAutoRotate::setAutoRotationPlaying( p_play );
				_notifyViews( new Event::VTXEvent( Event::Model::AUTO_ROTATE_DATA_CHANGE ) );
			}

			void render( const Object3D::Camera & p_camera ) const override
			{
				if ( _buffer == nullptr )
					return;

				_buffer->bind();
				for ( Generic::BaseRenderable * const renderable : _renderables )
				{
					renderable->render( p_camera );
				}
				_buffer->unbind();
			}
			void init() override
			{
				_buffer = new B();
				_buffer->generate();

				_init();
				_fillBuffer();
				_instantiate3DViews();

				_isInit = true;
				_notifyViews( new Event::VTXEvent( Event::Model::INIT ) );
			}

		  protected:
			mutable Math::AABB					   _aabb;
			mutable Math::AABB					   _worldAabb;
			std::vector<Generic::BaseRenderable *> _renderables = std::vector<Generic::BaseRenderable *>();
			B *									   _buffer		= nullptr;
			bool								   _isInit		= false;

			std::unordered_set<Math::AABB *> _linkedAABBs = std::unordered_set<Math::AABB *>();

			BaseModel3D( const VTX::ID::VTX_ID & p_typeId ) : BaseModel( p_typeId ) {}
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
			virtual void _fillBuffer()		  = 0;
			virtual void _computeAABB() const = 0;
			virtual void _computeWorldAABB() const
			{
				const Vec4f worldAABBPosition
					= _transform.get() * Vec4f( _aabb.centroid().x, _aabb.centroid().y, _aabb.centroid().z, 1 );

				_worldAabb = Math::AABB( Vec3f( worldAABBPosition.x, worldAABBPosition.y, worldAABBPosition.z ),
										 _aabb.radius() );
			};
			virtual void _instantiate3DViews() = 0;
			inline void	 _addRenderable( Generic::BaseRenderable * const p_renderable )
			{
				p_renderable->init();
				_renderables.push_back( p_renderable );
			}
		};
	} // namespace Model
} // namespace VTX
#endif
