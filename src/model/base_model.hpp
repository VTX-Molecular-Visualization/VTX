#ifndef __VTX_BASE_MODEL__
#define __VTX_BASE_MODEL__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "id.hpp"
#include "selection/base_selectable.hpp"
#include <string>

namespace VTX
{
	namespace Model
	{
		using Model_ID = ulong;

		class BaseModel : public Selection::BaseSelectable
		{
		  public:
			BaseModel( const ID::VTX_ID & p_typeId ) : _typeId( &p_typeId ) {};

			inline static Model_ID COUNTER = 0;

			const Model_ID & getId() const { return _id; };
			void			 instantiateDefaultViews() {};

			const VTX::ID::VTX_ID & getTypeId() const { return *_typeId; };

			bool isEnable() const { return _enabled; };
			void setEnable( bool p_enable )
			{
				_enabled = p_enable;
				_notifyDataChanged();
			};

			const std::string & getDefaultName() const { return *_name; };
			void				setDefaultName( const std::string * const p_name )
			{
				_name = p_name;
				_notifyDataChanged();
			};

		  protected:
			inline static const std::string DEFAULT_NAME = "<unknown>";

			const Model_ID				  _id	   = COUNTER++;
			bool						  _enabled = true;
			const std::string *			  _name	   = &DEFAULT_NAME;
			const VTX::ID::VTX_ID * const _typeId;

			void		_notifyViews( const Event::VTX_EVENT_MODEL & p_event );
			inline void _notifyDataChanged() { _notifyViews( Event::VTX_EVENT_MODEL::DATA_CHANGE ); }
		};
	} // namespace Model
} // namespace VTX
#endif
