#ifndef __VTX_BASE_VIEW__
#define __VTX_BASE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "../defines.hpp"
#include "../event/event.hpp"
#include <type_traits>
#include <utility>

namespace VTX
{
	namespace View
	{
		enum class VIEW_NAME : int
		{
			UI_MOLECULE_STRUCTURE,
			UI_MOLECULE_TRANSFORM,
			UI_CHAIN,
			UI_RESIDUE,
			UI_ATOM,
			D3_CYLINDER,
			D3_SPHERE
		};

		class BaseModel;

		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseView
		{
		  public:
			BaseView() = default;
			virtual ~BaseView() { _model = nullptr; };

			virtual VIEW_NAME getViewName() const = 0;
			virtual void	  setModel( T * const p_model ) final
			{
				VTX_DEBUG( "SET MODEL" );
				_model = p_model;
				if ( _model != nullptr )
				{
					VTX_DEBUG( "SET MODEL OK" );
					_prepare();
				};
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
