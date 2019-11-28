#include "view_ui_molecule.hpp"
#include "../lib/imgui/imgui.h"

namespace VTX
{
	namespace View
	{
		void ViewUIMolecule::notify( Event::EVENT_MODEL p_event ) {}

		void ViewUIMolecule::_draw()
		{
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
		}
	} // namespace View
} // namespace VTX
