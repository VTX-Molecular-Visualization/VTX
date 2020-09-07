#include "molecule_structure.hpp"
#include "action/action_manager.hpp"
#include "action/chain.hpp"
#include "action/molecule.hpp"
#include "action/residue.hpp"
#include "action/selectable.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			void MoleculeStructure::_draw()
			{
				const std::string nbAtoms = std::to_string( _model->getAtomCount() ) + " atoms";
				const std::string nbBonds = std::to_string( _model->getBondCount() ) + " bonds";
				ImGui::PushID( ( "ViewMoleculeStructure" + std::to_string( _model->getId() ) ).c_str() );
				bool moleculeOpened = ImGui::TreeNodeEx(
					( _model->getPath().filename().string() + "\n" + _model->getName() + "\n" + nbAtoms ).c_str(),
					_model->isSelected() ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None );

				// Context menu.
				if ( ImGui::BeginPopupContextItem() )
				{
					if ( ImGui::MenuItem( LOCALE( "View.Delete" ) ) )
					{
						VTX_ACTION( new Action::Molecule::Delete( *_model ) );
					}
					ImGui::EndPopup();
				}
				// Context menu END.
				if ( ImGui::IsItemClicked() )
				{
					if ( moleculeOpened )
					{
						VTX_ACTION( new Action::Selectable::SetSelected( *_model, false ) );
					}

					else
					{
						VTX_ACTION( new Action::Selectable::SetSelected( *_model, true ) );
					}
				}
				if ( moleculeOpened )
				{
					for ( Model::Chain * const chain : _model->getChains() )
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
									VTX_ACTION( new Action::Chain::ChangeVisibility(
										*chain, Action::Chain::ChangeVisibility::VISIBILITY_MODE::SHOW ) );
								}
							}
							else
							{
								if ( ImGui::MenuItem( LOCALE( "View.Hide" ) ) )
								{
									VTX_ACTION( new Action::Chain::ChangeVisibility(
										*chain, Action::Chain::ChangeVisibility::VISIBILITY_MODE::HIDE ) );
								}
							}
							if ( ImGui::MenuItem( LOCALE( "View.Solo" ) ) )
							{
								VTX_ACTION( new Action::Chain::ChangeVisibility(
									*chain, Action::Chain::ChangeVisibility::VISIBILITY_MODE::SOLO ) );
							}
							if ( ImGui::MenuItem( LOCALE( "View.All" ) ) )
							{
								VTX_ACTION( new Action::Chain::ChangeVisibility(
									*chain, Action::Chain::ChangeVisibility::VISIBILITY_MODE::ALL ) );
							}
							ImGui::EndPopup();
						}
						// Context menu END.
						if ( ImGui::IsItemClicked() )
						{
							if ( chainOpened )
							{
								VTX_ACTION( new Action::Selectable::SetSelected( *chain, false ) );
							}

							else
							{
								VTX_ACTION( new Action::Selectable::SetSelected( *chain, true ) );
							}
						}
						if ( chainOpened )
						{
							for ( uint i = 0; i < chain->getResidueCount(); ++i )
							{
								Model::Residue & residue = _model->getResidue( chain->getIndexFirstResidue() + i );
								ImGui::PushID( residue.getId() );
								bool residueOpened = ImGui::TreeNodeEx(
									( ( VTX_SETTING().symbolDisplayMode == Style::SYMBOL_DISPLAY_MODE::SHORT
											? residue.getSymbolShort()
											: residue.getSymbolName() )
									  + " " + std::to_string( residue.getIndex() ) )
										.c_str(),
									residue.isSelected() ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None );
								// Context menu.
								if ( ImGui::BeginPopupContextItem() )
								{
									if ( residue.isVisible() == false )
									{
										if ( ImGui::MenuItem( LOCALE( "View.Show" ) ) )
										{
											VTX_ACTION( new Action::Residue::ChangeVisibility(
												residue, Action::Residue::ChangeVisibility::VISIBILITY_MODE::SHOW ) );
										}
									}
									else
									{
										if ( ImGui::MenuItem( LOCALE( "View.Hide" ) ) )
										{
											VTX_ACTION( new Action::Residue::ChangeVisibility(
												residue, Action::Residue::ChangeVisibility::VISIBILITY_MODE::HIDE ) );
										}
									}
									if ( ImGui::MenuItem( LOCALE( "View.Solo" ) ) )
									{
										VTX_ACTION( new Action::Residue::ChangeVisibility(
											residue, Action::Residue::ChangeVisibility::VISIBILITY_MODE::SOLO ) );
									}
									if ( ImGui::MenuItem( LOCALE( "View.All" ) ) )
									{
										VTX_ACTION( new Action::Residue::ChangeVisibility(
											residue, Action::Residue::ChangeVisibility::VISIBILITY_MODE::ALL ) );
									}
									ImGui::EndPopup();
								}
								// Context menu END.
								if ( ImGui::IsItemClicked() )
								{
									if ( residueOpened )
									{
										VTX_ACTION( new Action::Selectable::SetSelected( residue, false ) );
									}
									else
									{
										VTX_ACTION( new Action::Selectable::SetSelected( residue, true ) );
									}
								}
								if ( residueOpened )
								{
									for ( uint j = 0; j < residue.getAtomCount(); ++j )
									{
										Model::Atom & atom = _model->getAtom( residue.getIndexFirstAtom() + j );
										ImGui::PushID( atom.getId() );
										if ( ImGui::Selectable( ( ( VTX_SETTING().symbolDisplayMode
																			== Style::SYMBOL_DISPLAY_MODE::SHORT
																		? atom.getSymbolStr()
																		: atom.getSymbolName() )
																  + " " + std::to_string( atom.getIndex() ) )
																	.c_str(),
																atom.isSelected() ) )
										{
											if ( atom.isSelected() )
											{
												VTX_ACTION( new Action::Selectable::SetSelected( atom, false ) );
											}
											else
											{
												VTX_ACTION( new Action::Selectable::SetSelected( atom, true ) );
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
