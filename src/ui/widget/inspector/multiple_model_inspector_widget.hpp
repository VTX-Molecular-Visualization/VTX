#ifndef __VTX_UI_WIDGET_MULTIPLE_OBJECT_INSPECTOR__
#define __VTX_UI_WIDGET_MULTIPLE_OBJECT_INSPECTOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "inspector_item_widget.hpp"
#include "inspector_section_flag.hpp"
#include "mvc/mvc_manager.hpp"
#include "view/callback_view.hpp"
#include <QWidget>
#include <type_traits>
#include <unordered_set>

namespace VTX::UI::Widget::Inspector
{
	template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
	class MultipleModelInspectorWidget : public InspectorItemWidget
	{
		using InspectorView = View::CallbackView<T, MultipleModelInspectorWidget<T>>;

	  public:
		virtual void receiveEvent( const Event::VTXEvent & p_event ) override
		{
			if ( p_event.name == Event::Global::LATE_UPDATE )
				_endOfFrameEvent();
		}
		void refresh( const SectionFlag & p_flag = SectionFlag::ALL ) override final
		{
			_sectionToRefresh = SectionFlag( _sectionToRefresh | p_flag );
		}

		void clearTargets()
		{
			if ( _targets.size() > 0 )
			{
				_resetFieldStates( SectionFlag::ALL );

				for ( T * const target : _targets )
				{
					if ( MVC::MvcManager::get().hasView( target, _callbackViewId ) )
						MVC::MvcManager::get().deleteView( target, _callbackViewId );
				}
				_targets.clear();

				_sectionToRefresh = SectionFlag::ALL;
			}
		}

		void addTarget( T * const p_target )
		{
			_targets.emplace( p_target );

			InspectorView * const view
				= MVC::MvcManager::get().instantiateView<InspectorView>( p_target, _callbackViewId );
			view->setCallback( this, &MultipleModelInspectorWidget<T>::_onTargetChangeEvent );

			_sectionToRefresh = SectionFlag::ALL;
		}
		void removeTarget( T * const p_target )
		{
			_targets.erase( p_target );

			if ( MVC::MvcManager::get().hasView( p_target, _callbackViewId ) )
				MVC::MvcManager::get().deleteView( p_target, _callbackViewId );

			_sectionToRefresh = SectionFlag::ALL;
		}

	  protected:
		MultipleModelInspectorWidget( QWidget * p_parent, const ID::VTX_ID & p_callbackViewId ) :
			InspectorItemWidget( p_parent ), _callbackViewId( p_callbackViewId )
		{
			_registerEvent( Event::Global::LATE_UPDATE );
		}

		const std::unordered_set<T *> & _getTargets() const { return _targets; }
		std::unordered_set<T *> &		_getTargets() { return _targets; }

		virtual void _resetFieldStates( const SectionFlag & p_section ) = 0;
		virtual void _onTargetChangeEvent( const T * const p_target, const Event::VTXEvent * const p_event )
		{
			if ( p_event->name == Event::Model::DATA_CHANGE )
			{
				_sectionToRefresh = SectionFlag( _sectionToRefresh | SectionFlag::INFOS );
			}
			else if ( p_event->name == Event::Model::TRANSFORM_CHANGE )
			{
				_sectionToRefresh = SectionFlag( _sectionToRefresh | SectionFlag::TRANSFORM );
			}
			else if ( p_event->name == Event::Model::REPRESENTATION_CHANGE )
			{
				_sectionToRefresh = SectionFlag( _sectionToRefresh | SectionFlag::REPRESENTATION );
			}
			else if ( p_event->name == Event::Model::TRAJECTORY_DATA_CHANGE )
			{
				_sectionToRefresh = SectionFlag( _sectionToRefresh | SectionFlag::TRAJECTORY );
			}
			else if ( p_event->name == Event::Model::TRAJECTORY_FRAME_CHANGE )
			{
				_sectionToRefresh = SectionFlag( _sectionToRefresh | SectionFlag::TRAJECTORY_TIMER );
			}
		}

		virtual void _endOfFrameEvent()
		{
			if ( _sectionToRefresh != SectionFlag::NONE )
			{
				_endOfFrameRefresh( _sectionToRefresh );
				_sectionToRefresh = SectionFlag::NONE;
			}
		}
		virtual void _endOfFrameRefresh( const SectionFlag & p_flag = SectionFlag::ALL )
		{
			InspectorItemWidget::refresh( p_flag );
		}

	  private:
		std::unordered_set<T *> _targets = std::unordered_set<T *>();

		SectionFlag		 _sectionToRefresh = SectionFlag::NONE;
		const ID::VTX_ID _callbackViewId;
	};

} // namespace VTX::UI::Widget::Inspector

#endif
