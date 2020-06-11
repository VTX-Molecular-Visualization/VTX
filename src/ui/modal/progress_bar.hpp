#ifndef __VTX_COMPONENT_MODAL_PROGRESS_BAR__
#define __VTX_COMPONENT_MODAL_PROGRESS_BAR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component_modal.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Modal
		{
			class ProgressBar : public BaseComponentModal
			{
			  public:
				virtual const std::string & getName() const override { return ID::UI::PROGRESS_BAR; }
				virtual const char *		getTitle() const override { return LOCALE( "Modal.Loading" ); }
				virtual void				receiveEvent( const Event::VTXEvent & p_event ) override;

			  protected:
				virtual bool _drawHeader() override;
				virtual void _drawContent() override;
				virtual void _drawFooter() override;

				virtual std::vector<Event::VTX_EVENT> _getEvents() const override
				{
					return std::vector<Event::VTX_EVENT>( { Event::Global::UPDATE_PROGRESS_BAR } );
				}

			  private:
				float _value	= 0.f;
				bool  _isOpened = false;
			};
		} // namespace Modal
	}	  // namespace UI
} // namespace VTX
#endif
