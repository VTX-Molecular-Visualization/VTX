#include "molecule_structure.hpp"
#include "setting.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			void MoleculeStructure::_draw()
			{
				ImGui::PushID( ( "ViewMolecule" + std::to_string( _getModel().getId() ) ).c_str() );

				if ( ImGui::CollapsingHeader( LOCALE( "View.Data" ), ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					ImGui::Text( "Chains: %d", _getModel().getChainCount() );
					ImGui::Text( "Residues: %d", _getModel().getResidueCount() );
					ImGui::Text( "Atoms: %d", _getModel().getAtomCount() );
					ImGui::Text( "Bonds: %d", _getModel().getBondCount() / 2 );
				}

				if ( ImGui::CollapsingHeader( _getModel().getName().c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					for ( Model::Chain & chain : _getModel().getChains() )
					{
						ImGui::PushID( chain.getId() );
						bool chainOpened = ImGui::TreeNodeEx(
							chain.getName().c_str(),
							chain.isSelected() ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None );
						if ( ImGui::IsItemClicked() )
						{
							if ( chainOpened )
								_getModel().resetSelectedChain();
							else
								_getModel().setSelectedChain( &chain );
						}
						if ( chainOpened )
						{
							for ( uint i = 0; i < chain.getResidueCount(); ++i )
							{
								Model::Residue & residue = _getModel().getResidue( chain.getIdFirstResidue() + i );
								ImGui::PushID( residue.getId() );
								bool residueOpened = ImGui::TreeNodeEx(
									VTX::Setting::UI::symbolDisplayMode == VTX::Setting::UI::SYMBOL_DISPLAY_MODE::SHORT
										? ( residue.getSymbolShort() + " " + std::to_string( residue.getId() ) ).c_str()
										: residue.getSymbolName().c_str(),
									residue.isSelected() ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None );
								if ( ImGui::IsItemClicked() )
								{
									if ( residueOpened )
										_getModel().resetSelectedResidue();
									else
										_getModel().setSelectedResidue( &residue );
								}
								if ( residueOpened )
								{
									for ( uint j = 0; j < residue.getAtomCount(); ++j )
									{
										Model::Atom & atom = _getModel().getAtom( residue.getIdFirstAtom() + j );
										ImGui::PushID( atom.getId() );
										if ( ImGui::Selectable(
												 VTX::Setting::UI::symbolDisplayMode
														 == VTX::Setting::UI::SYMBOL_DISPLAY_MODE::SHORT
													 ? ( atom.getSymbolStr() + " " + std::to_string( atom.getId() ) )
														   .c_str()
													 : atom.getSymbolName().c_str(),
												 atom.isSelected() ) )
										{
											_getModel().setSelectedAtom( &atom );
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
		} // namespace UI
	}	  // namespace View
} // namespace VTX
