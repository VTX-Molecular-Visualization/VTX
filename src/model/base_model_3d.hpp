#ifndef __VTX_BASE_MODEL_3D__
#define __VTX_BASE_MODEL_3D__

#ifdef _MSC_VER
#pragma once
#endif

#include "buffer/base_buffer_opengl.hpp"
#include "generic/base_renderable.hpp"
#include "generic/base_transformable.hpp"
#include "generic/base_visible.hpp"
#include "id.hpp"
#include "math/aabb.hpp"
#include "model/base_model.hpp"

namespace VTX
{
	namespace Model
	{
		template<typename B, typename = std::enable_if<std::is_base_of<Buffer::BaseBufferOpenGL, B>::value>>
		class BaseModel3D : public BaseModel, public Generic::BaseTransformable, public Generic::BaseRenderable, public Generic::BaseVisible
		{
		  public:
			BaseModel3D( const ID::VTX_ID & p_typeId ) : BaseModel( p_typeId ) {};
			virtual ~BaseModel3D()
			{
				if ( _buffer != nullptr )
				{
					_buffer->free();
					delete _buffer;
				}
			}

			inline const Math::AABB & getAABB() const { return _aabb; }
			inline const B * const	  getBuffer() const { return _buffer; }
			inline B * const		  getBuffer() { return _buffer; }
			inline bool				  isInit() const { return _isInit; };

			virtual void render()
			{
				_buffer->bind();
				_notifyViews( new Event::VTXEvent( Event::Model::RENDER ) );
				_buffer->unbind();
			}

			void init( OpenGLFunctions * const p_gl )
			{
				_buffer = new B( p_gl );
				_buffer->generate();
				_init();
				_instanciate3DViews();

				_isInit = true;
				_notifyViews( new Event::VTXEvent( Event::Model::INIT ) );
			}

		  protected:
			Math::AABB _aabb;
			B *		   _buffer = nullptr;
			bool	   _isInit = false;

			virtual void _init()			   = 0;
			virtual void _instanciate3DViews() = 0;
		};
	} // namespace Model
} // namespace VTX
#endif
