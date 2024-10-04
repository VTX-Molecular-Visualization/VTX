#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "util/image.hpp"
// #include <stb_image.h>
#include <stb_image_write.h>

namespace VTX::Util::Image
{

	FilePath write(
		const FilePath &			  p_path,
		const E_FORMAT				  p_format,
		const size_t				  p_width,
		const size_t				  p_height,
		const uchar * const			  p_data,
		const std::optional<FilePath> p_watermark
	)
	{
		// Remove extension from path.
		FilePath path = p_path.parent_path() / p_path.stem();

		stbi_flip_vertically_on_write( true );

		// Watermark.
		/*		if ( p_watermark.has_value() )
		{
			FilePath waterMarkPath = p_watermark.value();

			// Load watermark.
			int		watermarkWidth, watermarkHeight, watermarkChannels;
			uchar * watermarkData
				= stbi_load( waterMarkPath.string().c_str(), &watermarkWidth, &watermarkHeight, &watermarkChannels, 4 );
		}
		*/

		// Save image.
		switch ( p_format )
		{
		case E_FORMAT::PNG:
			path += ".png";
			stbi_write_png_compression_level = 0;
			stbi_write_png( path.string().c_str(), int( p_width ), int( p_height ), 4, p_data, 0 );
			break;
		case E_FORMAT::JPEG:
			path += ".jpg";
			stbi_write_jpg( path.string().c_str(), int( p_width ), int( p_height ), 4, p_data, 100 );
			break;

		default: assert( 1 ); ;
		}

		return path;
	}

	/*

	void Snapshoter::_addWatermark( QImage & p_image ) const
	{
		QSvgRenderer watermarkSvg( QString( ":/sprite/logo_vect.svg" ) );
		QSize		 watermarkSize = watermarkSvg.defaultSize();

		// Compute size.
		uint  desiredHeight = p_image.size().height() / 6;
		float ratio			= desiredHeight / (float)watermarkSize.height();
		watermarkSize.setHeight( desiredHeight );
		watermarkSize.setWidth( watermarkSize.width() * ratio );

		// Create image to render svg into.
		QImage watermarkImg( watermarkSize, QImage::Format_RGBA64 );
		watermarkImg.fill( QColor( 255, 255, 255, 0 ) );
		QPainter watermarkPainter = QPainter( &watermarkImg );
		watermarkSvg.render( &watermarkPainter );

		// Reduce alpha.
		for ( int i = 0; i < watermarkImg.width(); ++i )
		{
			for ( int j = 0; j < watermarkImg.height(); ++j )
			{
				QColor pixelColor = watermarkImg.pixelColor( i, j );
				pixelColor.setAlpha( pixelColor.alpha() / 2 );
				watermarkImg.setPixelColor( i, j, pixelColor );
			}
		}

		// Invert color to match background.
		if ( VTX_RENDER_EFFECT().getBackgroundColor().brightness() < 0.5f && _exportData.getBackgroundOpacity() > 0.5f )
		{
			watermarkImg.invertPixels( QImage::InvertMode::InvertRgb );
		}

#ifdef VTX_DEBUG_WATERMARK
		watermarkImg.save( QString( _path.string().c_str() ), "png", 0 );
		return;
#endif

		// Paint watermark over the desired image.
		QRect rect = QRect(
			QPoint( p_image.size().width() - watermarkImg.width(), p_image.size().height() - watermarkImg.height() ),
			watermarkImg.size() );
		QPainter imagePainter = QPainter( &p_image );
		imagePainter.drawImage( rect, watermarkImg );
	}

	*/
} // namespace VTX::Util::Image
