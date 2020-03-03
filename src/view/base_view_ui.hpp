#ifndef __VTX_BASE_VIEW_UI__
#define __VTX_BASE_VIEW_UI__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_view.hpp"
#include "model/base_model.hpp"
#include "ui/base_component.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseViewUI : public BaseView<T>, public Generic::BaseDrawable
		{
		  public:
			explicit BaseViewUI( T * const p_model ) : BaseView( p_model ) {}
			virtual ~BaseViewUI() {}

			virtual void init() override
			{
				VTXApp::get().getUI().getComponentByName( getComponentParentName() )->addItem( this );
			}

			virtual void clean() override
			{
				VTX_DEBUG( "CLEAN " + getName() );
				VTXApp::get().getUI().getComponentByName( getComponentParentName() )->removeItem( getName() );
			}

			virtual std::string getComponentParentName() const = 0;
		};
	} // namespace View
} // namespace VTX
#endif
