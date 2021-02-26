#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_SELECTION__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_SELECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "contextual_menu_template.hpp"
#include "model/selection.hpp"
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

		AllButAtom = ~Atom,
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
		struct ActionData
		{
		  public:
			ActionData( const QString & p_name,
						const TypeMask	p_validTypes,
						void ( ContextualMenuSelection::*p_action )(),
						const QKeySequence p_shortcut = QKeySequence() ) :
				name( p_name ),
				validTypes( p_validTypes ), action( p_action ), shortcut( p_shortcut ) {};
			ActionData() : ActionData( "-empty-", TypeMask::None, nullptr ) {};

			const QString name;
			void ( ContextualMenuSelection::*action )();
			const TypeMask	   validTypes;
			const QKeySequence shortcut;
		};

	  public:
		~ContextualMenuSelection();
		void localize() override;

	  protected:
		ContextualMenuSelection( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;

		void _setupSlots() override;
		void _updateActionsWithSelection();

		void _orientAction();
		void _showAction();
		void _hideAction();
		void _copyAction();
		void _extractAction();
		void _deleteAction();

	  private:
		std::vector<ActionData> _actions = std::vector<ActionData>();
		TypeMask				_getTypeMaskFromTypeSet( const std::set<ID::VTX_ID> & p_typeIds );
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
