#ifndef __VTX_UI_OBJECT_BACKGROUND_DEPENDENT_ICON__
#define __VTX_UI_OBJECT_BACKGROUND_DEPENDENT_ICON__

#include <QIcon>
#include <QImage>
#include <util/color/rgba.hpp>

namespace VTX::UI::Object
{
	class BackgroundDependentIcon
	{
	  public:
		BackgroundDependentIcon();
		BackgroundDependentIcon( const QImage & p_image, bool p_isImageLight = true );

		void generateIcons( const QImage & p_image, bool p_isImageLight = true );

		const QIcon & getIcon() const;
		const QIcon & getIcon( const Util::Color::Rgba & p_backgroundColor ) const;

	  private:
		QIcon _lightIcon;
		QIcon _shadeIcon;
	};
} // namespace VTX::UI::Object
#endif
