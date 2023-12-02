#include "_tests.hpp"

#include "ecs_view.hpp"

TEST_F(ECS_View, Basics)
{
	using namespace ECS;
	using Entity = EntityType;

	Registry registry;

	auto view_all = registry.GetView<char, int, float>();
	auto view_char = registry.GetView<char>();
	auto view_int = registry.GetView<int>();
	auto view_float = registry.GetView<float>();
	auto const_view = std::as_const(registry).GetView<const char>();

	const auto entity_1 = registry.Create();
	const auto entity_2 = registry.Create();
	const auto entity_3a = registry.Create();
	const auto entity_3b = registry.Create();
	const auto entity_3c = registry.Create();

	ASSERT_TRUE(view_all.Empty());
	ASSERT_TRUE(view_char.Empty());
	ASSERT_TRUE(view_int.Empty());
	ASSERT_TRUE(view_float.Empty());
	ASSERT_TRUE(const_view.Empty());

	registry.Emplace<char>(entity_1, 'X');

	registry.Emplace<char>(entity_2, 'Y');
	registry.Emplace<int>(entity_2, 0);

	registry.Emplace<char>(entity_3a, 'a');
	registry.Emplace<int>(entity_3a, 1);
	registry.Emplace<float>(entity_3a, 1.0f);

	registry.Emplace<char>(entity_3b, 'b');
	registry.Emplace<int>(entity_3b, 2);
	registry.Emplace<float>(entity_3b, 2.0f);

	registry.Emplace<char>(entity_3c, 'c');
	registry.Emplace<int>(entity_3c, 3);
	registry.Emplace<float>(entity_3c, 3.0f);

	////////////////////////////////////////////////////////////////////////////////

	view_all.Each
	(
		[](auto&& entity, auto&& a, auto&& b, auto&& c)
		{
			static_assert(std::is_same_v<decltype(entity), const EntityType&>);
			static_assert(std::is_same_v<decltype(a), char&>);
			static_assert(std::is_same_v<decltype(b), int&>);
			static_assert(std::is_same_v<decltype(c), float&>);
	
			//std::cerr << "[          ] view_all::entity = " << entity << std::endl;
		}
	);
	
	view_char.Each
	(
		[](auto&& entity, auto&& a)
		{
			static_assert(std::is_same_v<decltype(entity), const EntityType&>);
			static_assert(std::is_same_v<decltype(a), char&>);
	
			//std::cerr << "[          ] view_char::entity = " << entity << std::endl;
		}
	);
	
	view_int.Each
	(
		[](auto&& entity, auto&& b)
		{
			static_assert(std::is_same_v<decltype(entity), const EntityType&>);
			static_assert(std::is_same_v<decltype(b), int&>);
			
			//std::cerr << "[          ] view_int::entity = " << entity << std::endl;
		}
	);
	
	view_float.Each
	(
		[](auto&& entity, auto&& c)
		{
			static_assert(std::is_same_v<decltype(entity), const EntityType&>);
			static_assert(std::is_same_v<decltype(c), float&>);
	
			//std::cerr << "[          ] view_float::entity = " << entity << std::endl;
		}
	);
	
	const_view.Each
	(
		[](auto&& entity, auto&& d)
		{
			static_assert(std::is_same_v<decltype(entity), const EntityType&>);
			static_assert(std::is_same_v<decltype(d), const char&>);
	
			//std::cerr << "[          ] const_view::entity = " << entity << std::endl;
		}
	);
}

TEST_F(ECS_View, Access)
{
	using namespace ECS;
	using Entity = EntityType;

	Registry registry;

	auto view_a = registry.GetView<char>();
	auto view_b = registry.GetView<char, int>();
	auto view_c = registry.GetView<char, int, float>();

	const auto entity_1 = registry.Create();
	const auto entity_2a = registry.Create();
	const auto entity_2b = registry.Create();
	const auto entity_3a = registry.Create();
	const auto entity_3b = registry.Create();
	const auto entity_3c = registry.Create();

	registry.Emplace<char>(entity_1, 'z');

	registry.Emplace<char>(entity_2a, 'x');
	registry.Emplace<char>(entity_2b, 'y');
	registry.Emplace<int>(entity_2a, 111);
	registry.Emplace<int>(entity_2b, 222);

	registry.Emplace<char>(entity_3a, 'a');
	registry.Emplace<char>(entity_3b, 'b');
	registry.Emplace<char>(entity_3c, 'c');
	registry.Emplace<int>(entity_3a, 1);
	registry.Emplace<int>(entity_3b, 2);
	registry.Emplace<int>(entity_3c, 3);
	registry.Emplace<float>(entity_3a, 1.1f);
	registry.Emplace<float>(entity_3b, 2.2f);
	registry.Emplace<float>(entity_3c, 3.3f);

	////////////////////////////////////////////////////////////////////////////////

	auto size_a_char = view_a.Size<char>();
	auto size_b_char = view_b.Size<char>();
	auto size_c_char = view_c.Size<char>();

	ASSERT_EQ(size_a_char, size_a_char);
	ASSERT_EQ(size_a_char, size_b_char);
	ASSERT_EQ(size_a_char, size_c_char);

	auto size_b_int = view_b.Size<int>();
	auto size_c_int = view_c.Size<int>();

	ASSERT_EQ(size_b_int, size_b_int);
	ASSERT_EQ(size_b_int, size_c_int);

	////////////////////////////////////////////////////////////////////////////////

	auto va_2a_char = view_a.Get<char>(entity_2a);

	auto vb_2a_char = view_b.Get<char>(entity_2a);
	auto vb_2a_int = view_b.Get<int>(entity_2a);
	auto vb_2a_all = view_b.Get<char, int>(entity_2a);

	auto vc_2a_char = view_b.Get<char>(entity_2a);
	auto vc_2a_int = view_b.Get<int>(entity_2a);
	auto vc_2a_all = view_b.Get<char, int>(entity_2a);

	ASSERT_EQ(va_2a_char, va_2a_char);
	ASSERT_EQ(va_2a_char, vb_2a_char);
	ASSERT_EQ(va_2a_char, vc_2a_char);

	ASSERT_EQ(vb_2a_int, vb_2a_int);
	ASSERT_EQ(vb_2a_int, vc_2a_int);

	ASSERT_EQ(vb_2a_all, vb_2a_all);
	ASSERT_EQ(vb_2a_all, vc_2a_all);
}

TEST_F(ECS_View, Order)
{
	using namespace ECS;
	using Entity = EntityType;

	Registry registry;

	//std::vector<int> vector(100);
	//std::iota(std::begin(vector), std::end(vector), 100);
	//registry.Create(std::begin(vector), std::end(vector));
}