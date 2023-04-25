#ifndef __VTX_MODEL_BASE_MODEL__
#define __VTX_MODEL_BASE_MODEL__

#include "app/core/event/vtx_event.hpp"
#include "app/event/model.hpp"
#include "app/old_app/id.hpp"
#include <string>
#include <util/types.hpp>

namespace VTX::App::Manager
{
	class MvcManager;
}

#define VTX_MODEL friend VTX::App::Manager::MvcManager;

namespace VTX::App::Core::Model
{
	using ID				= uint;
	constexpr ID ID_UNKNOWN = 0;
	class BaseModel
	{
		VTX_MODEL

	  public:
		inline const ID &			   getId() const { return _id; }
		inline const VTX::ID::VTX_ID & getTypeId() const { return *_typeId; }
		inline bool					   isEnable() const { return _enabled; }
		inline void					   setEnable( bool p_enable )
		{
			_enabled = p_enable;
			_notifyDataChanged();
		}
		inline const std::string & getDefaultName() const { return *_name; }
		inline void				   setDefaultName( const std::string * const p_name )
		{
			_name = p_name;
			_notifyDataChanged();
		}
		inline void forceNotifyDataChanged() { _notifyDataChanged(); }

	  protected:
		inline static ID				COUNTER		 = 1;
		inline static const std::string DEFAULT_NAME = "Unknown";

		const ID					  _id	   = COUNTER++;
		bool						  _enabled = true;
		const std::string *			  _name	   = &DEFAULT_NAME;
		const VTX::ID::VTX_ID * const _typeId;

		BaseModel( const VTX::ID::VTX_ID & p_typeId ) : _typeId( &p_typeId ) {}
		~BaseModel() {}

		const std::string *& _getNamePtr() { return _name; }

		void _instantiateDefaultViews() {}

		inline void _notifyViews( const App::Core::Event::VTX_EVENT & p_eventID )
		{
			App::Core::Event::VTXEvent * const event = new App::Core::Event::VTXEvent( p_eventID );
			_notifyViews( event );
		}

		template<typename... Args>
		inline void _notifyViews( const App::Core::Event::VTX_EVENT & p_eventID, Args... p_args )
		{
			App::Core::Event::VTXEventArg<Args...> * const event
				= new App::Core::Event::VTXEventArg<Args...>( p_eventID, p_args... );
			_notifyViews( event );
		}

		void _notifyViews( const App::Core::Event::VTXEvent * const p_event );

		inline void _notifyDataChanged() { _notifyViews( VTX::App::Event::Model::DATA_CHANGE ); }
	};
} // namespace VTX::App::Core::Model
#endif
