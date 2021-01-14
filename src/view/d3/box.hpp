#ifndef __VTX_VIEW_3D_MOLECULE_BOX__
#define __VTX_VIEW_3D_MOLECULE_BOX__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "view/base_view_3d.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			class Box : public BaseView3D<Model::Molecule>
			{
				VTX_VIEW
			  public:
				explicit Box( Model::Molecule * const p_model ) : BaseView3D( p_model )
				{
					Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();

					_program = pm.createProgram( "LineShader", { "line.vert", "line.frag" } );

					assert( _program != nullptr );
					_uModelViewMatrixLoc = _gl()->glGetUniformLocation( _program->getId(), "uMVMatrix" );
					_uProjMatrixLoc		 = _gl()->glGetUniformLocation( _program->getId(), "uProjMatrix" );

					_gl()->glLineWidth( 1.0 );
				}

				void render() override
				{
					_program->use();

					const Object3D::Camera & cam = VTXApp::get().getScene().getCamera();
					_gl()->glUniformMatrix4fv( _uModelViewMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getViewMatrix() * _model->getTransform().get() ) );
					_gl()->glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );

					_gl()->glDrawElements( GL_LINES, uint( _model->getBufferAABBIndices().size() ), GL_UNSIGNED_INT, 0 );
				}

			  private:
				GLint _uModelViewMatrixLoc = GL_INVALID_INDEX;
				GLint _uProjMatrixLoc	   = GL_INVALID_INDEX;
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif;
