#ifndef __VTX_VIEW_3D_BALL_AND_STICK__
#define __VTX_VIEW_3D_BALL_AND_STICK__

#ifdef _MSC_VER
#pragma once
#endif

#include "../shader/glsl_program_manager.hpp"
#include "base_view_3d_molecule.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace VTX
{
	namespace View
	{
		class View3DBallAndStick : public BaseView3DMolecule
		{
		  public:
			View3DBallAndStick()  = default;
			~View3DBallAndStick() = default;

			virtual std::string getNameStr() const override
			{
				return std::string( magic_enum::enum_name( REPRESENTATION::BALL_AND_STICK ) );
			};
			virtual void render( const uint p_time ) override {};
			virtual void notify( Event::EVENT_MODEL ) {};

		  private:
			// TODO: move in BaseView3D or BaseView3DMolecule?
			GLint _uViewMatrix	   = GL_INVALID_INDEX;
			GLint _uProjMatrix	   = GL_INVALID_INDEX;
			GLint _uCylinderRadius = GL_INVALID_INDEX;
		};
	} // namespace View
} // namespace VTX
#endif
