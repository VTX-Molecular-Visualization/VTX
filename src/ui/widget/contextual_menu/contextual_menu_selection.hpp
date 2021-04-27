#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_SELECTION__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_SELECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "contextual_menu_template.hpp"
#include "model/selection.hpp"
#include "ui/widget/custom_widget/set_representation_menu.hpp"
#include <QMenu>
#include <vector>

namespace VTX::UI::Widget::ContextualMenu
{
	enum class TypeMask : int
	{
		Molecule	   = 1 << 0,
		Chain		   = 1 << 1,
		Residue		   = 1 << 2,
		Atom		   = 1 << 3,
		Representation = 1 << 4,

		None = 0,
		All	 = 0xFFFF,

		AllButMolecule = ~Molecule,
		AllButAtom	   = ~Atom,
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
		VTX_WIDGET

	  private:
		class ItemData
		{
		  public:
			ItemData( const QString & p_name, const TypeMask p_validTypes ) :
				_name( p_name ), validTypes( p_validTypes ) {};
			virtual ~ItemData() {};

			virtual void appendToMenu( ContextualMenuSelection * const p_menu ) = 0;

			void setRefreshFunction( const ContextualMenuSelection * const p_caller,
									 void ( ContextualMenuSelection::*p_refreshFunc )( QAction & ) const )
			{
				_refreshFunctionCaller = p_caller;
				_refreshFunction	   = p_refreshFunc;
			}

			virtual void refresh()
			{
				if ( _refreshFunctionCaller != nullptr && _action != nullptr )
					( _refreshFunctionCaller->*_refreshFunction )( *_action );
			};

			const TypeMask validTypes;

		  protected:
			QAction *	  _action = nullptr;
			const QString _name;

			const ContextualMenuSelection * _refreshFunctionCaller				   = nullptr;
			void ( ContextualMenuSelection::*_refreshFunction )( QAction & ) const = nullptr;
		};
		class ActionData : public ItemData
		{
		  public:
			ActionData( const QString & p_name,
						const TypeMask	p_validTypes,
						void ( ContextualMenuSelection::*p_action )(),
						const QKeySequence p_shortcut = QKeySequence() ) :
				ItemData( p_name, p_validTypes ),
				action( p_action ), shortcut( p_shortcut ) {};

			ActionData() : ActionData( "-empty-", TypeMask::None, nullptr ) {};

			void appendToMenu( ContextualMenuSelection * const p_menu ) override
			{
				_action = p_menu->addAction( _name, p_menu, action, shortcut );
			}

			void ( ContextualMenuSelection::*action )();
			const QKeySequence shortcut;
		};
		class ActionDataSection : public ItemData
		{
		  public:
			ActionDataSection( const QString & p_name, const TypeMask p_validTypes ) :
				ItemData( p_name, p_validTypes ) {};
			void appendToMenu( ContextualMenuSelection * const p_menu ) override
			{
				_action = p_menu->addSection( _name );
			}
		};
		class SubMenuData : public ItemData
		{
		  public:
			SubMenuData( const QString & p_name, const TypeMask p_validTypes, QMenu * const p_menu ) :
				ItemData( p_name, p_validTypes ), _menu( p_menu )
			{
				_menu->setTitle( _name );
			};

			void appendToMenu( ContextualMenuSelection * const p_menu ) override { _action = p_menu->addMenu( _menu ); }

			QMenu * const _menu;
		};

	  public:
		~ContextualMenuSelection();
		void localize() override;

	  protected:
		ContextualMenuSelection( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;

		void _setupSlots() override;
		void _updateActionsWithSelection();

		void _renameAction();

		void _toggleWaterVisibilityAction();
		void _toggleSolventVisibilityAction();
		void _toggleHydrogenVisibilityAction();

		void _orientAction();
		void _showAction();
		void _hideAction();
		void _copyAction();
		void _extractAction();
		void _deleteAction();

		void _applyRepresentationAction( const int p_representationIndex );

	  private:
		std::vector<ItemData *>				  _actions = std::vector<ItemData *>();
		CustomWidget::SetRepresentationMenu * _representationMenu;

		TypeMask _getTypeMaskFromTypeSet( const std::set<ID::VTX_ID> & p_typeIds );
		void	 _updateCurrentRepresentationFeedback();

		void _refreshToggleWaterText( QAction & _action ) const;
		void _refreshToggleSolventText( QAction & _action ) const;
		void _refreshToggleHydrogenText( QAction & _action ) const;
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
