#include "molecule_structure.hpp"
#include "action/action_manager.hpp"
#include "action/chain_change_visibility.hpp"
#include "action/molecule_delete.hpp"
#include "action/residue_change_visibility.hpp"
#include "action/selectable_select.hpp"
#include "action/selectable_unselect.hpp"
#include "setting.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			void MoleculeStructure::_draw()
			{
				ImGui::PushID( ( "ViewMoleculeStructure" + std::to_string( _getModel().getId() ) ).c_str() );
				bool moleculeOpened = ImGui::TreeNodeEx(
					( _getModel().getFileName() + " " + _getModel().getName() ).c_str(),
					_getModel().isSelected() ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None );
				// Context menu.
				if ( ImGui::BeginPopupContextItem() )
				{
					if ( ImGui::MenuItem( LOCALE( "View.Delete" ) ) )
					{
						VTX_ACTION( new Action::MoleculeDelete( _getModel() ) );
					}
					ImGui::EndPopup();
				}
				// Context menu END.
				if ( ImGui::IsItemClicked() )
				{
					if ( moleculeOpened )
					{
						VTX_ACTION( new Action::SelectableUnselect( _getModel() ) );
					}

					else
					{
						VTX_ACTION( new Action::SelectableSelect( _getModel() ) );
					}
				}
				if ( moleculeOpened )
				{
					for ( Model::Chain * const chain : _getModel().getChains() )
					{
						ImGui::PushID( chain->getId() );
						bool chainOpened = ImGui::TreeNodeEx(
							chain->getName().c_str(),
							chain->isSelected() ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None );
						// Context menu.
						if ( ImGui::BeginPopupContextItem() )
						{
							if ( chain->isVisible() == false )
							{
								if ( ImGui::MenuItem( LOCALE( "View.Show" ) ) )
								{
									VTX_ACTION( new Action::ChainChangeVisibility(
										*chain, Action::ChainChangeVisibility::VISIBILITY_MODE::SHOW ) );
								}
							}
							else
							{
								if ( ImGui::MenuItem( LOCALE( "View.Hide" ) ) )
								{
									VTX_ACTION( new Action::ChainChangeVisibility(
										*chain, Action::ChainChangeVisibility::VISIBILITY_MODE::HIDE ) );
								}
							}
							if ( ImGui::MenuItem( LOCALE( "View.Solo" ) ) )
							{
								VTX_ACTION( new Action::ChainChangeVisibility(
									*chain, Action::ChainChangeVisibility::VISIBILITY_MODE::SOLO ) );
							}
							if ( ImGui::MenuItem( LOCALE( "View.All" ) ) )
							{
								VTX_ACTION( new Action::ChainChangeVisibility(
									*chain, Action::ChainChangeVisibility::VISIBILITY_MODE::ALL ) );
							}
							ImGui::EndPopup();
						}
						// Context menu END.
						if ( ImGui::IsItemClicked() )
						{
							if ( chainOpened )
							{
								VTX_ACTION( new Action::SelectableUnselect( *chain ) );
							}

							else
							{
								VTX_ACTION( new Action::SelectableSelect( *chain ) );
							}
						}
						if ( chainOpened )
						{
							for ( uint i = 0; i < chain->getResidueCount(); ++i )
							{
								Model::Residue & residue = _getModel().getResidue( chain->getIndexFirstResidue() + i );
								ImGui::PushID( residue.getId() );
								bool residueOpened = ImGui::TreeNodeEx(
									VTX::Setting::UI::symbolDisplayMode == VTX::Setting::UI::SYMBOL_DISPLAY_MODE::SHORT
										? ( residue.getSymbolShort() + " " + std::to_string( residue.getIndex() ) )
											  .c_str()
										: residue.getSymbolName().c_str(),
									residue.isSelected() ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None );
								// Context menu.
								if ( ImGui::BeginPopupContextItem() )
								{
									if ( residue.isVisible() == false )
									{
										if ( ImGui::MenuItem( LOCALE( "View.Show" ) ) )
										{
											VTX_ACTION( new Action::ResidueChangeVisibility(
												residue, Action::ResidueChangeVisibility::VISIBILITY_MODE::SHOW ) );
										}
									}
									else
									{
										if ( ImGui::MenuItem( LOCALE( "View.Hide" ) ) )
										{
											VTX_ACTION( new Action::ResidueChangeVisibility(
												residue, Action::ResidueChangeVisibility::VISIBILITY_MODE::HIDE ) );
										}
									}
									if ( ImGui::MenuItem( LOCALE( "View.Solo" ) ) )
									{
										VTX_ACTION( new Action::ResidueChangeVisibility(
											residue, Action::ResidueChangeVisibility::VISIBILITY_MODE::SOLO ) );
									}
									if ( ImGui::MenuItem( LOCALE( "View.All" ) ) )
									{
										VTX_ACTION( new Action::ResidueChangeVisibility(
											residue, Action::ResidueChangeVisibility::VISIBILITY_MODE::ALL ) );
									}
									ImGui::EndPopup();
								}
								// Context menu END.
								if ( ImGui::IsItemClicked() )
								{
									if ( residueOpened )
									{
										VTX_ACTION( new Action::SelectableUnselect( residue ) );
									}
									else
									{
										VTX_ACTION( new Action::SelectableSelect( residue ) );
									}
								}
								if ( residueOpened )
								{
									for ( uint j = 0; j < residue.getAtomCount(); ++j )
									{
										Model::Atom & atom = _getModel().getAtom( residue.getIndexFirstAtom() + j );
										ImGui::PushID( atom.getId() );
										if ( ImGui::Selectable(
												 VTX::Setting::UI::symbolDisplayMode
														 == VTX::Setting::UI::SYMBOL_DISPLAY_MODE::SHORT
													 ? ( atom.getSymbolStr() + " " + std::to_string( atom.getIndex() ) )
														   .c_str()
													 : atom.getSymbolName().c_str(),
												 atom.isSelected() ) )
										{
											if ( atom.isSelected() )
											{
												VTX_ACTION( new Action::SelectableUnselect( atom ) );
											}
											else
											{
												VTX_ACTION( new Action::SelectableSelect( atom ) );
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
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
