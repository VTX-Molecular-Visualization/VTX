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
				ProgressBar();

				virtual const std::string & getName() const override { return ID::UI::PROGRESS_BAR; }
				virtual const char *		getTitle() const override { return LOCALE( "Modal.Loading" ); }
				virtual void				receiveEvent( const Event::VTXEvent & p_event ) override;

			  protected:
				virtual bool _drawHeader() override;
				virtual void _drawContent() override;
				virtual void _drawFooter() override;

			  private:
				float _value = 0.f;
			};
		} // namespace Modal
	}	  // namespace UI
} // namespace VTX
#endif
