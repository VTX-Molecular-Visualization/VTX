#ifndef __VTX_UI_WIDGET_MODEL_LIST_WIDGET__
#define __VTX_UI_WIDGET_MODEL_LIST_WIDGET__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include <QGridLayout>
#include <QObject>
#include <QString>
#include <QWidget>
#include <app/old/core/model/base_model.hpp>
#include <app/old/id.hpp>
#include <type_traits>
#include <vector>

namespace VTX::UI::Widget::CustomWidget
{
	class BaseModelListWidget;

	class BaseModelFieldLine : public QObject
	{
	  public:
		BaseModelFieldLine( BaseModelListWidget * p_parent );
		virtual ~BaseModelFieldLine() = default;

		virtual App::Old::Core::Model::BaseModel * const getModel() const										= 0;
		virtual void								setModel( App::Old::Core::Model::BaseModel * const p_model ) = 0;

		virtual bool blockSignals( const bool p_block ) { return QObject::blockSignals( p_block ); };
	};

	class BaseModelListWidget : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET
		Q_OBJECT

	  private:
		enum class GRID_LAYOUT_COLUMN : int
		{
			TICK  = 0,
			MODEL = 1,
		};

	  public:
		void localize() override;

		void addModel( App::Old::Core::Model::BaseModel * const p_model );
		void insertModel( App::Old::Core::Model::BaseModel * const p_model, const int p_row );
		void removeModel( App::Old::Core::Model::BaseModel * const p_model );
		void clearModels();
		void swapModels( App::Old::Core::Model::BaseModel * const p_model1,
						 App::Old::Core::Model::BaseModel * const p_model2 ) const;

		bool									   hasModel( const App::Old::Core::Model::BaseModel * const p_model ) const;
		int										   getModelCount() const;
		std::vector<App::Old::Core::Model::BaseModel *> getModels() const;

		template<typename M, typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>>
		std::vector<M *> getModels() const
		{
			std::vector<M *>								 res		= std::vector<M *>();
			const std::vector<App::Old::Core::Model::BaseModel *> baseModels = getModels();
			res.reserve( baseModels.size() );

			for ( const App::Old::Core::Model::BaseModel * const model : baseModels )
				res.emplace_back( static_cast<M *>( model ) );

			return res;
		}

		bool		 getContainsOnlyUniqueModel() const { return _containsOnlyUniqueModel; }
		virtual void setContainsOnlyUniqueModel( const bool p_containsOnlyUniqueModel );

		const std::vector<App::Old::VTX_ID> & getFilters() const { return _filters; }
		virtual void					 addTypeFilter( const App::Old::VTX_ID & p_typeID );

	  signals:
		void onModelListChange();

	  protected:
		BaseModelListWidget( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		virtual void _initColumns() = 0;
		void		 _initColumn( const int p_columnIndex, const std::string p_title, const int p_stretch );

		void _addModelInLayout( App::Old::Core::Model::BaseModel * const p_model, const int p_row );
		virtual BaseModelFieldLine * const _instantiateLine() = 0;
		virtual void _initLine( BaseModelFieldLine * const p_line, App::Old::Core::Model::BaseModel * const p_model ) const;
		virtual void _addLineInLayout( BaseModelFieldLine * const p_line, const int p_row ) = 0;

		void _addWidgetInColumn( QWidget * p_widget, const int p_row, const int p_column );

		virtual void _onLineWillBeRemoved( CustomWidget::BaseModelFieldLine * const p_line ) {};

		QGridLayout *							  _getLayout() const { return _layout; }
		std::vector<BaseModelFieldLine *> &		  _getLines() { return _lines; }
		const std::vector<BaseModelFieldLine *> & _getLines() const { return _lines; }

		bool				 _isModelAlreadyInList( const App::Old::Core::Model::BaseModel * const p_model ) const;
		BaseModelFieldLine * _findLineFromModel( const App::Old::Core::Model::BaseModel * const p_model ) const;

		template<typename T, typename = std::enable_if<std::is_base_of<BaseModelFieldLine, T>::value>>
		T * _findLineFromModel( const App::Old::Core::Model::BaseModel * const p_model ) const
		{
			return dynamic_cast<T *>( _findLineFromModel( p_model ) );
		}

		BaseModelFieldLine * _findNextLine( const BaseModelFieldLine * const p_from ) const;

	  private:
		QGridLayout * _layout = nullptr;

		std::vector<BaseModelFieldLine *> _lines				   = std::vector<BaseModelFieldLine *>();
		bool							  _containsOnlyUniqueModel = true;
		std::vector<App::Old::VTX_ID>		  _filters				   = std::vector<App::Old::VTX_ID>();
	};

} // namespace VTX::UI::Widget::CustomWidget
#endif
