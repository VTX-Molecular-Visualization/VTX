#ifndef __VTX_UI_QT_MENU_REPRESENTATION__
#define __VTX_UI_QT_MENU_REPRESENTATION__

#include <QMenu>
#include <optional>

namespace VTX::UI::QT::Menu
{

	class Representation : public Widget::BaseWidget<Representation, QMenu>
	{
	  public:
		Representation( QWidget * p_parent, const std::optional<App::ECS::Entity> p_representation = std::nullopt ) :
			BaseWidget( p_parent )
		{
		}
	};

} // namespace VTX::UI::QT::Menu

#endif
