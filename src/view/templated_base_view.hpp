#ifndef __VTX_BASE_VIEW_TEMPLATED__
#define __VTX_BASE_VIEW_TEMPLATED__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_view.hpp"
#include <type_traits>

namespace VTX
{
	namespace Model
	{
		class BaseModel;
	}

	namespace View
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class TemplatedBaseView : public BaseView
		{
		  public:
			explicit TemplatedBaseView( T * const p_model ) : BaseView( p_model ) { _model = p_model; };

		  protected:
			T * _model;
		};
	} // namespace View
} // namespace VTX

#endif
