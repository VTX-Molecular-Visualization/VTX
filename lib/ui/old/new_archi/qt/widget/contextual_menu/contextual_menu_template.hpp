#ifndef __VTX_UI_QT_WIDGET_CONTEXTUAL_MENU_TEMPLATE__
#define __VTX_UI_QT_WIDGET_CONTEXTUAL_MENU_TEMPLATE__

#include "ui/qt/widget/base_manual_widget.hpp"
#include <QAction>
#include <QMenu>
#include <QString>
#include <list>

namespace VTX::UI::QT::Widget::ContextualMenu
{
	class BaseContextualMenu : public BaseManualWidget<QMenu>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		enum class SECTION_POSITION_HINT : int
		{
			UNDEFINED,
			TOP,
			BOTTOM,
		};

	  public:
		QAction * addSection( const QString & p_sectionName )
		{
			return addSection( p_sectionName, SECTION_POSITION_HINT::UNDEFINED );
		}
		QAction * addSection( const QString & p_sectionName, const SECTION_POSITION_HINT & p_positionHint );

		template<class Obj, typename Func1>
		void appendToSection( const QString & p_sectionName, const QString & text, Obj * p_obj, Func1 slot )
		{
			QAction * const action = new QAction( text, this );
			connect( action, &QAction::triggered, p_obj, std::move( slot ) );

			appendToSection( p_sectionName, action );
		};

		void appendToSection( const QString & p_sectionName, QAction * const p_action );

	  protected:
		BaseContextualMenu( QWidget * p_parent = nullptr ) : BaseManualWidget( p_parent ) {}

		QAction * _findSection( const QString & p_sectionName ) const;
		QAction * _getNextSection( const SECTION_POSITION_HINT & p_positionHint ) const;
		QAction * _getNextSection( QAction * p_section ) const;
	};

	template<typename T>
	class ContextualMenuTemplate : public BaseContextualMenu
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		virtual void setTarget( T * const p_target ) { _target = p_target; }

	  protected:
		ContextualMenuTemplate( QWidget * p_parent = nullptr ) : BaseContextualMenu( p_parent ) {}

	  protected:
		T * _target = nullptr;
	};

} // namespace VTX::UI::QT::Widget::ContextualMenu

#endif
