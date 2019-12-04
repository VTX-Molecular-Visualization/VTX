#include "view_ui_molecule.hpp"

namespace VTX
{
	namespace View
	{
		void ViewUIMolecule::notify( Event::EVENT_MODEL p_event ) {}

		void ViewUIMolecule::_draw()
		{
			if ( ImGui::CollapsingHeader( LOCALE( "View.Molecule.Data" ), ImGuiTreeNodeFlags_DefaultOpen ) )
			{
				ImGui::Text( "Chains: %d", _model->getChainCount() );
				ImGui::Text( "Residues: %d", _model->getResidueCount() );
				ImGui::Text( "Atoms: %d", _model->getAtomCount() );
				ImGui::Text( "Bonds: %d", _model->getBondCount() );
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
					if ( chainOpened && chain.isSelected() )
					{
						for ( uint i = 0; i < chain.getResidueCount(); ++i )
						{
							Model::ModelResidue & residue = _model->getResidue( chain.getIdFirstResidue() + i );
							ImGui::PushID( residue.getId() );
							bool residueOpened = ImGui::TreeNodeEx(
								residue.getSymbolShort().c_str(),
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
									if ( ImGui::Selectable( atom.getSymbolStr().c_str(), atom.isSelected() ) )
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
			}
			ImGui::PopID();
		}
	} // namespace View
} // namespace VTX
