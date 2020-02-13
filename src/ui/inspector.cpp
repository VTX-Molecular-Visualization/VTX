#include "inspector.hpp"
#include "define.hpp"
#include "localization/language.hpp"
#include "style.hpp"
#include "view/ui/atom.hpp"
#include "view/ui/chain.hpp"
#include "view/ui/molecule.hpp"
#include "view/ui/residue.hpp"

namespace VTX
{
	namespace UI
	{
		void Inspector::_draw()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Inspector.Inspector" ), isVisiblePtr(), flags ) == false )
			{
				ImGui::End();
				return;
			}

			_drawComponent( ID::View::UI_MOLECULE );
			_drawComponent( ID::View::UI_CHAIN );
			_drawComponent( ID::View::UI_RESIDUE );
			_drawComponent( ID::View::UI_ATOM );
			ImGui::Separator();
			_drawComponent( ID::View::UI_PATH );
			_drawComponent( ID::View::UI_VIEWPOINT );

			ImGui::End();
		}

	} // namespace UI
} // namespace VTX
