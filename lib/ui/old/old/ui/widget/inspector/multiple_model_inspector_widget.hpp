#ifndef __VTX_UI_WIDGET_MULTIPLE_MODEL_INSPECTOR__
#define __VTX_UI_WIDGET_MULTIPLE_MODEL_INSPECTOR__

#include "inspector_item_widget.hpp"
#include "inspector_section_flag.hpp"
#include <QWidget>
#include <app/old/core/event/vtx_event.hpp>
#include <app/old/mvc.hpp>
#include <app/old/event/global.hpp>
#include <app/old/event/model.hpp>
#include <app/old/core/view/callback_view.hpp>
#include <type_traits>
#include <unordered_set>

namespace VTX::UI::Widget::Inspector
{
	template<typename T, typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, T>::value>>
	class MultipleModelInspectorWidget : public InspectorItemWidget
	{
		using InspectorView = App::Old::Core::View::CallbackView<T, MultipleModelInspectorWidget<T>>;

	  public:
		virtual void receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event ) override
		{
			if ( p_event.name == VTX::App::Old::Event::Global::LATE_UPDATE )
				_endOfFrameEvent();
		}
		void refresh( const SectionFlag & p_flag = SectionFlag::ALL ) override final { _sectionToRefresh |= p_flag; }

		virtual void clearTargets() override
		{
			if ( _targets.size() > 0 )
			{
				for ( T * const target : _targets )
				{
					if ( VTX::MVC_MANAGER().hasView( target, _callbackViewId ) )
						VTX::MVC_MANAGER().deleteView( target, _callbackViewId );
				}
				_targets.clear();

				_clearFlag = true;
			}
		}

		virtual void addTarget( T * const p_target )
		{
			_targets.emplace( p_target );

			InspectorView * const view
				= VTX::MVC_MANAGER().instantiateView<InspectorView>( p_target, _callbackViewId );
			view->setCallback( this, &MultipleModelInspectorWidget<T>::_onTargetChangeEvent );

			_sectionToRefresh = SectionFlag::ALL;
		}
		virtual void removeTarget( T * const p_target )
		{
			_targets.erase( p_target );

			if ( VTX::MVC_MANAGER().hasView( p_target, _callbackViewId ) )
				VTX::MVC_MANAGER().deleteView( p_target, _callbackViewId );

			_sectionToRefresh = SectionFlag::ALL;
		}

		bool hasTarget( T * const p_target ) const { return _targets.find( p_target ) != _targets.end(); }

		const std::unordered_set<T *> & getTargets() const { return _targets; }
		std::unordered_set<T *> &		getTargets() { return _targets; }

	  protected:
		MultipleModelInspectorWidget( QWidget * p_parent, const App::Old::VTX_ID & p_callbackViewId ) :
			InspectorItemWidget( p_parent ), _callbackViewId( p_callbackViewId )
		{
			_registerEvent( VTX::App::Old::Event::Global::LATE_UPDATE );
		}

		virtual void _resetFieldStates( const SectionFlag & p_section ) = 0;
		virtual void _onTargetChangeEvent( const T * const								 p_target,
										   const VTX::App::Old::Core::Event::VTXEvent * const p_event )
		{
			if ( p_event->name == VTX::App::Old::Event::Model::DISPLAY_NAME_CHANGE )
			{
				_sectionToRefresh |= SectionFlag::INFOS;
			}
			else if ( p_event->name == VTX::App::Old::Event::Model::DATA_CHANGE )
			{
				_sectionToRefresh |= SectionFlag::INFOS;
			}
			else if ( p_event->name == VTX::App::Old::Event::Model::TRANSFORM_CHANGE )
			{
				_sectionToRefresh |= SectionFlag::TRANSFORM;
			}
			else if ( p_event->name == VTX::App::Old::Event::Model::COLOR_CHANGE )
			{
				_sectionToRefresh |= SectionFlag::INFOS;
			}
			else if ( p_event->name == VTX::App::Old::Event::Model::REPRESENTATION_CHANGE )
			{
				_sectionToRefresh |= SectionFlag::REPRESENTATION;
			}
			else if ( p_event->name == VTX::App::Old::Event::Model::TRAJECTORY_DATA_CHANGE )
			{
				_sectionToRefresh |= SectionFlag::TRAJECTORY;
			}
			else if ( p_event->name == VTX::App::Old::Event::Model::TRAJECTORY_FRAME_CHANGE )
			{
				_sectionToRefresh |= SectionFlag::TRAJECTORY_TIMER;
			}
			else if ( p_event->name == VTX::App::Old::Event::Model::AUTO_ROTATE_DATA_CHANGE )
			{
				_sectionToRefresh |= SectionFlag::AUTO_ROTATE;
			}
		}

		virtual void _endOfFrameEvent()
		{
			if ( _clearFlag )
			{
				_resetFieldStates( SectionFlag::ALL );
				_sectionToRefresh = SectionFlag::ALL;

				_clearFlag = false;
			}

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
		bool			 _clearFlag		   = false;
		const App::Old::VTX_ID _callbackViewId;
	};

} // namespace VTX::UI::Widget::Inspector

#endif