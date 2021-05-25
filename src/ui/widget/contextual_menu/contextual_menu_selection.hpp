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
			ItemData( const QString &						p_name,
					  const TypeMask						p_validTypes,
					  const ContextualMenuSelection * const p_linkedMenu ) :
				_name( p_name ),
				validTypes( p_validTypes ), _linkedMenu( p_linkedMenu ) {};
			virtual ~ItemData() {};

			virtual void appendToMenu( ContextualMenuSelection * const p_menu ) = 0;

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
			ActionDataSection( const QString &						 p_name,
							   const TypeMask						 p_validTypes,
							   const ContextualMenuSelection * const p_linkedMenu ) :
				ItemData( p_name, p_validTypes, p_linkedMenu ) {};
			void appendToMenu( ContextualMenuSelection * const p_menu ) override
			{
				_action = p_menu->addSection( _name );
			}
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

		void _toggleTrajectoryPlayingAction();

		void _orientAction();
		void _showAction();
		void _hideAction();
		void _copyAction();
		void _extractAction();
		void _deleteAction();

		void _applyRepresentationAction( const int p_representationIndex );

		void _exportAction();

	  private:
		std::vector<ItemData *>				  _actions = std::vector<ItemData *>();
		CustomWidget::SetRepresentationMenu * _representationMenu;

		TypeMask _getTypeMaskFromTypeSet( const std::set<ID::VTX_ID> & p_typeIds );
		void	 _updateCurrentRepresentationFeedback();

		void _refreshToggleWaterText( QAction & _action ) const;
		void _refreshToggleSolventText( QAction & _action ) const;
		void _refreshToggleHydrogenText( QAction & _action ) const;

		void _refreshToggleTrajectoryPlay( QAction & _action ) const;
		bool _checkToggleTrajectoryPlayAction() const;
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
