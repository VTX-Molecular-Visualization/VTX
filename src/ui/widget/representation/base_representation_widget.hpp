#ifndef __VTX_UI_WIDGET_BASE_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_BASE_REPRESENTATION_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/representation/instantiated_representation.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QWidget>
#include <type_traits>

namespace VTX::UI::Widget::Representation
{
	class BaseRepresentationWidget : public VTX::UI::Widget::BaseManualWidget<QWidget>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		BaseRepresentationWidget( QWidget * p_parent = nullptr ) : BaseManualWidget( p_parent ) {};
		void setRepresentation( Model::Representation::InstantiatedRepresentation * const p_representation )
		{
			_instantiatedRepresentation = p_representation;
			refresh();
		}

		virtual void refresh() {};

	  signals:
		void onDataChange();

	  protected:
		Model::Representation::InstantiatedRepresentation * _instantiatedRepresentation = nullptr;
	};
} // namespace VTX::UI::Widget::Representation
#endif
