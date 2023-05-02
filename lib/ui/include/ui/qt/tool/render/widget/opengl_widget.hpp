#ifndef __VTX_UI_QT_WIDGET_OPENGL__
#define __VTX_UI_QT_WIDGET_OPENGL__

#include "ui/qt/widget/base_manual_widget.hpp"
#include <QElapsedTimer>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLWidget>
#include <QPainter>
#include <QSurface>
#include <app/render/renderer/base_renderer.hpp>

using OpenGLFunctions = QOpenGLFunctions_4_5_Core;
using OpenGLContext	  = QOpenGLContext;
using OpenGLSurface	  = QSurface;
using OpenGLWidget	  = QOpenGLWidget;

namespace VTX
{
	namespace App::Render::Renderer
	{
		namespace GL
		{
			class GL;
		} // namespace GL

		namespace RT
		{
			class RayTracer;
		} // namespace RT

	} // namespace App::Render::Renderer

	namespace UI::QT::Tool::Render::Widget
	{
		class OpenGLWidget : public QT::Widget::BaseManualWidget<QOpenGLWidget>, public Generic::BaseOpenGL
		{
			NEW_ARCHI_VTX_WIDGET

		  public:
			enum RenderSettingFlag
			{
				Targets		= 1 << 0,
				ColorBuffer = 1 << 1,

				ALL = 0xFFFF,
			};

			~OpenGLWidget();

			void localize() override {}

			inline App::Render::Renderer::BaseRenderer &		getRenderer() { return *_renderer; }
			inline const App::Render::Renderer::BaseRenderer &	getRenderer() const { return *_renderer; }
			inline App::Render::Renderer::GL::GL &				getRendererGL() { return *_rendererGL; }
			inline const App::Render::Renderer::GL::GL &		getRendererGL() const { return *_rendererGL; }
			inline App::Render::Renderer::RT::RayTracer &		getRendererRT() { return *_rendererRT; }
			inline const App::Render::Renderer::RT::RayTracer & getRendererRT() const { return *_rendererRT; }

			void		initializeGL() override;
			void		paintGL() override;
			void		resizeGL( int, int ) override;
			inline void updateRenderSetting( const App::Render::Renderer::RENDER_SETTING p_setting )
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

			App::Render::Renderer::BaseRenderer *  _renderer   = nullptr;
			App::Render::Renderer::GL::GL *		   _rendererGL = nullptr;
			App::Render::Renderer::RT::RayTracer * _rendererRT = nullptr;

			void _switchRenderer( const App::Render::Renderer::MODE );
			void _retrieveSpec() const;
		};
	} // namespace UI::QT::Tool::Render::Widget

} // namespace VTX

#endif
