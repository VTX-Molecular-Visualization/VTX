#ifndef __VTX_UI_QT_WIDGET_CONTEXTUAL_MENU_SELECTION__
#define __VTX_UI_QT_WIDGET_CONTEXTUAL_MENU_SELECTION__

#include "qt/widget/contextual_menu/contextual_menu_template.hpp"
// #include "ui/widget/custom_widget/set_representation_menu.hpp"
// #include "ui/widget/custom_widget/trajectory_frames_menu.hpp"
#include <QHideEvent>
#include <QMenu>
#include <map>
#include <old/model/base_model.hpp>
#include <old/model/selection.hpp>
#include <unordered_set>
#include <vector>

namespace VTX::Model
{
	class Label;
}

namespace VTX::UI::QT::Widget::ContextualMenu
{
	enum class TypeMask : int
	{
		Molecule	   = 1 << 0,
		Chain		   = 1 << 1,
		Residue		   = 1 << 2,
		Atom		   = 1 << 3,
		Representation = 1 << 4,
		Viewpoint	   = 1 << 5,
		Label		   = 1 << 6,

		None = 0,
		All	 = 0xFFFF,

		MoleculeStructure = Molecule | Chain | Residue | Atom,
		AllButMolecule	  = MoleculeStructure & ~Molecule,
		AllButAtom		  = MoleculeStructure & ~Atom,
	};

	inline TypeMask operator&( const TypeMask p_lhs, const TypeMask p_rhs )
	{
		return static_cast<TypeMask>( int( p_lhs ) & int( p_rhs ) );
	}
	inline TypeMask operator&=( TypeMask & lhs, const TypeMask rhs )
	{
		lhs = lhs & rhs;
		return lhs;
	}
	inline TypeMask operator|( const TypeMask lhs, const TypeMask rhs )
	{
		return static_cast<TypeMask>( int( lhs ) | int( rhs ) );
	}
	inline TypeMask operator|=( TypeMask & lhs, const TypeMask rhs )
	{
		lhs = lhs | rhs;
		return lhs;
	}

	class ContextualMenuSelection : public ContextualMenuTemplate<Model::Selection>
	{
		NEW_ARCHI_VTX_WIDGET

	  private:
		class ItemData
		{
		  public:
			ItemData( const QString &						p_name,
					  const TypeMask						p_validTypes,
					  const ContextualMenuSelection * const p_linkedMenu ) :
				_name( p_name ),
				validTypes( p_validTypes ), _linkedMenu( p_linkedMenu ) {};

			virtual ~ItemData() {};

			virtual void appendToMenu( QMenu * const p_parent ) = 0;

			void setRefreshFunction( void ( ContextualMenuSelection::*p_refreshFunc )( QAction & ) const )
			{
				_refreshFunction = p_refreshFunc;
			}
			void setCheckFunction( bool ( ContextualMenuSelection::*p_checkFunc )() const )
			{
				_checkFunction = p_checkFunc;
			}

			virtual void refresh()
			{
				if ( _linkedMenu != nullptr && _refreshFunction != nullptr && _action != nullptr )
					( _linkedMenu->*_refreshFunction )( *_action );
			};
			virtual bool check()
			{
				if ( _linkedMenu != nullptr && _checkFunction != nullptr )
					return ( _linkedMenu->*_checkFunction )();
				return true;
			};

			void setParent( QMenu * const p_parent ) { _action->setParent( p_parent ); };
			void setVisible( const bool p_visible ) { _action->setVisible( p_visible ); };

			const TypeMask validTypes;

		  protected:
			QAction *	  _action = nullptr;
			const QString _name;

			const ContextualMenuSelection * _linkedMenu							   = nullptr;
			void ( ContextualMenuSelection::*_refreshFunction )( QAction & ) const = nullptr;
			bool ( ContextualMenuSelection::*_checkFunction )() const			   = nullptr;
		};
		class ActionData : public ItemData
		{
		  public:
			ActionData( const QString &						  p_name,
						const TypeMask						  p_validTypes,
						const ContextualMenuSelection * const p_linkedMenu,
						void ( ContextualMenuSelection::*p_action )(),
						const QKeySequence p_shortcut = QKeySequence() ) :
				ItemData( p_name, p_validTypes, p_linkedMenu ),
				action( p_action ), shortcut( p_shortcut ) {};

			ActionData() : ActionData( "-empty-", TypeMask::None, nullptr, nullptr ) {};

			void appendToMenu( QMenu * const p_parent ) override
			{
				_action = p_parent->addAction( _name, _linkedMenu, action, shortcut );
			}

