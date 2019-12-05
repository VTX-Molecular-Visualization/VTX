#ifndef __VTX_VIEW_3D_BALL_AND_STICK__
#define __VTX_VIEW_3D_BALL_AND_STICK__

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

			virtual void render( const uint p_time ) override {};
			virtual void notify( Event::EVENT_MODEL ) {};

		  private:
			// TODO: move in BaseView3D?
			GLint _uViewMatrix	   = GL_INVALID_INDEX;
			GLint _uProjMatrix	   = GL_INVALID_INDEX;
			GLint _uCylinderRadius = GL_INVALID_INDEX;
		};
	} // namespace View
} // namespace VTX
#endif
