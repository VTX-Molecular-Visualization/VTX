#ifndef __VTX__TYPE__
#define __VTX__TYPE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/base_model.hpp"
#include "../ui/base_component.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace Util
	{
		namespace Type
		{
			using BaseViewSharedPtr = std::shared_ptr<View::BaseView<Model::BaseModel>>;

			template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
			static BaseViewSharedPtr componentToView( UI::BaseComponent::ComponentSharedPtr p_component )
			{
				if ( p_component == nullptr ) VTX_ERROR( "Component is null" );
				std::shared_ptr<View::BaseView<T>> view = std::dynamic_pointer_cast<View::BaseView<T>>( p_component );
				if ( view == nullptr ) VTX_ERROR( "View is null" );
				BaseViewSharedPtr viewCast = std::reinterpret_pointer_cast<View::BaseView<Model::BaseModel>>( view );
				if ( viewCast == nullptr ) VTX_ERROR( "View is null" );
			}

			static UI::BaseComponent::ComponentSharedPtr viewToComponent( BaseViewSharedPtr p_view )
			{
				if ( p_view == nullptr ) VTX_ERROR( "View is null" );
				BaseViewSharedPtr view = std::reinterpret_pointer_cast<View::BaseView<Model::BaseModel>>( p_view );
				if ( view == nullptr ) VTX_ERROR( "View is null" );
				UI::BaseComponent::ComponentSharedPtr component = std::dynamic_pointer_cast<UI::BaseComponent>( view );
				if ( component == nullptr ) VTX_ERROR( "Component is null" );
				return component;
			}

			static BaseViewSharedPtr viewPtrToViewSharedPtr( View::BaseView<Model::BaseModel> * p_view )
			{
				return BaseViewSharedPtr( p_view );
			}
		} // namespace Type
	}	  // namespace Util
} // namespace VTX

#endif
