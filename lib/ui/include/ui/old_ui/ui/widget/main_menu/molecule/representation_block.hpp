#ifndef __VTX_UI_WIDGET_MENU_MOLECULE_REPRESENTATION_BLOCK__
#define __VTX_UI_WIDGET_MENU_MOLECULE_REPRESENTATION_BLOCK__

#include "ui/old_ui/ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/old_ui/ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include "representation_preset_button.hpp"
#include <QWidget>
#include <app/model/representation/representation_library.hpp>
#include <app/view/base_view.hpp>
#include <map>

namespace VTX::UI::Widget::MainMenu::Molecule
{
	class RepresentationBlock :
		public VTX::View::BaseView<Model::Representation::RepresentationLibrary>,
		public MenuToolBlockWidget
	{
		VTX_WIDGET
		VTX_VIEW

	  public:
		void localize() override;

	  protected:
		RepresentationBlock( Model::Representation::RepresentationLibrary * const _representationLibrary,
							 QWidget *											  p_parent );
		~RepresentationBlock();
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshNames();
		void _refreshIcons();
		void _refreshView() override;

		void notify( const VTX::Event::VTXEvent * const p_event ) override;

	  private:
		std::map<int, RepresentationPresetButton *> _buttons   = std::map<int, RepresentationPresetButton *>();
		MenuToolButtonWidget *						_addPreset = nullptr;

		void _instantiateUI();
		void _addPresetAction();
	};
} // namespace VTX::UI::Widget::MainMenu::Molecule
#endif
