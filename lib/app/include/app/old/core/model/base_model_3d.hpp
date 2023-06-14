#ifndef __VTX_MODEL_BASE_MODEL_3D__
#define __VTX_MODEL_BASE_MODEL_3D__

#include "app/old/component/generic/base_auto_rotate.hpp"
#include "app/old/component/generic/base_renderable.hpp"
#include "app/old/component/generic/base_transformable.hpp"
#include "app/old/component/generic/base_visible.hpp"
#include "app/old/component/object3d/helper/aabb.hpp"
#include "app/old/core/event/vtx_event.hpp"
#include "app/old/core/model/base_model.hpp"
#include "app/old/id.hpp"
#include "app/old/render/buffer/base_buffer_opengl.hpp"
#include <unordered_set>
#include <util/logger.hpp>
#include <util/math.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Old::Core::Model
{
	template<typename B, typename = std::enable_if<std::is_base_of<Render::Buffer::BaseBufferOpenGL, B>::value>>
	class BaseModel3D :
		public App::Old::Core::Model::BaseModel,
		public Component::Generic::BaseTransformable,
		public Component::Generic::BaseRenderable,
		public Component::Generic::BaseVisible,
		public Component::Generic::BaseAutoRotate
	{
		VTX_MODEL

	  public:
		inline virtual App::Old::Component::Object3D::Helper::AABB & getAABB() const
		{
			if ( !_aabb.isValid() )
				_computeAABB();

			return _aabb;
		}
		inline virtual App::Old::Component::Object3D::Helper::AABB & getWorldAABB() const
		{
			if ( !_worldAabb.isValid() )
				_computeWorldAABB();

			return _worldAabb;
		}
		inline const B * const getBuffer() const { return _buffer; }
		inline B * const	   getBuffer() { return _buffer; }
		inline bool			   isInit() const { return _isInit; }

		inline void referenceLinkedAABB( App::Old::Component::Object3D::Helper::AABB * const p_aabb )
		{
			_linkedAABBs.emplace( p_aabb );
		}

		void setAutoRotationVector( const Vec3f p_value ) override
		{
			Component::Generic::BaseAutoRotate::setAutoRotationVector( p_value );
			_notifyViews( App::Old::Event::Model::AUTO_ROTATE_DATA_CHANGE );
		}
		void setAutoRotationNormalizedVector( const Vec3f p_value ) override
		{
			Component::Generic::BaseAutoRotate::setAutoRotationNormalizedVector( p_value );
			_notifyViews( App::Old::Event::Model::AUTO_ROTATE_DATA_CHANGE );
		}
		void setAutoRotationMagnitude( const float p_speed ) override
		{
			Component::Generic::BaseAutoRotate::setAutoRotationMagnitude( p_speed );
			_notifyViews( App::Old::Event::Model::AUTO_ROTATE_DATA_CHANGE );
		}
		void setAutoRotationPlaying( const bool p_play ) override
		{
			Component::Generic::BaseAutoRotate::setAutoRotationPlaying( p_play );
			_notifyViews( App::Old::Event::Model::AUTO_ROTATE_DATA_CHANGE );
		}

		virtual void render( const App::Old::Component::Render::Camera & p_camera ) const override
		{
			if ( _buffer == nullptr )
				return;

			_buffer->bind();
			for ( App::Old::Component::Generic::BaseRenderable * const renderable : _renderables )
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
			_instantiate3DViews();

			_isInit = true;
			_notifyViews( App::Old::Event::Model::INIT );
		}

	  protected:
		mutable App::Old::Component::Object3D::Helper::AABB		   _aabb;
		mutable App::Old::Component::Object3D::Helper::AABB		   _worldAabb;
		std::vector<App::Old::Component::Generic::BaseRenderable *> _renderables
			= std::vector<App::Old::Component::Generic::BaseRenderable *>();
		B *	 _buffer = nullptr;
		bool _isInit = false;

		std::unordered_set<App::Old::Component::Object3D::Helper::AABB *> _linkedAABBs
			= std::unordered_set<App::Old::Component::Object3D::Helper::AABB *>();

		BaseModel3D( const VTX::App::Old::VTX_ID & p_typeId ) : BaseModel( p_typeId ) {}
		virtual ~BaseModel3D()
		{
			_linkedAABBs.clear();

			if ( _buffer != nullptr )
			{
				_buffer->makeContextCurrent();
				delete _buffer;
				_buffer->doneContextCurrent();
			}
		}

		virtual void _transformModifiedEvent() override
		{
			_invalidateWorldAABB();
			_notifyViews( App::Old::Event::Model::TRANSFORM_CHANGE );
		};

		void _invalidateAABB()
		{
			_aabb.invalidate();
			_invalidateWorldAABB();
		};

		void _invalidateWorldAABB()
		{
			_worldAabb.invalidate();

			for ( App::Old::Component::Object3D::Helper::AABB * const aabb : _linkedAABBs )
				aabb->invalidate();
		};

		virtual void _init() {}
		virtual void _computeAABB() const {}
		virtual void _computeWorldAABB() const
		{
			std::vector<Vec3f> aabbSummits = getAABB().getSummits();

			_worldAabb = App::Old::Component::Object3D::Helper::AABB();
			for ( const Vec3f & summit : aabbSummits )
			{
				const Vec4f worldSummit = _transform.get() * Vec4f( summit, 1 );
				_worldAabb.extend( worldSummit );
			}
		};
		virtual void _instantiate3DViews() = 0;
		inline void	 _addRenderable( App::Old::Component::Generic::BaseRenderable * const p_renderable )
		{
			p_renderable->init();
			_renderables.push_back( p_renderable );
		}
	};
} // namespace VTX::App::Old::Core::Model
#endif
