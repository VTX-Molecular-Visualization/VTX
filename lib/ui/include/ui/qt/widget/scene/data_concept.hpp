#include <app/component/scene/scene_item_component.hpp>
#include <concepts>
#include <string>

namespace VTX::UI::QT::Widget
{

	struct Data
	{
		uint level;
	};

	// Static polymorphism.
	template<typename T>
	concept TreeItemData = requires( T t ) {
		{
			t.getName()
		} -> std::same_as<const std::string &>;
		{
			t.getPersistentSceneID()
		} -> std::same_as<int>;
	};

	// Test structs.
	struct TestData
	{
		std::string name		 = "Default molecule";
		int			persistentId = 0;

		const std::string & getName() const { return name; }
		int					getPersistentSceneID() const { return persistentId; }
	};

	struct TestData2
	{
		std::string myName = "Default molecule adapted";
		int			myID   = 0;

		const std::string & getMyName() const { return myName; }
		int					getMyID() const { return myID; }
	};

	// Adapter.
	class BaseAdapterTreeItemData
	{
	  public:
		virtual const std::string & name() const = 0;
		virtual int					id() const	 = 0;
		virtual ~BaseAdapterTreeItemData()		 = default;
	};

	class AdapterSceneItemComponent : public BaseAdapterTreeItemData
	{
	  public:
		AdapterSceneItemComponent( const VTX::App::Component::Scene::SceneItemComponent & p_item ) : _item( p_item ) {}

		const std::string & name() const override { return _item.getName(); }
		int					id() const override { return _item.getPersistentSceneID(); }

	  private:
		const VTX::App::Component::Scene::SceneItemComponent & _item;
	};

	class AdapterTestData2 : public BaseAdapterTreeItemData
	{
	  public:
		AdapterTestData2( const TestData2 & p_item ) : _item( p_item ) {}

		const std::string & name() const override { return _item.getMyName(); }
		int					id() const override { return _item.getMyID(); }

	  private:
		const TestData2 & _item;
	};

} // namespace VTX::UI::QT::Widget