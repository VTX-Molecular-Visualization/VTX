#include "snapshoter.hpp"
#include "action/action_manager.hpp"
#include "action/setting.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/gl.hpp"
#include "ui/main_window.hpp"
#include "util/filesystem.hpp"
#include "util/time.hpp"
#include "vtx_app.hpp"
#include <QOpenGLFramebufferObject>
#include <QPainter>
#include <QSvgRenderer>
#include <vector>

//#define VTX_DEBUG_WATERMARK

namespace VTX
{
	namespace Worker
	{
		const void Snapshoter::_takeSnapshotGL() const
		{
			if ( _path.exists() )
			{
				VTX_ERROR( "File already exists" );
				return;
			}

			const float						   pixelRatio = VTXApp::get().getPixelRatio();
			UI::Widget::Render::OpenGLWidget & glWidget	  = VTXApp::get().getMainWindow().getOpenGLWidget();

			glWidget.makeCurrent();

			Renderer::GL::Framebuffer fbo	  = Renderer::GL::Framebuffer( glWidget.getGL() );
			Renderer::GL::Texture2D	  texture = Renderer::GL::Texture2D( glWidget.getGL() );

			texture.create( _width,
							_height,
							Renderer::GL::Texture2D::InternalFormat::RGBA16F,
							Renderer::GL::Texture2D::Wrapping::CLAMP_TO_EDGE,
							Renderer::GL::Texture2D::Wrapping::CLAMP_TO_EDGE,
							Renderer::GL::Texture2D::Filter::LINEAR,
							Renderer::GL::Texture2D::Filter::LINEAR );
			fbo.create( Renderer::GL::Framebuffer::Target::FRAMEBUFFER );
			fbo.attachTexture( texture, Renderer::GL::Framebuffer::Attachment::COLOR0 );

			// Force AA.
			const bool activeAA = VTX_RENDER_EFFECT().getAA();
			if ( activeAA == false )
			{
				VTX_ACTION( new Action::Setting::ActiveAA( true ) );
			}

			VTXApp::get().getScene().getCamera().setScreenSize( _width, _height );

			////////////
			/*
			QOpenGLFramebufferObject qfbo
				= QOpenGLFramebufferObject( _width * pixelRatio,
											_height * pixelRatio,
											QOpenGLFramebufferObject::Attachment::NoAttachment,
											GL_TEXTURE_2D,
											GL_RGB16F );
											*/
			// qfbo.addColorAttachment( _width * pixelRatio, _height * pixelRatio, GL_RGB16F );
			////////////

			glWidget.getRenderer().resize( _width * pixelRatio, _height * pixelRatio, fbo.getId() );
			glWidget.getRenderer().renderFrame( VTXApp::get().getScene() );

			// Grab image.
			// QImage render = glWidget.grabFramebuffer();
			// VTX_DEBUG( std::to_string( render.format() ) );

			std::vector<uchar> data = std::vector<uchar>();
			data.reserve( 64 * _width * _height );

			texture.getImage( 0,
							  Renderer::GL::Texture2D::Format::RGBA,
							  Renderer::GL::Texture2D::Type::FLOAT,
							  64 * _width * _height,
							  data.data() );

			QImage render = QImage( data.data(), _width, _height, QImage::Format::Format_RGBA64 );

			// QImage render = qfbo.toImage();

			// Restore values.
			if ( activeAA == false )
			{
				VTX_ACTION( new Action::Setting::ActiveAA( false ) );
			}

			glWidget.doneCurrent();

			// fbo.destroy();

			// Add watermark.
			//_addWatermark( render );

#ifndef VTX_DEBUG_WATERMARK
			// Save.
			if ( render.save( _path.qpath(), _path.extension().c_str(), _quality ) )
			{
				VTX_INFO( "Snapshot taken: " + _path.filename() );
			}
			else
			{
				VTX_ERROR( "Snapshot failed" );
			}
#endif

			glWidget.resizeGL( glWidget.width(), glWidget.height() );
		}

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
			if ( VTX_RENDER_EFFECT().getBackgroundColor().brightness() < 0.5f
				 && VTX_SETTING().getSnapshotBackgroundOpacity() > 0.5f )
			{
				watermarkImg.invertPixels( QImage::InvertMode::InvertRgb );
			}

#ifdef VTX_DEBUG_WATERMARK
			watermarkImg.save( QString( _path.string().c_str() ), "png", 0 );
			return;
#endif

			// Paint watermark over the desired image.
			QRect	 rect		  = QRect( QPoint( p_image.size().width() - watermarkImg.width(),
										   p_image.size().height() - watermarkImg.height() ),
								   watermarkImg.size() );
			QPainter imagePainter = QPainter( &p_image );
			imagePainter.drawImage( rect, watermarkImg );
		}

	} // namespace Worker
} // namespace VTX
