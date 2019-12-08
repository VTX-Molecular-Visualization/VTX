#include "base_view_3d_molecule.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace VTX
{
	namespace View
	{
		void BaseView3DMolecule::setCameraMatrices( const Object3D::Camera::BaseCamera & p_camera ) const
		{
			glUniformMatrix4fv( _uViewMatrix, 1, GL_FALSE, glm::value_ptr( p_camera.getViewMatrix() ) );
			glUniformMatrix4fv( _uProjMatrix, 1, GL_FALSE, glm::value_ptr( p_camera.getProjectionMatrix() ) );
			// TODO: set radius as setting.
			glUniform1f( _uCylinderRadius, 0.2f );
		}
	} // namespace View
} // namespace VTX
