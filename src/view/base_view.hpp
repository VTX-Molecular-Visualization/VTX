#ifndef __VTX_BASE_VIEW__
#define __VTX_BASE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "event/event.hpp"
#include "generic/base_notifiable.hpp"
#include "id.hpp"
#include <QObject>
#include <type_traits>
#include <utility>

namespace VTX
{
	namespace Model
	{
		class BaseModel;
	}

	namespace View
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseView
		{
		  public:
			explicit BaseView( T * const p_model ) : _model( p_model ) {}
			virtual ~BaseView() = default;

			virtual void notify( const Event::VTX_EVENT_MODEL & ) { std::cout << "notify super class" << std::endl; };

		  protected:
			T * _model;
		};
	} // namespace View
} // namespace VTX
#endif
