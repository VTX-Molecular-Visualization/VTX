#ifndef __VTX_BASE_COMPONENT__
#define __VTX_BASE_COMPONENT__

#ifdef _MSC_VER
#pragma once
#endif

#include "../define.hpp"
#include "../event/event.hpp"
#include "../model/base_model.hpp"
#include "imgui/imgui.h"
#include <map>
#include <set>

namespace VTX
{
	namespace UI
	{
		enum class COMPONENT_NAME
		{
			UI,
			MENU,
			CONSOLE,
			SCENE,
			INSPECTOR,
			CAMERA_EDITOR,
			VIEW
		};

		class BaseComponent
		{
		  public:
			using MapStringToComponentSharedPtr	 = std::map<std::string, std::shared_ptr<UI::BaseComponent>>;
			using PairStringToComponentSharedPtr = std::pair<std::string, std::shared_ptr<UI::BaseComponent>>;

			BaseComponent( bool * const );
			virtual ~BaseComponent();

			virtual void display();
			virtual void init();
			virtual bool isShown() final { return *_show; }

			virtual const std::shared_ptr<UI::BaseComponent> getComponentByName( const COMPONENT_NAME ) const final;
			virtual void									 receiveEvent( const Event::EVENT_UI, void * const ) final;

			virtual COMPONENT_NAME getComponentName() const = 0;
			virtual std::string	   getName() const { return ENUM_TO_STRING( getComponentName() ); };

			virtual void addView( const std::shared_ptr<View::BaseView<Model::BaseModel>> );
			virtual void addComponent( const std::shared_ptr<UI::BaseComponent> ) final;

		  protected:
			bool * _show = nullptr;

			virtual void _addComponents() {};
			virtual void _drawComponent( const COMPONENT_NAME );
			virtual void _drawComponents() final;

			virtual void _registerEventHandler( const Event::EVENT_UI ) final;
			virtual void _registerEventHandlers() {};
			virtual void _applyEvent( const Event::EVENT_UI, void * const p_arg ) {};

			virtual void _draw() = 0;

		  private:
			bool						  _isInitialized = false;
			MapStringToComponentSharedPtr _components	 = MapStringToComponentSharedPtr();
			std::set<Event::EVENT_UI>	  _events		 = std::set<Event::EVENT_UI>();
		};
	} // namespace UI
} // namespace VTX
#endif
