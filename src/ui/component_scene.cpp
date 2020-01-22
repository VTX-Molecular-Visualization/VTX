#include "component_scene.hpp"
#include "../defines.hpp"
#include "../localization/language.hpp"
#include "../style.hpp"
#include "../view/view_ui_molecule_scene.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentScene::ComponentScene( bool * const p_show ) : BaseComponent( p_show ) {}

		void ComponentScene::_addComponents()
		{
			//_addComponent( std::make_shared<View::ViewUIMoleculeScene>( View::ViewUIMoleculeScene( _show ) ) );
		}

		void ComponentScene::_draw()
		{
			ImGuiWindowFlags flagsWindow = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Scene.Scene" ), _show, flagsWindow ) == false )
			{
				ImGui::End();
				return;
			}

			_drawComponents();

			ImGui::End();
		}

	} // namespace UI
} // namespace VTX
