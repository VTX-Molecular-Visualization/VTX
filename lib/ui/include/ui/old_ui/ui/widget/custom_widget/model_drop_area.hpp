#ifndef __VTX_UI_WIDGET_MODEL_DROP_AREA__
#define __VTX_UI_WIDGET_MODEL_DROP_AREA__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFrame>
#include <QString>
#include <QWidget>
#include <app/id.hpp>
#include <vector>

namespace VTX
{
	namespace App::Core::Model
	{
		class BaseModel;
	} // namespace App::Core::Model

	namespace UI::Widget::CustomWidget
	{
		class ModelDropArea : public BaseManualWidget<QFrame>
		{
			Q_OBJECT
			VTX_WIDGET

		  public:
			static void copyTypeFilters( const ModelDropArea & p_from, ModelDropArea & p_to );

		  public:
			~ModelDropArea() {};
			void localize() override;

			bool isAcceptingGroup() const { return _acceptGroup; }
			void setAcceptGroup( const bool p_accept );

			void addTypeFilter( const VTX::App::VTX_ID & p_modelID );
			bool matchFilter( const VTX::App::VTX_ID & p_type ) const;
			bool matchFilter( const App::Core::Model::BaseModel & p_model ) const;

		  signals:
			void onModelDropped( App::Core::Model::BaseModel * const p_model );
			void onModelsDropped( std::vector<App::Core::Model::BaseModel *> p_models );

		  protected:
			ModelDropArea( QWidget * p_parent );

			void _setupUi( const QString & p_name ) override;
			void _setupSlots() override;

			bool _hasFiltersToCheck() const;

			void dragEnterEvent( QDragEnterEvent * event ) override;
			void dropEvent( QDropEvent * event ) override;

		  private:
			std::vector<VTX::App::VTX_ID> _filters	  = std::vector<VTX::App::VTX_ID>();
			bool						 _acceptGroup = false;
		};
	} // namespace UI::Widget::CustomWidget
} // namespace VTX
#endif
