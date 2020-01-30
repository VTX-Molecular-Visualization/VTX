#ifndef __VTX_BASE_VIEW__
#define __VTX_BASE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "../define.hpp"
#include "../event/event.hpp"
#include "../generic/base_drawable.hpp"
#include "../generic/base_namable.hpp"
#include "../id.hpp"
#include <type_traits>
#include <utility>

namespace VTX
{
	namespace View
	{
		class BaseModel;

		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseView : public Generic::BaseDrawable
		{
		  public:
			BaseView( T * const p_model ) : _model( p_model ) {}
			// virtual void setModel( T * const p_model ) final { _model = p_model; }
			virtual void notify( Event::EVENT_MODEL ) {};

		  protected:
			virtual T & _getModel() final { return *_model; }

		  private:
			T * _model = nullptr;
		};
	} // namespace View
} // namespace VTX
#endif
