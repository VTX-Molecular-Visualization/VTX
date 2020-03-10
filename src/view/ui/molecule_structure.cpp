#include "molecule_structure.hpp"
#include "action/action_manager.hpp"
#include "action/select.hpp"
#include "action/unselect.hpp"
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

				if ( ImGui::CollapsingHeader( _getModel().getName().c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					ImGui::Text( "Chains: %d", _getModel().getChainCount() );
					ImGui::Text( "Residues: %d", _getModel().getResidueCount() );
					ImGui::Text( "Atoms: %d", _getModel().getAtomCount() );
					ImGui::Text( "Bonds: %d", _getModel().getBondCount() / 2 );
					ImGui::Separator();
					for ( Model::Chain * const chain : _getModel().getChains() )
					{
						ImGui::PushID( chain->getId() );
						bool chainOpened = ImGui::TreeNodeEx(
							chain->getName().c_str(),
							chain->isSelected() ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None );
						if ( ImGui::IsItemClicked() )
						{
							if ( chainOpened )
							{ VTXApp::get().getActionManager().execute( new Action::Unselect( *chain ) ); }

							else
							{
								VTXApp::get().getActionManager().execute( new Action::Select( *chain ) );
							}
						}
						if ( chainOpened )
						{
							for ( uint i = 0; i < chain->getResidueCount(); ++i )
							{
								Model::Residue & residue = _getModel().getResidue( chain->getIdFirstResidue() + i );
								ImGui::PushID( residue.getId() );
								bool residueOpened = ImGui::TreeNodeEx(
									VTX::Setting::UI::symbolDisplayMode == VTX::Setting::UI::SYMBOL_DISPLAY_MODE::SHORT
										? ( residue.getSymbolShort() + " " + std::to_string( residue.getId() ) ).c_str()
										: residue.getSymbolName().c_str(),
									residue.isSelected() ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None );
								if ( ImGui::IsItemClicked() )
								{
									if ( residueOpened )
									{ VTXApp::get().getActionManager().execute( new Action::Unselect( residue ) ); }
									else
									{
										VTXApp::get().getActionManager().execute( new Action::Select( residue ) );
									}
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
											if ( atom.isSelected() )
											{
												VTXApp::get().getActionManager().execute(
													new Action::Unselect( atom ) );
											}
											else
											{
												VTXApp::get().getActionManager().execute( new Action::Select( atom ) );
											}
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
