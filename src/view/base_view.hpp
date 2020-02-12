#ifndef __VTX_BASE_VIEW__
#define __VTX_BASE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "event/event.hpp"
#include "generic/base_collectionable.hpp"
#include "generic/base_notifiable.hpp"
#include "id.hpp"
#include <type_traits>
#include <utility>

namespace VTX
{
	namespace View
	{
		class BaseModel;

		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseView : virtual public Generic::BaseCollectionable, public Generic::BaseNotifiable<Event::EVENT_MODEL>
		{
		  public:
			explicit BaseView( T * const p_model ) : _model( p_model ) {}
			virtual ~BaseView() {}

			virtual void notify( const Event::EVENT_MODEL & ) override {};

		  protected:
			virtual T & _getModel() final { return *_model; }

		  private:
			T * _model = nullptr;
		};
	} // namespace View
} // namespace VTX
#endif
