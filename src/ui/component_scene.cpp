#include "component_scene.hpp"
#include "../defines.hpp"
#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "../style.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentScene::ComponentScene( bool * const p_show ) : BaseComponent( p_show ) {}

		void ComponentScene::_draw()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Scene.Scene" ), _show, flags ) == false )
			{
				ImGui::End();
				return;
			}

			ImGuiIO & io = ImGui::GetIO();

			ImGui::Text( "Time UI:  %.3fms", VTXApp::get().getTimeLastUI() );
			ImGui::Text( "Time renderer:  %.3fms", VTXApp::get().getTimeLastRenderer() );
			ImGui::Text( "FPS ImGui: %.1f", io.Framerate );

			try
			{
				ImGui::Text( "FPS computed: %.1f", 1e3f / VTXApp::get().getTimeLast() );
			}
			catch ( const std::exception )
			{
			}

			ImGui::Separator();

			if ( _model == nullptr )
			{
				ImGui::End();
				return;
			}

			ImGui::Text( "Chains: %d", _model->getChainCount() );
			ImGui::Text( "Residues: %d", _model->getResidueCount() );
			ImGui::Text( "Atoms: %d", _model->getAtomCount() );
			ImGui::Text( "Bonds: %d", _model->getBondCount() );
			ImGui::Separator();

			if ( ImGui::TreeNode( _model->getName().c_str() ) )
			{
				for ( Model::ModelChain & chain : _model->getChains() )
				{
					if ( ImGui::TreeNode( chain.getName().c_str() ) )
					{
						for ( int i = 0; i < chain.getResidueCount(); ++i )
						{
							Model::ModelResidue & residue = _model->getResidue( chain.getIdFirstResidue() + i );
							if ( ImGui::TreeNode( residue.getSymbolName().c_str() ) )
							{
								for ( int j = 0; j < residue.getAtomCount(); ++j )
								{
									Model::ModelAtom & atom = _model->getAtom( residue.getIdFirstAtom() + j );
									ImGui::Selectable( atom.getName().c_str() );
								}
								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
			ImGui::End();
		}

		void ComponentScene::notify( Event::EVENT_MODEL p_event )
		{
			if ( p_event == Event::EVENT_MODEL::UPDATE ) { VTX_INFO( "NOTIFY VIEW GUI" ); }
		}
	} // namespace UI
} // namespace VTX
