#ifndef __VTX_UI_QT_WIDGET_TRANSFORM__
#define __VTX_UI_QT_WIDGET_TRANSFORM__

#include "ui/qt/widget/vector.hpp"
#include <QLabel>
#include <QPointer>
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
			using namespace VTX::Util::Math;

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
					[ this ]() { emit positionChanged( _position->value() ); }
				);
			}

			// Rotation vector.
			if ( ( p_flags & E_FLAG::ROTATION ) != E_FLAG::NONE )
			{
				_rotation = new Widget::Vector<Vec3f>( this );
				_rotation->setRange( Vec3f( -360 ), Vec3f( 360 ) );
				_rotation->setDecimals( 0 );
				_rotation->setSuffix( QChar( 0x00B0 ) );
				layout->addWidget( new QLabel( "Rotation", this ) );
				layout->addWidget( _rotation );
				connect(
					_rotation,
					&Widget::Vector<Vec3f>::valueEdited,
					[ this ]() { emit rotationChanged( Quatf( radians( _rotation->value() ) ) ); }
				);
			}

			// Scale vector.
			if ( ( p_flags & E_FLAG::SCALE ) != E_FLAG::NONE )
			{
				_scale = new Widget::Vector<Vec3f>( this );
				layout->addWidget( new QLabel( "Scale", this ) );
				layout->addWidget( _scale );
				connect(
					_scale, &Widget::Vector<Vec3f>::valueEdited, [ this ]() { emit scaleChanged( _scale->value() ); }
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
			QSignalBlocker blocker( _position );
			_position->setValue( p_position );
		}

		inline void setRotation( const Quatd & p_rotation )
		{
			using namespace VTX::Util::Math;
			assert( _rotation );
			QSignalBlocker blocker( _rotation );
			_rotation->setValue( degrees( eulerAngles( p_rotation ) ) );
		}

		inline void setScale( const Vec3f & p_scale )
		{
			assert( _scale );
			QSignalBlocker blocker( _scale );
			_scale->setValue( p_scale );
		}

		inline void setTransform( const VTX::Util::Math::Transform & p_transform )
		{
			if ( _position )
				setPosition( p_transform.getPosition() );
			if ( _rotation )
				setRotation( p_transform.getRotation() );
			if ( _scale )
				setScale( p_transform.getScale() );
		}

		inline void setReadOnly( const bool p_readOnly, const E_FLAG p_flags = E_FLAG::ALL )
		{
			if ( _position && ( p_flags & E_FLAG::ROTATION ) != E_FLAG::NONE )
				_position->setReadOnly( p_readOnly );
			if ( _rotation && ( p_flags & E_FLAG::ROTATION ) != E_FLAG::NONE )
				_rotation->setReadOnly( p_readOnly );
			if ( _scale && ( p_flags & E_FLAG::SCALE ) != E_FLAG::NONE )
				_scale->setReadOnly( p_readOnly );
		}

		inline void setEnabled( const bool p_enabled, const E_FLAG p_flags = E_FLAG::ALL )
		{
			if ( _position && ( p_flags & E_FLAG::ROTATION ) != E_FLAG::NONE )
				_position->setEnabled( p_enabled );
			if ( _rotation && ( p_flags & E_FLAG::ROTATION ) != E_FLAG::NONE )
				_rotation->setEnabled( p_enabled );
			if ( _scale && ( p_flags & E_FLAG::SCALE ) != E_FLAG::NONE )
				_scale->setEnabled( p_enabled );
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
		QPointer<Vector<Vec3f>> _rotation;

		/**
		 * @brief Scale vector.
		 */
		QPointer<Vector<Vec3f>> _scale;
	};

} // namespace VTX::UI::QT::Widget

#endif
