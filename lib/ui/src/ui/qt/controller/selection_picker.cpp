// #include "ui/qt/controller/picker.hpp"
// #include "ui/qt/action/selection.hpp"
// #include "ui/qt/application_qt.hpp"
// #include "ui/qt/main_window.hpp"
// #include "ui/qt/tool/keys.hpp"
// #include "ui/qt/tool/render/widget/render_widget.hpp"
// #include <app/old/action/selection.hpp>
// #include <app/old/application/selection/enum_selection.hpp>
// #include <app/old/application/selection/selection.hpp>
// #include <app/old/application/selection/selection_manager.hpp>
// #include <app/old/component/chemistry/atom.hpp>
// #include <app/old/component/chemistry/chain.hpp>
// #include <app/old/component/chemistry/molecule.hpp>
// #include <app/old/component/chemistry/residue.hpp>
// #include <app/old/mvc.hpp>
// #include <util/logger.hpp>
//
// namespace VTX::UI::QT::Controller
//{
//	Picker::Picker() : _lastClickedIds { App::Old::Core::Model::ID_UNKNOWN, App::Old::Core::Model::ID_UNKNOWN } {}
//	void Picker::update( const float & p_deltaTime ) { BaseMouseController::update( p_deltaTime ); }
//
//	void Picker::_onMouseLeftClick( const uint p_x, const uint p_y )
//	{
//		const Vec2i ids = QT_APP()->getMainWindow().getRender()->getPickedIds( p_x, p_y );
//		_performSelection( ids );
//		_lastClickedIds = ids;
//	}
//
//	void Picker::_onMouseRightClick( const uint p_x, const uint p_y )
//	{
//		QT::Tool::Render::Widget::RenderWidget * renderWidget = QT_APP()->getMainWindow().getRender();
//
//		const Vec2i ids = renderWidget->getPickedIds( p_x, p_y );
//		if ( !_isTargetSelected( ids ) )
//		{
//			_performSelection( ids );
//			_lastClickedIds = ids;
//		}
//
//		App::Old::Application::Selection::SelectionModel & selection
//			= VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel();
//
//		const QPoint position = renderWidget->mapToGlobal( QPoint( p_x, p_y ) );
//
//		if ( selection.isEmpty() )
//		{
//			QT_APP()->getMainWindow().getContextualMenu().pop( Tool::ContextualMenu::RENDER, position );
//		}
//		else
//		{
//			QT_APP()->getMainWindow().getContextualMenu().pop( Tool::ContextualMenu::SELECTION, &selection, position );
//		}
//	}
//
//	void Picker::_performSelection( const Vec2i & p_ids ) const
//	{
//		// Clear selection if not CTRL.
//		if ( _isModifierExclusive( ModifierFlag::Control ) == false )
//		{
//			VTX_ACTION( new VTX::App::Old::Action::Selection::ClearSelection(
//				App::Old::Application::Selection::SelectionManager::get().getSelectionModel() ) );
//		}
//
//		// If something clicked.
//		if ( p_ids.x != App::Old::Core::Model::ID_UNKNOWN )
//		{
//			const App::Old::VTX_ID & typeId = VTX::MVC_MANAGER().getModelTypeID( p_ids.x );
//
//			// Already selected.
//			if ( App::Old::Application::Selection::SelectionManager::get().getSelectionModel().isModelSelected(
//					 VTX::MVC_MANAGER().getModel<App::Old::Core::Model::BaseModel>( p_ids.x ) )
//				 && ( p_ids.y != App::Old::Core::Model::ID_UNKNOWN
//						  ?
// App::Old::Application::Selection::SelectionManager::get().getSelectionModel().isModelSelected(
//							  VTX::MVC_MANAGER().getModel<App::Old::Core::Model::BaseModel>( p_ids.y ) )
//						  : true ) )
//			{
//				// Remove from selection.
//				if ( _isModifierExclusive( ModifierFlag::Control ) )
//				{
//					// Residue.
//					if ( typeId == App::Old::ID::Model::MODEL_RESIDUE )
//					{
//						App::Old::Component::Chemistry::Residue & residuePicked
//							= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Residue>( p_ids.x );
//						_unselectItem( residuePicked );
//					}
//					// Bond.
//					else if ( p_ids.y != App::Old::Core::Model::ID_UNKNOWN )
//					{
//						App::Old::Component::Chemistry::Atom & atomPicked1
//							= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Atom>( p_ids.x );
//						App::Old::Component::Chemistry::Atom & atomPicked2
//							= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Atom>( p_ids.y );
//
//						_unselectItem( atomPicked1, atomPicked2 );
//					}
//					// Atom.
//					else
//					{
//						App::Old::Component::Chemistry::Atom & atomPicked
//							= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Atom>( p_ids.x );
//						_unselectItem( atomPicked );
//					}
//				}
//			}
//			// Add to selection.
//			else
//			{
//				// Residue.
//				if ( typeId == App::Old::ID::Model::MODEL_RESIDUE )
//				{
//					App::Old::Component::Chemistry::Residue & residuePicked
//						= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Residue>( p_ids.x );
//					_selectItem( residuePicked );
//				}
//				// Bond.
//				else if ( p_ids.y != App::Old::Core::Model::ID_UNKNOWN )
//				{
//					App::Old::Component::Chemistry::Atom & atomPicked1
//						= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Atom>( p_ids.x );
//					App::Old::Component::Chemistry::Atom & atomPicked2
//						= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Atom>( p_ids.y );
//
//					_selectItem( atomPicked1, atomPicked2 );
//				}
//				// Atom.
//				else
//				{
//					App::Old::Component::Chemistry::Atom & atomPicked
//						= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Atom>( p_ids.x );
//					_selectItem( atomPicked );
//				}
//			}
//		}
//	}
//
//	bool Picker::_isTargetSelected( const Vec2i & p_ids ) const
//	{
//		if ( p_ids.x == App::Old::Core::Model::ID_UNKNOWN )
//			return false;
//
//		bool targetSelected =
// App::Old::Application::Selection::SelectionManager::get().getSelectionModel().isModelSelected(
//			MVC_MANAGER().getModel<App::Old::Core::Model::BaseModel>( p_ids.x ) );
//
//		if ( p_ids.y != App::Old::Core::Model::ID_UNKNOWN )
//		{
//			targetSelected
//				= targetSelected
//				  && App::Old::Application::Selection::SelectionManager::get().getSelectionModel().isModelSelected(
//					  MVC_MANAGER().getModel<App::Old::Core::Model::BaseModel>( p_ids.y ) );
//		}
//
//		return targetSelected;
//	}
//
//	void Picker::_selectItem( App::Old::Component::Chemistry::Atom & p_atomPicked ) const
//	{
//		App::Old::Application::Selection::SelectionModel & selectionModel
//			= App::Old::Application::Selection::SelectionManager::get().getSelectionModel();
//		const bool appendToSelection = _isModifierExclusive( ModifierFlag::Control );
//
//		switch ( VTX_SETTING().getSelectionGranularity() )
//		{
//		case VTX::App::Old::Application::Selection::GRANULARITY::MOLECULE:
//			VTX_ACTION( new VTX::App::Old::Action::Selection::SelectMolecule(
//				selectionModel, *p_atomPicked.getMoleculePtr(), appendToSelection ) );
//			break;
//		case VTX::App::Old::Application::Selection::GRANULARITY::CHAIN:
//			VTX_ACTION( new VTX::App::Old::Action::Selection::SelectChain(
//				selectionModel, *p_atomPicked.getChainPtr(), appendToSelection ) );
//			break;
//		case VTX::App::Old::Application::Selection::GRANULARITY::RESIDUE:
//			VTX_ACTION( new VTX::App::Old::Action::Selection::SelectResidue(
//				selectionModel, *p_atomPicked.getResiduePtr(), appendToSelection ) );
//			break;
//		case VTX::App::Old::Application::Selection::GRANULARITY::ATOM:
//		default:
//			VTX_ACTION(
//				new VTX::App::Old::Action::Selection::SelectAtom( selectionModel, p_atomPicked, appendToSelection ) );
//			break;
//		}
//	}
//	void Picker::_selectItem( App::Old::Component::Chemistry::Atom & p_atomPicked1,
//							  App::Old::Component::Chemistry::Atom & p_atomPicked2 ) const
//	{
//		App::Old::Application::Selection::SelectionModel & selectionModel
//			= App::Old::Application::Selection::SelectionManager::get().getSelectionModel();
//		const bool appendToSelection = _isModifierExclusive( ModifierFlag::Control );
//
//		switch ( VTX_SETTING().getSelectionGranularity() )
//		{
//		case VTX::App::Old::Application::Selection::GRANULARITY::MOLECULE:
//		{
//			App::Old::Component::Chemistry::Molecule * const mol1 = p_atomPicked1.getMoleculePtr();
//			App::Old::Component::Chemistry::Molecule * const mol2 = p_atomPicked2.getMoleculePtr();
//			if ( mol1 == mol2 )
//			{
//				VTX_ACTION(
//					new VTX::App::Old::Action::Selection::SelectMolecule( selectionModel, *mol1, appendToSelection ) );
//			}
//			else
//			{
//				VTX_ACTION( new VTX::App::Old::Action::Selection::SelectMolecule(
//					selectionModel, { mol1, mol2 }, appendToSelection ) );
//			}
//			break;
//		}
//		case VTX::App::Old::Application::Selection::GRANULARITY::CHAIN:
//		{
//			App::Old::Component::Chemistry::Chain * const chain1 = p_atomPicked1.getChainPtr();
//			App::Old::Component::Chemistry::Chain * const chain2 = p_atomPicked2.getChainPtr();
//
//			if ( chain1 == chain2 )
//			{
//				VTX_ACTION(
//					new VTX::App::Old::Action::Selection::SelectChain( selectionModel, *chain1, appendToSelection ) );
//			}
//			else
//			{
//				VTX_ACTION( new VTX::App::Old::Action::Selection::SelectChain(
//					selectionModel, { chain1, chain2 }, appendToSelection ) );
//			}
//			break;
//		}
//		case VTX::App::Old::Application::Selection::GRANULARITY::RESIDUE:
//		{
//			App::Old::Component::Chemistry::Residue * const res1 = p_atomPicked1.getResiduePtr();
//			App::Old::Component::Chemistry::Residue * const res2 = p_atomPicked2.getResiduePtr();
//
//			if ( res1 == res2 )
//			{
//				VTX_ACTION(
//					new VTX::App::Old::Action::Selection::SelectResidue( selectionModel, *res1, appendToSelection ) );
//			}
//			else
//			{
//				VTX_ACTION( new VTX::App::Old::Action::Selection::SelectResidue(
//					selectionModel, { res1, res2 }, appendToSelection ) );
//			}
//			break;
//		}
//		case VTX::App::Old::Application::Selection::GRANULARITY::ATOM:
//		default:
//		{
//			if ( &p_atomPicked1 == &p_atomPicked2 )
//			{
//				VTX_ACTION(
//					new VTX::App::Old::Action::Selection::SelectAtom( selectionModel, p_atomPicked1, appendToSelection )
//);
//			}
//			else
//			{
//				VTX_ACTION( new VTX::App::Old::Action::Selection::SelectAtom(
//					selectionModel, { &p_atomPicked1, &p_atomPicked2 }, appendToSelection ) );
//			}
//			break;
//		}
//		}
//	}
//	void Picker::_selectItem( App::Old::Component::Chemistry::Residue & p_residuePicked ) const
//	{
//		App::Old::Application::Selection::SelectionModel & selectionModel
//			= App::Old::Application::Selection::SelectionManager::get().getSelectionModel();
//		const bool appendToSelection = _isModifierExclusive( ModifierFlag::Control );
//
//		switch ( VTX_SETTING().getSelectionGranularity() )
//		{
//		case VTX::App::Old::Application::Selection::GRANULARITY::MOLECULE:
//			VTX_ACTION( new VTX::App::Old::Action::Selection::SelectMolecule(
//				selectionModel, *p_residuePicked.getMoleculePtr(), appendToSelection ) );
//			break;
//		case VTX::App::Old::Application::Selection::GRANULARITY::CHAIN:
//			VTX_ACTION( new VTX::App::Old::Action::Selection::SelectChain(
//				selectionModel, *p_residuePicked.getChainPtr(), appendToSelection ) );
//			break;
//		case VTX::App::Old::Application::Selection::GRANULARITY::RESIDUE:
//		case VTX::App::Old::Application::Selection::GRANULARITY::ATOM:
//		default:
//			VTX_ACTION(
//				new VTX::App::Old::Action::Selection::SelectResidue( selectionModel, p_residuePicked, appendToSelection
//)
//); 			break;
//		}
//	}
//
//	void Picker::_unselectItem( App::Old::Component::Chemistry::Atom & p_atomPicked ) const
//	{
//		App::Old::Application::Selection::SelectionModel & selectionModel
//			= App::Old::Application::Selection::SelectionManager::get().getSelectionModel();
//		const bool appendToSelection = _isModifierExclusive( ModifierFlag::Control );
//
//		switch ( VTX_SETTING().getSelectionGranularity() )
//		{
//		case VTX::App::Old::Application::Selection::GRANULARITY::MOLECULE:
//			VTX_ACTION( new VTX::App::Old::Action::Selection::UnselectMolecule(
//				selectionModel, *p_atomPicked.getMoleculePtr(), appendToSelection ) );
//			break;
//		case VTX::App::Old::Application::Selection::GRANULARITY::CHAIN:
//			VTX_ACTION( new VTX::App::Old::Action::Selection::UnselectChain(
//				selectionModel, *p_atomPicked.getChainPtr(), appendToSelection ) );
//			break;
//		case VTX::App::Old::Application::Selection::GRANULARITY::RESIDUE:
//			VTX_ACTION( new VTX::App::Old::Action::Selection::UnselectResidue(
//				selectionModel, *p_atomPicked.getResiduePtr(), appendToSelection ) );
//			break;
//		case VTX::App::Old::Application::Selection::GRANULARITY::ATOM:
//		default:
//			VTX_ACTION(
//				new VTX::App::Old::Action::Selection::UnselectAtom( selectionModel, p_atomPicked, appendToSelection ) );
//			break;
//		}
//	}
//	void Picker::_unselectItem( App::Old::Component::Chemistry::Atom & p_atomPicked1,
//								App::Old::Component::Chemistry::Atom & p_atomPicked2 ) const
//	{
//		App::Old::Application::Selection::SelectionModel & selectionModel
//			= App::Old::Application::Selection::SelectionManager::get().getSelectionModel();
//		const bool appendToSelection = _isModifierExclusive( ModifierFlag::Control );
//
//		switch ( VTX_SETTING().getSelectionGranularity() )
//		{
//		case VTX::App::Old::Application::Selection::GRANULARITY::MOLECULE:
//		{
//			App::Old::Component::Chemistry::Molecule * const mol1 = p_atomPicked1.getMoleculePtr();
//			App::Old::Component::Chemistry::Molecule * const mol2 = p_atomPicked2.getMoleculePtr();
//			if ( mol1 == mol2 )
//			{
//				VTX_ACTION(
//					new VTX::App::Old::Action::Selection::UnselectMolecule( selectionModel, *mol1, appendToSelection )
//);
//			}
//			else
//			{
//				VTX_ACTION( new VTX::App::Old::Action::Selection::UnselectMolecule(
//					selectionModel, { mol1, mol2 }, appendToSelection ) );
//			}
//			break;
//		}
//		case VTX::App::Old::Application::Selection::GRANULARITY::CHAIN:
//		{
//			App::Old::Component::Chemistry::Chain * const chain1 = p_atomPicked1.getChainPtr();
//			App::Old::Component::Chemistry::Chain * const chain2 = p_atomPicked2.getChainPtr();
//
//			if ( chain1 == chain2 )
//			{
//				VTX_ACTION(
//					new VTX::App::Old::Action::Selection::UnselectChain( selectionModel, *chain1, appendToSelection ) );
//			}
//			else
//			{
//				VTX_ACTION( new VTX::App::Old::Action::Selection::UnselectChain(
//					selectionModel, { chain1, chain2 }, appendToSelection ) );
//			}
//			break;
//		}
//		case VTX::App::Old::Application::Selection::GRANULARITY::RESIDUE:
//		{
//			App::Old::Component::Chemistry::Residue * const res1 = p_atomPicked1.getResiduePtr();
//			App::Old::Component::Chemistry::Residue * const res2 = p_atomPicked2.getResiduePtr();
//
//			if ( res1 == res2 )
//			{
//				VTX_ACTION(
//					new VTX::App::Old::Action::Selection::UnselectResidue( selectionModel, *res1, appendToSelection ) );
//			}
//			else
//			{
//				VTX_ACTION( new VTX::App::Old::Action::Selection::UnselectResidue(
//					selectionModel, { res1, res2 }, appendToSelection ) );
//			}
//		}
//		break;
//		case VTX::App::Old::Application::Selection::GRANULARITY::ATOM:
//		default:
//			if ( &p_atomPicked1 == &p_atomPicked2 )
//			{
//				VTX_ACTION(
//					new VTX::App::Old::Action::Selection::UnselectAtom( selectionModel, p_atomPicked1, appendToSelection
//)
//);
//			}
//			else
//			{
//				VTX_ACTION( new VTX::App::Old::Action::Selection::UnselectAtom(
//					selectionModel, { &p_atomPicked1, &p_atomPicked2 }, appendToSelection ) );
//			}
//			break;
//		}
//	}
//	void Picker::_unselectItem( App::Old::Component::Chemistry::Residue & p_residuePicked ) const
//	{
//		App::Old::Application::Selection::SelectionModel & selectionModel
//			= App::Old::Application::Selection::SelectionManager::get().getSelectionModel();
//		const bool appendToSelection = _isModifierExclusive( ModifierFlag::Control );
//
//		switch ( VTX_SETTING().getSelectionGranularity() )
//		{
//		case VTX::App::Old::Application::Selection::GRANULARITY::MOLECULE:
//			VTX_ACTION( new VTX::App::Old::Action::Selection::UnselectMolecule(
//				selectionModel, *p_residuePicked.getMoleculePtr(), appendToSelection ) );
//			break;
//		case VTX::App::Old::Application::Selection::GRANULARITY::CHAIN:
//			VTX_ACTION( new VTX::App::Old::Action::Selection::UnselectChain(
//				selectionModel, *p_residuePicked.getChainPtr(), appendToSelection ) );
//			break;
//		case VTX::App::Old::Application::Selection::GRANULARITY::RESIDUE:
//		case VTX::App::Old::Application::Selection::GRANULARITY::ATOM:
//		default:
//			VTX_ACTION( new VTX::App::Old::Action::Selection::UnselectResidue(
//				selectionModel, p_residuePicked, appendToSelection ) );
//			break;
//		}
//	}
//
//	void Picker::_onMouseLeftDoubleClick( const uint p_x, const uint p_y )
//	{
//		const Vec2i ids = QT_APP()->getMainWindow().getRender()->getPickedIds( p_x, p_y );
//
//		if ( ids.x == App::Old::Core::Model::ID_UNKNOWN )
//			return;
//
//		if ( _lastClickedIds.x != ids.x || _lastClickedIds.y != ids.y )
//			return;
//
//		if ( _isModifierExclusive( ModifierFlag::None ) )
//		{
//			const App::Old::Application::Selection::SelectionModel & selectionModel
//				= App::Old::Application::Selection::SelectionManager::get().getSelectionModel();
//			VTX_ACTION( new QT::Action::Selection::Orient( selectionModel ) );
//		}
//	}
//
// } // namespace VTX::UI::QT::Controller
