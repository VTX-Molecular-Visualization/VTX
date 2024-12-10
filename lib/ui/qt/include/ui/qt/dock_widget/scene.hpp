#ifndef __VTX_UI_QT_DOCK_WIDGET_SCENE__
#define __VTX_UI_QT_DOCK_WIDGET_SCENE__

#include "ui/qt/core/base_dock_widget.hpp"
#include <QPointer>
#include <QTreeWidget>

namespace VTX::UI::QT::DockWidget
{
	/*
	struct Data
	{
		uint level;
	};

	// Static polymorphism.
	template<typename T>
	concept TreeItemData = requires( T t ) {
		{ t.getName() } -> std::same_as<const std::string_view>;
		{ t.getPersistentSceneID() } -> std::same_as<int>;
	};

	// Test structs.
	struct TestData
	{
		std::string_view name		  = "Default system";
		int				 persistentId = 0;

		const std::string_view getName() const { return name; }
		int					   getPersistentSceneID() const { return persistentId; }
	};

	struct TestData2
	{
		std::string_view myName = "Default system adapted";
		int				 myID	= 0;

		const std::string_view getMyName() const { return myName; }
		int					   getMyID() const { return myID; }
	};

	// Adapter.
	class BaseAdapterTreeItemData
	{
	  public:
		virtual const std::string_view name() const = 0;
		virtual int					   id() const	= 0;
		virtual ~BaseAdapterTreeItemData()			= default;
	};

	class AdapterSceneItemComponent : public BaseAdapterTreeItemData
	{
	  public:
		AdapterSceneItemComponent( const VTX::App::Component::Scene::SceneItemComponent & p_item ) : _item( p_item ) {}

		const std::string_view name() const override { return _item.getName(); }
		int					   id() const override { return _item.getPersistentSceneID(); }

	  private:
		const VTX::App::Component::Scene::SceneItemComponent & _item;
	};

	class AdapterTestData2 : public BaseAdapterTreeItemData
	{
	  public:
		AdapterTestData2( const TestData2 & p_item ) : _item( p_item ) {}

		const std::string_view name() const override { return _item.getMyName(); }
		int					   id() const override { return _item.getMyID(); }

	  private:
		const TestData2 & _item;
	};
	*/

	class Scene : public Core::BaseDockWidget<Scene>
	{
	  public:
		using WidgetData = size_t;
		struct TreeItemData
		{
			std::string_view name;
			WidgetData		 data;
			size_t			 childrenCount;
		};
		using LoadFunc = std::function<void( const uint, QTreeWidgetItem * const )>;

		Scene( QWidget * );

	  private:
		QPointer<QTreeWidget>				  _tree;
		std::map<QTreeWidgetItem *, LoadFunc> _loadFuncs;

		void _addTreeItem( const TreeItemData &, std::variant<const LoadFunc, QTreeWidgetItem * const> );
		void _resetTreeItem( QTreeWidgetItem * const );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
