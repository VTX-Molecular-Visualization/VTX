#ifndef __VTX_COMPONENT_MENU__
#define __VTX_COMPONENT_MENU__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component.hpp"
#include "portable-file-dialogs.h"

namespace VTX
{
	namespace UI
	{
		class Menu : public BaseComponent
		{
		  public:
			virtual const std::string & getName() const override { return ID::UI::MENU; }

		  protected:
			virtual bool _drawHeader() override;
			virtual void _drawContent() override;
			virtual void _drawFooter() override;

		  private:
			bool _showDialogImport = false;

			std::shared_ptr<pfd::open_file> _openFileDialog = nullptr;
		};
	} // namespace UI
} // namespace VTX
#endif
