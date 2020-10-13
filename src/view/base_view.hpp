#ifndef __VTX_BASE_VIEW__
#define __VTX_BASE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "event/event.hpp"
#include "generic/base_notifiable.hpp"
#include "id.hpp"
#include "model/base_model.hpp"
#include <utility>

namespace VTX
{
	namespace View
	{
		class BaseView
		{
		  public:
			explicit BaseView( VTX::Model::BaseModel * const p_model ) {};
			virtual ~BaseView() = default;

			virtual void notify( const Event::VTX_EVENT_MODEL & ) { /*std::cout << "notify super class" << std::endl;*/ };

		  protected:
		};
	} // namespace View
} // namespace VTX
#endif
