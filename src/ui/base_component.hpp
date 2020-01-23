#ifndef __VTX_BASE_COMPONENT__
#define __VTX_BASE_COMPONENT__

#ifdef _MSC_VER
#pragma once
#endif

#include "../defines.hpp"
#include "../event/event.hpp"
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

		using ComponentSharedPtr			 = std::shared_ptr<BaseComponent>;
		using MapStringToComponentSharedPtr	 = std::map<const std::string, ComponentSharedPtr>;
		using PairStringToComponentSharedPtr = std::pair<const std::string, ComponentSharedPtr>;

		class BaseComponent
		{
		  public:
			BaseComponent( bool * const );
			virtual ~BaseComponent();

			virtual void display();
			virtual void init();
			virtual bool isShown() final { return *_show; }

			virtual const ComponentSharedPtr getComponentByName( const COMPONENT_NAME ) const final;
			virtual void					 receiveEvent( const Event::EVENT_UI, void * const ) final;

			virtual void		   addComponent( const ComponentSharedPtr ) final;
			virtual COMPONENT_NAME getComponentName() const = 0;
			virtual std::string	   getName() const { return ENUM_TO_STRING( getComponentName() ); };

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
