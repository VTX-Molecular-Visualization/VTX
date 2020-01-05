#include "view_ui_molecule_scene.hpp"
#include "../settings.hpp"

namespace VTX
{
	namespace View
	{
		void ViewUIMoleculeScene::notify( Event::EVENT_MODEL p_event ) {}

		void ViewUIMoleculeScene::_draw()
		{
			if ( ImGui::CollapsingHeader( LOCALE( "View.Molecule.Data" ), ImGuiTreeNodeFlags_DefaultOpen ) )
			{
				ImGui::Text( "Chains: %d", _model->getChainCount() );
				ImGui::Text( "Residues: %d", _model->getResidueCount() );
				ImGui::Text( "Atoms: %d", _model->getAtomCount() );
				ImGui::Text( "Bonds: %d", _model->getBondCount() / 2 );
			}

			ImGui::PushID( "ViewMolecule" );
			if ( ImGui::CollapsingHeader( _model->getName().c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
			{
				for ( Model::ModelChain & chain : _model->getChains() )
				{
					ImGui::PushID( chain.getId() );
					bool chainOpened = ImGui::TreeNodeEx(
						chain.getName().c_str(),
						chain.isSelected() ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None );
					if ( ImGui::IsItemClicked() )
					{
						if ( chainOpened )
							_model->resetSelectedChain();
						else
							_model->setSelectedChain( chain.getId() );
					}
					if ( chainOpened )
					{
						for ( uint i = 0; i < chain.getResidueCount(); ++i )
						{
							Model::ModelResidue & residue = _model->getResidue( chain.getIdFirstResidue() + i );
							ImGui::PushID( residue.getId() );
							bool residueOpened = ImGui::TreeNodeEx(
								VTX::Setting::UI::symbolDisplayMode == VTX::Setting::UI::SYMBOL_DISPLAY_MODE::SHORT
									? ( residue.getSymbolShort() + " " + std::to_string( residue.getId() ) ).c_str()
									: residue.getSymbolName().c_str(),
								residue.isSelected() ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None );
							if ( ImGui::IsItemClicked() )
							{
								if ( residueOpened )
									_model->resetSelectedResidue();
								else
									_model->setSelectedResidue( residue.getId() );
							}
							if ( residueOpened )
							{
								for ( uint j = 0; j < residue.getAtomCount(); ++j )
								{
									Model::ModelAtom & atom = _model->getAtom( residue.getIdFirstAtom() + j );
									ImGui::PushID( atom.getId() );
									if ( ImGui::Selectable(
											 VTX::Setting::UI::symbolDisplayMode
													 == VTX::Setting::UI::SYMBOL_DISPLAY_MODE::SHORT
												 ? ( atom.getSymbolStr() + " " + std::to_string( atom.getId() ) )
													   .c_str()
												 : atom.getSymbolName().c_str(),
											 atom.isSelected() ) )
									{
										_model->setSelectedAtom( atom.getId() );
										// ImGui::SetItemDefaultFocus();
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
			}
			ImGui::PopID();
		}
	} // namespace View
} // namespace VTX
