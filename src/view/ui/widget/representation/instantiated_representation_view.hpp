#ifndef __VTX_VIEW_UI_WIDGET_INSTANTIATED_REPRESENTATION__
#define __VTX_VIEW_UI_WIDGET_INSTANTIATED_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/representation/base_representation_widget.hpp"
#include "view/base_view.hpp"

namespace VTX::View::UI::Widget::Representation
{
	class InstantiatedRepresentationView : View::BaseView<Model::Representation::InstantiatedRepresentation>
	{
		VTX_VIEW

		InstantiatedRepresentationView( Model::Representation::InstantiatedRepresentation * const p_model ) :
			View::BaseView<Model::Representation::InstantiatedRepresentation>( p_model ) {};

	  public:
		void setTarget( VTX::UI::Widget::Representation::BaseRepresentationWidget * const p_target )
		{
			_target = p_target;
		};

	  protected:
		~InstantiatedRepresentationView()
		{
			if ( _target != nullptr )
				_target->notifyInstantiatedRepresentationViewDeleted();
		}
		void _refreshView() override { _target->refresh(); };

	  private:
		VTX::UI::Widget::Representation::BaseRepresentationWidget * _target;
	};
} // namespace VTX::View::UI::Widget::Representation
#endif
