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
			template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
			static std::shared_ptr<View::BaseView<Model::BaseModel>> componentToView(
				const std::shared_ptr<UI::BaseComponent> p_component )
			{
				std::shared_ptr<UI::BaseComponent> component
					= VTXApp::get().getUIComponentByName( UI::COMPONENT_NAME::CONSOLE );

				if ( component == nullptr ) VTX_ERROR( "Component is null" );
				std::shared_ptr<View::BaseView<Model::BaseModel>> view
					= std::dynamic_pointer_cast<View::BaseView<T>>( component );
				if ( view == nullptr ) VTX_ERROR( "View is null" );
				std::shared_ptr<View::BaseView<Model::BaseModel>> viewCast
					= std::reinterpret_pointer_cast<View::BaseView<Model::BaseModel>>( view );
				if ( viewCast == nullptr ) VTX_ERROR( "View is null" );
				return viewCast;
			}

			template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
			static std::shared_ptr<UI::BaseComponent> viewToComponent( const std::shared_ptr<View::BaseView<T>> p_view )
			{
				if ( p_view == nullptr ) VTX_ERROR( "View is null" );
				/*
				std::shared_ptr<View::BaseView<Model::BaseModel>> view
					= std::reinterpret_pointer_cast<View::BaseView<Model::BaseModel>>( p_view );
				if ( view == nullptr ) VTX_ERROR( "View is null" );
				*/
				std::shared_ptr<UI::BaseComponent> component = std::dynamic_pointer_cast<UI::BaseComponent>( p_view );
				if ( component == nullptr ) VTX_ERROR( "Component is null" );
				return component;
			}

		} // namespace Type
	}	  // namespace Util
} // namespace VTX

#endif
