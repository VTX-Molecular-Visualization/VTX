#include "component_selection.hpp"
#include "../defines.hpp"
#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "../style.hpp"
#include "../view/view_ui_atom.hpp"
#include "../view/view_ui_chain.hpp"
#include "../view/view_ui_residue.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentSelection::ComponentSelection( bool * const p_show ) : BaseComponent( p_show )
		{
			_addComponent( std::make_shared<View::ViewUIChain>( View::ViewUIChain( _show ) ) );
			_addComponent( std::make_shared<View::ViewUIResidue>( View::ViewUIResidue( _show ) ) );
			_addComponent( std::make_shared<View::ViewUIAtom>( View::ViewUIAtom( _show ) ) );
		}

		void ComponentSelection::_addComponents() {}

		void ComponentSelection::_draw()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Selection.Selection" ), _show, flags ) == false )
			{
				ImGui::End();
				return;
			}

			_drawComponents();

			ImGui::End();
		}

	} // namespace UI
} // namespace VTX
