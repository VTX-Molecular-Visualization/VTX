#include "component_scene.hpp"
#include "../defines.hpp"
#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "../style.hpp"
#include "../view/base_view_ui.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentScene::ComponentScene( bool * const p_show ) : BaseViewUI<Model::ModelMolecule>( p_show ) {}

		void ComponentScene::_draw()
		{
			ImGuiWindowFlags flagsWindow = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Scene.Scene" ), _show, flagsWindow ) == false )
			{
				ImGui::End();
				return;
			}

			ImGuiIO & io = ImGui::GetIO();

			ImGui::Text( "Time UI:  %.0f ms", VTXApp::get().getTimeLastUI() * 1000.0 );
			ImGui::Text( "Time renderer:  %.0f ms", VTXApp::get().getTimeLastRenderer() * 1000.0 );
			ImGui::Text( "FPS ImGui: %.0f", io.Framerate );

			try
			{
				ImGui::Text( "FPS computed: %.0f", 1.f / VTXApp::get().getTimeLast() );
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
					ImGui::PushID( chain.getId() );
					if ( ImGui::TreeNode( chain.getName().c_str() ) )
					{
						for ( int i = 0; i < chain.getResidueCount(); ++i )
						{
							Model::ModelResidue & residue = _model->getResidue( chain.getIdFirstResidue() + i );
							ImGui::PushID( residue.getId() );
							if ( ImGui::TreeNode( residue.getSymbolName().c_str() ) )
							{
								for ( int j = 0; j < residue.getAtomCount(); ++j )
								{
									Model::ModelAtom & atom = _model->getAtom( residue.getIdFirstAtom() + j );
									ImGui::PushID( atom.getId() );
									if ( ImGui::Selectable( atom.getName().c_str() ) )
									{
										_model->setSelectedAtom( atom.getId() );
										ImGui::SetItemDefaultFocus();
									}
									ImGui::PopID();
								}
								ImGui::TreePop();
							}
							ImGui::PopID();
						}
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
				ImGui::TreePop();
			}
			ImGui::End();
		}

		void ComponentScene::notify( Event::EVENT_MODEL p_event ) {}
	} // namespace UI
} // namespace VTX
