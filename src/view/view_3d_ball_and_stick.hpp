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

			virtual std::string getNameStr() const override
			{
				return std::string( magic_enum::enum_name( REPRESENTATION::BALL_AND_STICK ) );
			};
			virtual void render( const uint p_time ) override;
			virtual void notify( Event::EVENT_MODEL ) {};

			virtual std::string				 getProgramName() const override;
			virtual std::vector<std::string> getShaderNames() const override;
			virtual void					 setupUniforms( const GLint p_programId ) override;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
