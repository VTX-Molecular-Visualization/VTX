#ifndef __VTX_UI_WIDGET_OPENGL__
#define __VTX_UI_WIDGET_OPENGL__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include <QElapsedTimer>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLWidget>
#include <QPainter>
#include <QSurface>
#include <app/old_app/renderer/base_renderer.hpp>

using OpenGLFunctions = QOpenGLFunctions_4_5_Core;
using OpenGLContext	  = QOpenGLContext;
using OpenGLSurface	  = QSurface;
using OpenGLWidget	  = QOpenGLWidget;

namespace VTX
{
	namespace Renderer
	{
		namespace GL
		{
			class GL;
		} // namespace GL
		class RayTracer;
	} // namespace Renderer

	namespace UI::Widget::Render
	{
		class OpenGLWidget : public BaseManualWidget<QOpenGLWidget>, public Generic::BaseOpenGL
		{
			VTX_WIDGET

		  public:
			enum RenderSettingFlag
			{
				Targets		= 1 << 0,
				ColorBuffer = 1 << 1,

				ALL = 0xFFFF,
			};

			~OpenGLWidget();

			void localize() override {}

			inline VTX::Renderer::BaseRenderer &	   getRenderer() { return *_renderer; }
			inline const VTX::Renderer::BaseRenderer & getRenderer() const { return *_renderer; }
			inline VTX::Renderer::GL::GL &			   getRendererGL() { return *_rendererGL; }
			inline const VTX::Renderer::GL::GL &	   getRendererGL() const { return *_rendererGL; }
			inline VTX::Renderer::RayTracer &		   getRendererRT() { return *_rendererRT; }
			inline const VTX::Renderer::RayTracer &	   getRendererRT() const { return *_rendererRT; }

			void		initializeGL() override;
			void		paintGL() override;
			void		resizeGL( int, int ) override;
			inline void updateRenderSetting( const VTX::Renderer::RENDER_SETTING p_setting )
			{
				_renderer->updateRenderSetting( p_setting );
			}

			const float getScreenPixelRatio() const;
			const Vec2i getPickedIds( const uint, const uint );

		  protected:
			OpenGLWidget( QWidget * p_parent );

			void _setupUi( const QString & p_name ) override {}
			void _setupSlots() override {}

		  protected:
			static inline OpenGLFunctions * _gl		 = nullptr;
			static inline OpenGLContext *	_context = nullptr;
			static inline OpenGLSurface *	_surface = nullptr;

		  private:
			QElapsedTimer _timer		= QElapsedTimer();
			QElapsedTimer _frameTimer	= QElapsedTimer();
			uint		  _frameCounter = 0u;
			QPainter	  _painter		= QPainter();

			VTX::Renderer::BaseRenderer * _renderer	  = nullptr;
			VTX::Renderer::GL::GL *		  _rendererGL = nullptr;
			VTX::Renderer::RayTracer *	  _rendererRT = nullptr;

			void _switchRenderer( const VTX::Renderer::MODE );
			void _retrieveSpec() const;
		};
	} // namespace UI::Widget::Render

} // namespace VTX

#endif
