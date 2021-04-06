#ifndef __VTX_UI_WIDGET_CUSTOM_SET_REPRESENTATION_MENU__
#define __VTX_UI_WIDGET_CUSTOM_SET_REPRESENTATION_MENU__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/base_manual_widget.hpp"
#include <QAction>
#include <QActionGroup>
#include <QMenu>

namespace VTX::UI::Widget::CustomWidget
{
	class SetRepresentationMenu : public BaseManualWidget<QMenu>
	{
		Q_OBJECT

	  public:
		SetRepresentationMenu( QWidget * p_parent = nullptr ) : BaseManualWidget( p_parent ) {};
		~SetRepresentationMenu() {};

		void localize() override;

		void tickCurrentRepresentation( const int p_representationIndex ) const;
		void removeTick() const;

	  signals:
		void onRepresentationChange( const int p_representationIndex );

	  protected:
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _applyRepresentation( const int p_representationIndex );

		QAction * _generateSetRepresentationAction( const int p_representationIndex );

	  private:
		QActionGroup * _representationGroup;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
