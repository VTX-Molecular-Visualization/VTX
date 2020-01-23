#ifndef __VTX_BASE_VIEW_UI__
#define __VTX_BASE_VIEW_UI__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/base_model.hpp"
#include "../ui/base_component.hpp"
#include "base_view.hpp"

namespace VTX
{
	namespace View
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseViewUI : public UI::BaseComponent, public BaseView<T>
		{
		  public:
			explicit BaseViewUI( bool * const p_show ) : BaseComponent( p_show ) {}

			virtual UI::COMPONENT_NAME getComponentName() const override { return UI::COMPONENT_NAME::VIEW; }
			virtual std::string		   getName() const override
			{
				return ENUM_TO_STRING( getViewName() ) + "_" + std::to_string( _model->getId() );
			}

			virtual void display() override
			{
				if ( _show != nullptr && isShown() == false ) { return; }
				if ( _model == nullptr )
				{
					VTX_DEBUG( "Trying to display a view without model" );
					return;
				}
				_draw();
			};

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
