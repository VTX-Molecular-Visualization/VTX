#ifndef __VTX_COMPONENT_MENU__
#define __VTX_COMPONENT_MENU__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component.hpp"
#include "portable-file-dialogs/portable-file-dialogs.h"

namespace VTX
{
	namespace UI
	{
		class ComponentMenu : public BaseComponent
		{
		  public:
			explicit ComponentMenu( bool * const, bool * const, bool * const, bool * const, bool * const );

			virtual COMPONENT_NAME getName() const override { return COMPONENT_NAME::MENU; }

		  protected:
			virtual void _addComponents() override;
			virtual void _draw() override;

		  private:
			bool * const _showConsole;
			bool * const _showScene;
			bool * const _showInspector;
			bool * const _showCameraEditor;
			bool		 _showDialogImport = false;

			std::shared_ptr<pfd::open_file> _openFileDialog = nullptr;
		};
	} // namespace UI
} // namespace VTX
#endif
