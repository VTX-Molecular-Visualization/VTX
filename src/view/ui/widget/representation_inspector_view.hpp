#ifndef __VTX_VIEW_UI_WIDGET_REPRESENTATION_INSPECTOR_VIEW__
#define __VTX_VIEW_UI_WIDGET_REPRESENTATION_INSPECTOR_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_widget_view.hpp"
#include "model/representation/representation.hpp"
#include "ui/widget/inspector/inspector_representation_widget.hpp"
#include "ui/widget_factory.hpp"
#include <QWidget>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class RepresentationInspectorView :
					public BaseWidgetView<Model::Representation::InstantiatedRepresentation, VTX::UI::Widget::Inspector::InspectorRepresentationWidget>
				{
					VTX_VIEW

				  protected:
					RepresentationInspectorView( Model::Representation::InstantiatedRepresentation * const p_molecule, QWidget * p_parent ) :
						BaseWidgetView( p_molecule, "representationInspector", p_parent ) {};
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
