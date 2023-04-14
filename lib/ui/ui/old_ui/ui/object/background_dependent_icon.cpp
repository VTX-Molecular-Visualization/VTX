#include "background_dependent_icon.hpp"

namespace VTX::UI::Object
{
	BackgroundDependentIcon::BackgroundDependentIcon() {}

	BackgroundDependentIcon::BackgroundDependentIcon( const QImage & p_image, bool p_isImageLight )
	{
		generateIcons( p_image, p_isImageLight );
	}

	const QIcon & BackgroundDependentIcon::getIcon() const { return _lightIcon; }

	const QIcon & BackgroundDependentIcon::getIcon( const Color::Rgba & p_backgroundColor ) const
	{
		return p_backgroundColor.brightness() < 0.5f ? _lightIcon : _shadeIcon;
	}

	void BackgroundDependentIcon::generateIcons( const QImage & p_image, bool p_isImageLight )
	{
		QImage invertedImage = p_image;
		invertedImage.invertPixels( QImage::InvertMode::InvertRgb );

		const QPixmap pixmap		 = QPixmap::fromImage( p_image );
		const QPixmap invertedPixmap = QPixmap::fromImage( invertedImage );

		const QIcon icon		 = QIcon( pixmap );
		const QIcon invertedIcon = QIcon( invertedPixmap );

		_lightIcon = p_isImageLight ? icon : invertedIcon;
		_shadeIcon = p_isImageLight ? invertedIcon : icon;
	}
} // namespace VTX::UI::Object
