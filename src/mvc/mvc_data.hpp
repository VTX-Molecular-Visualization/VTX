#ifndef __VTX_MVC_DATA__
#define __VTX_MVC_DATA__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/has_collection.hpp"
#include "id.hpp"
#include "model/base_model.hpp"
#include "view/base_view.hpp"

namespace VTX
{
	namespace MVC
	{
		class MvcData
		{
		  public:
			MvcData( const Model::BaseModel * p_model )
			{
				_model = p_model;
				_views = Generic::HasCollection<View::BaseView>();
			};

			~MvcData()
			{
				_views.clear();
			};

			const Model::Model_ID getId() const { return _model->getId(); }

			inline void				addView( const ID::VTX_ID & p_id, View::BaseView * p_view ) { _views.addItem( p_id, p_view ); };
			inline View::BaseView * removeView( const ID::VTX_ID & p_id ) { return _views.removeItem( p_id ); };
			inline bool				hasView( const ID::VTX_ID & p_id ) { return _views.getItem( p_id ); };

			void notifyViews( const Event::VTX_EVENT_MODEL & p_event )
			{
				Generic::HasCollection<View::BaseView>::MapStringToItemPtr mapViews = *( _views.getItems() );

				for ( Generic::HasCollection<View::BaseView>::PairStringToItemPtr & pair : mapViews )
					pair.second->notify( p_event );
			};

		  private:
			const Model::BaseModel *			   _model;
			Generic::HasCollection<View::BaseView> _views;
		};
	} // namespace MVC
} // namespace VTX

#endif
