#ifndef VTX_UI_QT_WIDGET_OPENGL_RENDERER_HPP
#define VTX_UI_QT_WIDGET_OPENGL_RENDERER_HPP

#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <cstdint>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Handle user input and forward it to the InputManager.
	 */
	class OpenGLRenderer : public QOpenGLWidget
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Constructor.
		 */
		explicit OpenGLRenderer( QWidget * p_parent );
		~OpenGLRenderer() override;

		static QSurfaceFormat surfaceFormat();
		static bool			  isOpenGLSupported();

		bool	 createGLContext();
		void	 destroyGLContext();
		void	 swapGLBuffers();
		bool	 setGLSwapInterval( int ) const;
		void *	 getGLProcAddress( const char * ) const;
		uint32_t getGLDefaultFramebuffer() const;

	  protected:
		/**
		 * @brief Override events.
		 */
		bool event( QEvent * ) override;
		void resizeGL( int, int ) override;
		void paintGL() override;

	  private:
		bool	 _glReady			 = false;
		uint32_t _defaultFramebuffer = 0;

		void _syncDefaultFramebuffer();
	};

} // namespace VTX::UI::QT::Widget

#endif
