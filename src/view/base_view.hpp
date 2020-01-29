#ifndef __VTX_BASE_VIEW__
#define __VTX_BASE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "../define.hpp"
#include "../event/event.hpp"
#include "../generic/base_namable.hpp"
#include <type_traits>
#include <utility>

namespace VTX
{
	namespace View
	{
		class BaseModel;

		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseView:
		{
		  public:
			BaseView() = default;
			virtual ~BaseView() { _model = nullptr; };

			virtual void setModel( T * const p_model ) final
			{
				_model = p_model;
				if ( _model != nullptr ) { _prepare(); };
			}
			virtual void notify( Event::EVENT_MODEL ) {};

		  protected:
			T * _model = nullptr;

			virtual T &	 _getModel() final { return *_model; }
			virtual void _prepare() {};
		};
	} // namespace View
} // namespace VTX
#endif
