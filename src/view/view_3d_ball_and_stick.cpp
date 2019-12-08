#include "view_3d_ball_and_stick.hpp"

namespace VTX
{
	namespace View
	{
		std::string View3DBallAndStick::getProgramName() const { return "CylinderGeom"; }

		std::vector<std::string> View3DBallAndStick::getShaderNames() const
		{
			return { "cylinderImpostorGeom.vert", "cylinderImpostorGeom.geom", "cylinderImpostorDeferred.frag" };
		}

		void View3DBallAndStick::setupUniforms( const GLint p_programId )
		{
			_uViewMatrix	 = glGetUniformLocation( p_programId, "uMVMatrix" );
			_uProjMatrix	 = glGetUniformLocation( p_programId, "uProjMatrix" );
			_uCylinderRadius = glGetUniformLocation( p_programId, "uCylRad" );
		}

		void View3DBallAndStick::render( const uint p_time )
		{
			glDrawElements( GL_LINES, 0, GL_UNSIGNED_INT, (void *)( _model->getAtomCount() * sizeof( uint ) ) );
			// glDrawArrays( GL_POINTS, 0, _model->getAtomCount() );
		}

	} // namespace View
} // namespace VTX
