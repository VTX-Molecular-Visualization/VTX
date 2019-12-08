#ifndef __VTX_VIEW_3D_BALL_AND_STICK__
#define __VTX_VIEW_3D_BALL_AND_STICK__

#ifdef _MSC_VER
#pragma once
#endif

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

			void setCameraMatrices( const Mat4f, const Mat4f ) const;

			virtual std::string getNameStr() const override
			{
				return std::string( magic_enum::enum_name( REPRESENTATION::BALL_AND_STICK ) );
			};
			virtual void render( const uint p_time ) override;
			virtual void setupShaders( Shader::GLSLProgramManager & ) override;
			virtual void useShaders( Shader::GLSLProgramManager & ) override;

			virtual void notify( Event::EVENT_MODEL ) {};

		  private:
			GLint _uViewMatrix		= GL_INVALID_INDEX;
			GLint _uProjMatrix		= GL_INVALID_INDEX;
			GLint _uViewMatrix2		= GL_INVALID_INDEX;
			GLint _uProjMatrix2		= GL_INVALID_INDEX;
			GLint _uBallRadiusScale = GL_INVALID_INDEX;
			GLint _uCylinderRadius	= GL_INVALID_INDEX;
		};
	} // namespace View
} // namespace VTX
#endif
