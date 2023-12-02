#include "_tests.hpp"

#include "ecs_registry.hpp"

TEST_F(ECS_Registry, Storage)
{
	Registry registry;

	ASSERT_EQ(registry.Size(), 0);
	ASSERT_EQ(registry.Alive(), 0);

	ASSERT_NO_THROW((registry.Reserve<int, char>(16)));
	ASSERT_NO_THROW(registry.Reserve(16));

	ASSERT_TRUE(registry.Empty());

	ASSERT_EQ(registry.Capacity(), 16);

	ASSERT_EQ(registry.Capacity<int>(), 16);
	ASSERT_EQ(registry.Capacity<char>(), 16);

	ASSERT_EQ(registry.Size<int>(), 0);
	ASSERT_EQ(registry.Size<char>(), 0);

	ASSERT_TRUE((registry.Empty<int, char>()));
}

TEST_F(ECS_Registry, Entity)
{
	Registry registry;

	//registry.Prepare<double>();

	const auto entity_a = registry.Create();
	const auto entity_b = registry.Create();

	////////////////////////////////////////////////////////////////////////////////

	registry.Emplace<int>(entity_b);
	registry.Emplace<char>(entity_b);

	ASSERT_TRUE(registry.Has<>(entity_a));
	ASSERT_FALSE(registry.Any<>(entity_b));

	ASSERT_EQ(registry.Size<int>(), 1);
	ASSERT_EQ(registry.Size<char>(), 1);

	ASSERT_FALSE(registry.Empty<int>());
	ASSERT_FALSE(registry.Empty<char>());

	ASSERT_FALSE((registry.Empty<int, char>()));

	ASSERT_NE(entity_a, entity_b);

	////////////////////////////////////////////////////////////////////////////////

	ASSERT_FALSE((registry.Has<int, char>(entity_a)));
	ASSERT_TRUE((registry.Has<int, char>(entity_b)));
	ASSERT_FALSE((registry.Any<int, double>(entity_a)));
	ASSERT_TRUE((registry.Any<int, double>(entity_b)));

	registry.Emplace<int>(entity_a, 16);
	ASSERT_EQ(registry.Get<int>(entity_a), 16);

	registry.Emplace<char>(entity_a, '6');
	ASSERT_EQ(registry.Get<char>(entity_a), '6');

	ASSERT_NO_THROW(registry.Remove<int>(entity_b));
	ASSERT_NO_THROW(registry.Remove<char>(entity_b));
	ASSERT_TRUE((registry.Has<int, char>(entity_a)));
	ASSERT_FALSE((registry.Has<int, char>(entity_b)));
	ASSERT_TRUE((registry.Any<int, double>(entity_a)));
	ASSERT_FALSE((registry.Any<int, double>(entity_b)));
}