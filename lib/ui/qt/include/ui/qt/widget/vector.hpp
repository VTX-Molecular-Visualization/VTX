#ifndef __VTX_UI_QT_WIDGET_VECTOR__
#define __VTX_UI_QT_WIDGET_VECTOR__

#include <QDoubleSpinBox>
#include <QEvent>
#include <QHBoxLayout>
#include <util/constants.hpp>

namespace VTX::UI::QT::Widget
{

	class IVector : public QWidget
	{
		Q_OBJECT

	  public:
		IVector( QWidget * p_parent ) : QWidget( p_parent ) {}
		virtual ~IVector() = default;

	  signals:
		void valueEdited();
	};

	template<typename V>
	class Vector : public IVector
	{
	  public:
		using Scalar							 = typename V::value_type;
		static constexpr int			  N		 = V::length();
		static constexpr std::string_view PREFIX = "xyzw";

		static_assert( N >= 2 && N <= 4, "Vector widget supports only vec2/vec3/vec4" );
		static_assert( std::is_arithmetic_v<Scalar>, "Vector widget requires numeric scalar" );
		static_assert( not std::is_unsigned_v<Scalar>, "Unsigned glm::uvec* not supported" );

		Vector( QWidget * p_parent ) : IVector( p_parent )
		{
			auto * lay = new QHBoxLayout( this );
			lay->setContentsMargins( 0, 0, 0, 0 );

			for ( int i = 0; i < N; i++ )
			{
				if constexpr ( std::is_integral_v<Scalar> )
				{
					auto * sb = new QSpinBox( this );
					sb->setRange( TypeLowest<Scalar>, TypeMax<Scalar> );
					sb->setPrefix( QString( PREFIX[ i ] ) + ": " );

					_spinBoxes[ i ] = sb;
				}
				else
				{
					auto * sb = new QDoubleSpinBox( this );
					sb->setRange( TypeLowest<Scalar>, TypeMax<Scalar> );
					sb->setPrefix( QString( PREFIX[ i ] ) + ": " );
					sb->setDecimals( 2 );

					_spinBoxes[ i ] = sb;
				}
				lay->addWidget( _spinBoxes[ i ] );
			}

			for ( auto * sb : _spinBoxes )
			{
				sb->setLocale( QLocale::c() );
				sb->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed );
				sb->setButtonSymbols( QAbstractSpinBox::NoButtons );
				sb->installEventFilter( this );

				connect( sb, &QAbstractSpinBox::editingFinished, [ this ]() { emit valueEdited(); } );
			}
		}

		bool eventFilter( QObject * p_obj, QEvent * p_ev ) override
		{
			if ( auto * sb = qobject_cast<QAbstractSpinBox *>( p_obj ) )
			{
				if ( sb->isEnabled() && not sb->isReadOnly() )
				{
					if ( p_ev->type() == QEvent::Enter )
					{
						sb->setButtonSymbols( QAbstractSpinBox::UpDownArrows );
					}
					else if ( p_ev->type() == QEvent::Leave )
					{
						sb->setButtonSymbols( QAbstractSpinBox::NoButtons );
					}
					else if ( p_ev->type() == QEvent::MouseButtonRelease )
					{
						emit valueEdited();
					}
				}
			}
			return QWidget::eventFilter( p_obj, p_ev );
		}

		void setValue( const V & p_v )
		{
			for ( int i = 0; i < N; i++ )
			{
				QSignalBlocker blocker( _spinBoxes[ i ] );
				if constexpr ( std::is_integral_v<Scalar> )
				{
					static_cast<QSpinBox *>( _spinBoxes[ i ] )->setValue( p_v[ i ] );
				}
				else
				{
					static_cast<QDoubleSpinBox *>( _spinBoxes[ i ] )->setValue( double( p_v[ i ] ) );
				}
			}
		}

		V value() const
		{
			V v;
			for ( int i = 0; i < N; i++ )
			{
				if constexpr ( std::is_integral_v<Scalar> )
				{
					v[ i ] = Scalar( static_cast<QSpinBox *>( _spinBoxes[ i ] )->value() );
				}
				else
				{
					v[ i ] = Scalar( static_cast<QDoubleSpinBox *>( _spinBoxes[ i ] )->value() );
				}
			}
			return v;
		}

		void setRange( const V p_min, const V p_max )
		{
			for ( int i = 0; i < N; i++ )
			{
				if constexpr ( std::is_integral_v<Scalar> )
				{
					static_cast<QSpinBox *>( _spinBoxes[ i ] )->setRange( p_min[ i ], p_max[ i ] );
				}
				else
				{
					static_cast<QDoubleSpinBox *>( _spinBoxes[ i ] )
						->setRange( double( p_min[ i ] ), double( p_max[ i ] ) );
				}
			}
		}

		void setDecimals( const int p_precision )
		{
			for ( int i = 0; i < N; i++ )
			{
				if constexpr ( not std::is_integral_v<Scalar> )
				{
					static_cast<QDoubleSpinBox *>( _spinBoxes[ i ] )->setDecimals( p_precision );
				}
			}
		}

		void setSuffix( const QString & p_suffix )
		{
			for ( int i = 0; i < N; i++ )
			{
				if constexpr ( std::is_integral_v<Scalar> )
				{
					static_cast<QSpinBox *>( _spinBoxes[ i ] )->setSuffix( p_suffix );
				}
				else
				{
					static_cast<QDoubleSpinBox *>( _spinBoxes[ i ] )->setSuffix( p_suffix );
				}
			}
		}

		void setSingleStep( const Scalar p_step )
		{
			for ( int i = 0; i < N; i++ )
			{
				if constexpr ( std::is_integral_v<Scalar> )
				{
					static_cast<QSpinBox *>( _spinBoxes[ i ] )->setSingleStep( p_step );
				}
				else
				{
					static_cast<QDoubleSpinBox *>( _spinBoxes[ i ] )->setSingleStep( static_cast<double>( p_step ) );
				}
			}
		}

		void setKeyboardTracking( const bool p_enable )
		{
			for ( int i = 0; i < N; i++ )
			{
				_spinBoxes[ i ]->setKeyboardTracking( p_enable );
			}
		}

		void setAccelerated( const bool p_enable )
		{
			for ( int i = 0; i < N; i++ )
			{
				_spinBoxes[ i ]->setAccelerated( p_enable );
			}
		}

		void setEnabled( const bool p_enable )
		{
			for ( int i = 0; i < N; i++ )
			{
				_spinBoxes[ i ]->setEnabled( p_enable );
			}
		}

		void setReadOnly( const bool p_readOnly )
		{
			for ( int i = 0; i < N; i++ )
			{
				_spinBoxes[ i ]->setReadOnly( p_readOnly );
			}
		}

	  private:
		std::array<QAbstractSpinBox *, N> _spinBoxes {};
	};

} // namespace VTX::UI::QT::Widget

#endif