			void ( ContextualMenuSelection::*action )();
			const QKeySequence shortcut;
		};
		class ActionDataSection : public ItemData
		{
		  public:
			ActionDataSection( const QString &						 p_name,
							   const TypeMask						 p_validTypes,
							   const ContextualMenuSelection * const p_linkedMenu ) :
				ItemData( p_name, p_validTypes, p_linkedMenu ) {};

			void appendToMenu( QMenu * const p_parent ) override { _action = p_parent->addSection( _name ); }
		};
		class SubMenuData : public ItemData
		{
		  public:
			SubMenuData( const QString &					   p_name,
						 const TypeMask						   p_validTypes,
						 const ContextualMenuSelection * const p_linkedMenu,
						 QMenu * const						   p_menu ) :
				ItemData( p_name, p_validTypes, p_linkedMenu ),
				_menu( p_menu )
			{
				_menu->setTitle( _name );
			};

			void appendToMenu( QMenu * const p_parent ) override { _action = p_parent->addMenu( _menu ); }

			QMenu * const _menu;
		};
		class SelectionSubMenu
		{
		  public:
			SelectionSubMenu( QWidget * const p_parent, const QString & p_name )
			{
				_groupMenu = new QMenu( p_parent );
				_groupMenu->setTitle( p_name );
			};
			~SelectionSubMenu()
			{
				for ( ItemData * action : _actions )
					delete action;
			}

			void addItemData( ItemData * const _itemData ) { _actions.emplace_back( _itemData ); };

			void refreshWithTarget( const Model::Selection & p_target, const TypeMask & p_mask, QMenu * const p_parent )
			{
				for ( ItemData * const action : _actions )
				{
					const bool actionVisible = bool( action->validTypes & p_mask ) && action->check();

					if ( actionVisible )
					{
						action->appendToMenu( p_parent );
						action->refresh();
					}
				}
			};
			QMenu * getMenu() { return _groupMenu; };

		  private:
			QMenu *					_groupMenu;
			std::vector<ItemData *> _actions = std::vector<ItemData *>();
		};

		enum class SUBMENU_TEMPLATE : int
		{
			MOLECULE_STRUCTURE,
			VIEWPOINT,
			LABEL,

			COUNT
		};

	  public:
		~ContextualMenuSelection();
		void localize() override;

		void setFocusedTarget( Model::BaseModel * const p_focusedTarget );

	  protected:
		ContextualMenuSelection( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void hideEvent( QHideEvent * p_event ) override;

		void _updateActionsWithSelection();

		void _renameAction();

		void _toggleWaterVisibilityAction();
		void _toggleHydrogenVisibilityAction();
		void _toggleSolventVisibilityAction();
		void _toggleIonVisibilityAction();

		void _toggleTrajectoryPlayingAction();

		void _orientAction();
		void _showAction();
		void _hideAction();
		void _soloAction();
		void _copyAction();
		void _copyFrameAction( const int p_frame );
		void _extractAction();
		void _deleteAction();

		void _gotoViewpointAction();
		void _relocateViewpointAction();
		void _deleteViewpointAction();

		void _deleteLabelAction();
		void _orientToLabelAction();

		void _applyRepresentationAction( const int p_representationIndex );

		void _exportAction();
		void _loadTrajectoryAction();

		// void _applyComputeRMSDAction();
		// void _applyAlignmentAction();
		// void _openAlignmentWindowAction();

	  private:
		// UI::Widget::CustomWidget::SetRepresentationMenu * _representationMenu = nullptr;
		// UI::Widget::CustomWidget::TrajectoryFramesMenu *  _frameListMenu	  = nullptr;

		std::map<ID::VTX_ID, int>		_submenusMap = std::map<ID::VTX_ID, int>();
		std::vector<SelectionSubMenu *> _submenus	 = std::vector<SelectionSubMenu *>();

		Model::BaseModel * _focusedTarget = nullptr;

		TypeMask _getTypeMaskFromTypeSet( const std::set<ID::VTX_ID> & p_typeIds );
		void	 _updateCurrentRepresentationFeedback( QAction & _action ) const;

		void _refreshToggleWaterText( QAction & _action ) const;
		void _refreshToggleHydrogenText( QAction & _action ) const;
		void _refreshToggleSolventText( QAction & _action ) const;
		void _refreshToggleIonText( QAction & _action ) const;

		void _refreshFrameListMenuItems( QAction & _action ) const;

		void _refreshToggleTrajectoryPlay( QAction & _action ) const;
		bool _checkToggleTrajectoryPlayAction() const;

		// bool _checkApplyAlignementAction() const;
		// bool _checkComputeRMSDAction() const;

		void _getAllLabelTypes( std::unordered_set<Model::Label *> & p_labels ) const;
	};

} // namespace VTX::UI::QT::Widget::ContextualMenu

#endif
