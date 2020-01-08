#include "view_ui_molecule.hpp"
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
				float *x, *y, *z;

				ImGui::Text( "Scale" );
				const Mat4f & scale = _model->getTransform().getScale();
				x					= (float *)&scale[ 0 ][ 0 ];
				y					= (float *)&scale[ 1 ][ 1 ];
				z					= (float *)&scale[ 2 ][ 2 ];
				ImGui::PushID( "Scale" );
				ImGui::InputFloat( "X", x, 2 );
				ImGui::InputFloat( "Y", y, 2 );
				ImGui::InputFloat( "Z", z, 2 );
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
