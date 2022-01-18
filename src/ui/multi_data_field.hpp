#ifndef __VTX_UI_MULTI_DATA_FIELD__
#define __VTX_UI_MULTI_DATA_FIELD__

#include <QComboBox>
#include <QLabel>
#include <QWidget>
#include <type_traits>

namespace VTX::UI
{
	class MultiDataField
	{
	  public:
		enum class State
		{
			Uninitialized,
			Identical,
			Different
		};

		virtual void resetState() { _state = MultiDataField::State::Uninitialized; }
		virtual bool hasIdenticalData() const { return _state == MultiDataField::State::Identical; }
		virtual bool hasDifferentData() const { return _state == MultiDataField::State::Different; }

	  protected:
		virtual void _displayDifferentsDataFeedback() {};

		MultiDataField::State _state = MultiDataField::State::Uninitialized;
	};

	template<typename T>
	class TMultiDataField : public MultiDataField
	{
	  public:
		virtual void updateWithNewValue( T & p_value ) = 0;
	};

	template<typename T>
	class TMultiDataFieldEquatable : public TMultiDataField<T>
	{
	  public:
		virtual void updateWithNewValue( T & p_value )
		{
			switch ( MultiDataField::_state )
			{
			case MultiDataField::State::Uninitialized:
				_setSingleValue( p_value );
				MultiDataField::_state = MultiDataField::State::Identical;
				break;

			case MultiDataField::State::Identical:
				if ( !_isEquals( _getValue(), p_value ) )
				{
					MultiDataField::_state = MultiDataField::State::Different;
					MultiDataField::_displayDifferentsDataFeedback();
				}
				break;

			case MultiDataField::State::Different: break;
			}
		}

	  protected:
		virtual const T & _getValue() const					   = 0;
		virtual void	  _setSingleValue( const T & p_value ) = 0;

		virtual bool _isEquals( const T & p_lhs, const T & p_rhs ) const { return p_lhs == p_rhs; }
	};

} // namespace VTX::UI

#endif
