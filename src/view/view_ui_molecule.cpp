#include "view_ui_molecule.hpp"
#include "../action/action_translate.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		void ViewUIMolecule::notify( Event::EVENT_MODEL p_event ) {}

		void ViewUIMolecule::_draw()
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
			ImGui::PushID( "ViewMolecule" );

			if ( ImGui::CollapsingHeader( LOCALE( "Inspector.Transform" ), flags ) )
			{
				ImGui::Text( LOCALE( "Inspector.Transform.Position" ) );
				const Mat4f & translation = _getModel().getTransform().getTranslation();
				ImGui::PushID( "Position" );
				float vec[] = { translation[ 3 ][ 0 ], translation[ 3 ][ 1 ], translation[ 3 ][ 2 ] };
				if ( ImGui::InputFloat3( "Position", vec, 2 ) )
				{
					VTXApp::get().action(
						new Action::ActionTranslate( _getModel(), Vec3f( vec[ 0 ], vec[ 1 ], vec[ 2 ] ) ) );
				}

				ImGui::PopID();

				ImGui::Text( LOCALE( "Inspector.Transform.Scale" ) );
				const Mat4f & scale = _getModel().getTransform().getScale();
				float		  f		= scale[ 0 ][ 0 ];
				ImGui::PushID( "Scale" );
				ImGui::InputFloat( "Scale", &f, 1.f );
				ImGui::PopID();
			}

#ifdef _DEBUG
			if ( ImGui::CollapsingHeader( "Transform-debug" ) )
			{
				float *x, *y, *z;

				ImGui::PushID( "Debug" );

				ImGui::Text( "Translation" );
				const Mat4f & translation = _model->getTransform().getTranslation();
				x						  = (float *)&translation[ 0 ];
				y						  = (float *)&translation[ 1 ];
				z						  = (float *)&translation[ 2 ];
				ImGui::PushID( "Translation" );
				ImGui::InputFloat3( "X", x, 2 );
				ImGui::InputFloat3( "Y", y, 2 );
				ImGui::InputFloat3( "Z", z, 2 );
				ImGui::PopID();

				ImGui::Text( "Rotation" );
				const Mat4f & rotation = _model->getTransform().getRotation();
				x					   = (float *)&rotation[ 0 ];
				y					   = (float *)&rotation[ 1 ];
				z					   = (float *)&rotation[ 2 ];
				ImGui::PushID( "Rotation" );
				ImGui::InputFloat3( "X", x, 2 );
				ImGui::InputFloat3( "Y", y, 2 );
				ImGui::InputFloat3( "Z", z, 2 );
				ImGui::PopID();

				ImGui::Text( "Scale" );
				const Mat4f & scale = _model->getTransform().getScale();
				x					= (float *)&scale[ 0 ];
				y					= (float *)&scale[ 1 ];
				z					= (float *)&scale[ 2 ];
				ImGui::PushID( "Scale" );
				ImGui::InputFloat3( "X", x, 2 );
				ImGui::InputFloat3( "Y", y, 2 );
				ImGui::InputFloat3( "Z", z, 2 );
				ImGui::PopID();

				ImGui::PopID();
			}
			ImGui::PopID();
#endif
		}
	} // namespace View
} // namespace VTX
