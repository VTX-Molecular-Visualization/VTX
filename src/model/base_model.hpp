#ifndef __VTX_BASE_MODEL__
#define __VTX_BASE_MODEL__

#include "define.hpp"
#include "event/event.hpp"
#include "id.hpp"
#include <string>

namespace VTX::MVC
{
	class MvcManager;
}

#define VTX_MODEL friend VTX::MVC::MvcManager;

namespace VTX
{
	namespace Model
	{
		using ID = uint;
		class BaseModel
		{
			VTX_MODEL

		  public:
			inline const ID &		  getId() const { return _id; }
			inline const ID::VTX_ID & getTypeId() const { return *_typeId; }
			inline bool				  isEnable() const { return _enabled; }
			inline void				  setEnable( bool p_enable )
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

			const ID				 _id	  = COUNTER++;
			bool					 _enabled = true;
			const std::string *		 _name	  = &DEFAULT_NAME;
			const ID::VTX_ID * const _typeId;

			BaseModel( const ID::VTX_ID & p_typeId ) : _typeId( &p_typeId ) {}
			void		_instantiateDefaultViews() {}
			void		_notifyViews( const Event::VTXEvent * const p_event );
			inline void _notifyDataChanged() { _notifyViews( new Event::VTXEvent( Event::Model::DATA_CHANGE ) ); }
		};
	} // namespace Model
} // namespace VTX
#endif
