#ifndef __VTX_UI_QT_WIDGET_TRANSFORM__
#define __VTX_UI_QT_WIDGET_TRANSFORM__

#include "ui/qt/widget/vector.hpp"
#include <QWidget>
#include <util/enum.hpp>
#include <util/math/transform.hpp>
#include <util/types.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Widget to manipulate a Transform (position, rotation, scale).
	 */
	class Transform : public QWidget
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Flags to enable/disable component.
		 */
		enum struct E_FLAG : uint8_t
		{
			VTX_ENUM_ENABLE_BITMASK,

			NONE	 = 0,
			POSITION = 1 << 0,
			ROTATION = 1 << 1,
			SCALE	 = 1 << 2,
			ALL		 = 0xFFu
		};

		/**
		 * @brief Build UI and connect signals.
		 */
		Transform( QWidget * p_parent, const E_FLAG p_flags = E_FLAG::ALL ) : QWidget( p_parent )
		{
			auto * layout = new QVBoxLayout( this );
			layout->setContentsMargins( 0, 0, 0, 0 );

			// Position vector.
			if ( ( p_flags & E_FLAG::POSITION ) != E_FLAG::NONE )
			{
				_position = new Widget::Vector<Vec3f>( this );
				layout->addWidget( new QLabel( "Position", this ) );
				layout->addWidget( _position );
				connect(
					_position,
					&Widget::Vector<Vec3f>::valueEdited,
					this,
					[ this ]() { emit positionChanged( _position->value() ); }
				);
			}

			// Rotation vector.
			if ( ( p_flags & E_FLAG::ROTATION ) != E_FLAG::NONE )
			{
				_rotation = new Widget::Vector<Vec3i>( this );
				_rotation->setRange( Vec3i( -360 ), Vec3i( 360 ) );
				_rotation->setSuffix( QChar( 0x00B0 ) );
				layout->addWidget( new QLabel( "Rotation", this ) );
				layout->addWidget( _rotation );
				connect(
					_rotation,
					&Widget::Vector<Vec3i>::valueEdited,
					this,
					[ this ]() { emit rotationChanged( Quatf( _rotation->value() ) ); }
				);
			}

			// Scale vector.
			if ( ( p_flags & E_FLAG::SCALE ) != E_FLAG::NONE )
			{
				_scale = new Widget::Vector<Vec3f>( this );
				layout->addWidget( new QLabel( "Scale", this ) );
				layout->addWidget( _scale );
				connect(
					_scale,
					&Widget::Vector<Vec3f>::valueEdited,
					this,
					[ this ]() { emit scaleChanged( _scale->value() ); }
				);
			}

			setLayout( layout );
		}

		/**
		 * @brief Accessors and mutators.
		 */
		inline void setPosition( const Vec3f & p_position )
		{
			assert( _position );
			_position->setValue( p_position );
		}

		inline void setRotation( const Quatd & p_rotation )
		{
			assert( _rotation );
			_rotation->setValue( VTX::Util::Math::eulerAngles( p_rotation ) );
		}

		inline void setScale( const Vec3f & p_scale )
		{
			assert( _scale );
			_scale->setValue( p_scale );
		}

		inline void setTransform( const VTX::Util::Math::Transform & p_transform )
		{
			setPosition( p_transform.getPosition() );
			setRotation( p_transform.getRotation() );
			setScale( p_transform.getScale() );
		}

	  signals:
		/**
		 * @brief Emits when a component is changed.
		 */
		void positionChanged( const Vec3f & p_position );
		void rotationChanged( const Quatf & p_rotation );
		void scaleChanged( const Vec3f & p_scale );

	  private:
		/**
		 * @brief Position vector.
		 */
		QPointer<Vector<Vec3f>> _position;

		/**
		 * @brief Rotation vector.
		 */
		QPointer<Vector<Vec3i>> _rotation;

		/**
		 * @brief Scale vector.
		 */
		QPointer<Vector<Vec3f>> _scale;
	};

} // namespace VTX::UI::QT::Widget

#endif
