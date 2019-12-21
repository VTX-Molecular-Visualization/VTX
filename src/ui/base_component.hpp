#ifndef __VTX_BASE_COMPONENT__
#define __VTX_BASE_COMPONENT__

#ifdef _MSC_VER
#pragma once
#endif

#include "../defines.hpp"
#include "../event/event.hpp"
#include "imgui/imgui.h"
#include <magic_enum.hpp>
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
			VIEW_MOLECULE,
			VIEW_CHAIN,
			VIEW_RESIDUE,
			VIEW_ATOM
		};

		class BaseComponent
		{
		  public:
			using ComponentSharedPtr		   = std::shared_ptr<BaseComponent>;
			using MapEnumToComponentSharedPtr  = std::map<COMPONENT_NAME, ComponentSharedPtr>;
			using PairEnumToComponentSharedPtr = std::pair<COMPONENT_NAME, ComponentSharedPtr>;

			BaseComponent( bool * const );
			virtual ~BaseComponent();

			virtual void display();
			virtual void init();
			virtual bool isShown() final { return *_show; }

			virtual const ComponentSharedPtr getComponentByName( const COMPONENT_NAME ) const final;
			virtual void					 receiveEvent( const Event::EVENT_UI, void * const ) final;

			virtual COMPONENT_NAME getName() const = 0;

		  protected:
			bool * _show = nullptr;

			virtual void _addComponent( const ComponentSharedPtr ) final;
			virtual void _addComponents() {};
			virtual void _drawComponent( const COMPONENT_NAME );
			virtual void _drawComponents() final;

			virtual void _registerEventHandler( const Event::EVENT_UI ) final;
			virtual void _registerEventHandlers() {};
			virtual void _applyEvent( const Event::EVENT_UI, void * const p_arg ) {};

			virtual void _draw() = 0;

		  private:
			bool						_isInitialized = false;
			MapEnumToComponentSharedPtr _components	   = MapEnumToComponentSharedPtr();
			std::set<Event::EVENT_UI>	_events		   = std::set<Event::EVENT_UI>();
		};
	} // namespace UI
} // namespace VTX
#endif
