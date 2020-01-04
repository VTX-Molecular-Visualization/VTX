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

			if ( ImGui::CollapsingHeader( "Transform", flags ) )
			{
				float *x, *y, *z, *w;

				ImGui::Text( "Translation" );
				const Mat4f & translation = _model->getTransform().getTranslation();
				x						  = (float *)&translation[ 0 ];
				y						  = (float *)&translation[ 1 ];
				z						  = (float *)&translation[ 2 ];
				w						  = (float *)&translation[ 3 ];
				ImGui::InputFloat4( "X", x, 2 );
				ImGui::InputFloat4( "Y", y, 2 );
				ImGui::InputFloat4( "Z", z, 2 );
				ImGui::InputFloat4( "W", w, 2 );

				ImGui::Text( "Rotation" );
				const Mat4f & rotation = _model->getTransform().getRotation();
				x					   = (float *)&rotation[ 0 ];
				y					   = (float *)&rotation[ 1 ];
				z					   = (float *)&rotation[ 2 ];
				w					   = (float *)&rotation[ 3 ];
				ImGui::InputFloat4( "X", x, 2 );
				ImGui::InputFloat4( "Y", y, 2 );
				ImGui::InputFloat4( "Z", z, 2 );
				ImGui::InputFloat4( "W", w, 2 );

				ImGui::Text( "Scale" );
				const Mat4f & scale = _model->getTransform().getScale();
				x					= (float *)&scale[ 0 ];
				y					= (float *)&scale[ 1 ];
				z					= (float *)&scale[ 2 ];
				w					= (float *)&scale[ 3 ];
				ImGui::InputFloat4( "X", x, 2 );
				ImGui::InputFloat4( "Y", y, 2 );
				ImGui::InputFloat4( "Z", z, 2 );
				ImGui::InputFloat4( "W", w, 2 );
			}

			ImGui::PopID();
		}
	} // namespace View
} // namespace VTX
