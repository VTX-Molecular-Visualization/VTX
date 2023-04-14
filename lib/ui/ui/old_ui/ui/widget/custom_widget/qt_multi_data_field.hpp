#ifndef __VTX_UI_WIDGET_CUSTOM_QT_MUTLI_DATA_FIELD__
#define __VTX_UI_WIDGET_CUSTOM_QT_MUTLI_DATA_FIELD__

#include "old_ui/style.hpp"
#include "old_ui/ui/multi_data_field.hpp"
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class QLabelMultiField : public QLabel, public TMultiDataFieldEquatable<const std::string>
	{
	  public:
		QLabelMultiField( QWidget * const p_parent = nullptr ) :
			QLabel( p_parent ), TMultiDataFieldEquatable<const std::string>() {};

		void resetState() override
		{
			TMultiDataField::resetState();
			_value = "";
			setText( "" );
		}

	  protected:
		const std::string & _getValue() const override { return _value; }
		void				_setSingleValue( const std::string & p_value ) override
		{
			setText( QString::fromStdString( p_value ) );
			_value = p_value;
		}
		void _displayDifferentsDataFeedback() override { setText( Style::DIFFERENT_MULTIPLE_DATA_STRING ); }

	  private:
		std::string _value;
	};

	class QPushButtonMultiField : public QPushButton, public TMultiDataFieldEquatable<const std::string>
	{
	  public:
		QPushButtonMultiField( QWidget * const p_parent = nullptr ) :
			QPushButton( p_parent ), TMultiDataFieldEquatable<const std::string>() {};

		void resetState() override
		{
			TMultiDataField::resetState();
			_value = "";
			setText( "" );
		}

	  protected:
		const std::string & _getValue() const override { return _value; }
		void				_setSingleValue( const std::string & p_value ) override
		{
			setText( QString::fromStdString( p_value ) );
			_value = p_value;
		}

		virtual void _displayDifferentsDataFeedback() override { setText( Style::DIFFERENT_MULTIPLE_DATA_STRING ); }

	  private:
		std::string _value;
	};

	class QComboBoxMultiField : public QComboBox, public TMultiDataFieldEquatable<const int>
	{
	  public:
		QComboBoxMultiField( QWidget * const p_parent = nullptr ) :
			QComboBox( p_parent ), TMultiDataFieldEquatable<const int>() {};

		void resetState() override
		{
			TMultiDataFieldEquatable::resetState();
			_index = -1;
		}

	  protected:
		const int & _getValue() const override { return _index; }
		void		_setSingleValue( const int & p_value ) override
		{
			setCurrentIndex( p_value );
			_index = p_value;
		}

		void _displayDifferentsDataFeedback() override
		{
			const bool previousSignalState = blockSignals( true );
			setCurrentIndex( -1 );
			setCurrentText( Style::DIFFERENT_MULTIPLE_DATA_STRING );
			blockSignals( previousSignalState );
		}

	  private:
		int _index = -1;
	};

	class QLineEditMultiField : public QLineEdit, public TMultiDataFieldEquatable<const std::string>
	{
	  public:
		QLineEditMultiField( QWidget * const p_parent = nullptr ) :
			QLineEdit( p_parent ), TMultiDataFieldEquatable<const std::string>() {};

		void resetState() override
		{
			TMultiDataField::resetState();
			_value = "";
			setText( "" );
		}

	  protected:
		const std::string & _getValue() const override { return _value; }
		void				_setSingleValue( const std::string & p_value ) override
		{
			setText( QString::fromStdString( p_value ) );
			_value = p_value;
		}
		virtual void _displayDifferentsDataFeedback() override { setText( "-" ); }

	  private:
		std::string _value;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
