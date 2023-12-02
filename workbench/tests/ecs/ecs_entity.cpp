#include "_tests.hpp"

#include "ecs_entity.hpp"

TEST_F(ECS_Entity, Basics)
{
	ASSERT_FALSE(Entity{} == EntityInvalid);

	ASSERT_TRUE(Entity(EntityTraits::EntityMask) == EntityInvalid);

	ASSERT_TRUE(Entity( ~typename EntityTraits::EntityType{} ) == EntityInvalid);

	ASSERT_TRUE(EntityInvalid == EntityInvalid);
	ASSERT_FALSE(EntityInvalid != EntityInvalid);

	////////////////////////////////////////////////////////////////////////////////

	Registry registry;

	auto entity = registry.Create();

	ASSERT_FALSE(entity == EntityInvalid);
	ASSERT_FALSE(EntityInvalid == entity);
	
	ASSERT_TRUE(entity != EntityInvalid);
	ASSERT_TRUE(EntityInvalid != entity);
	
	ASSERT_TRUE(registry.Valid(entity));
	registry.Destroy(entity);
	ASSERT_FALSE(registry.Valid(entity));
	
	////////////////////////////////////////////////////////////////////////////////
	
	entity = registry.Create();
	registry.Emplace<int>(entity, 16);

	ASSERT_EQ(registry.Get<int>(entity), 16);
}

TEST_F(ECS_Entity, Versioning)
{
	Registry registry;
	EntityType entities[3];

	ASSERT_FALSE(registry.Valid(entities[0]));
	ASSERT_FALSE(registry.Valid(entities[1]));
	ASSERT_FALSE(registry.Valid(entities[2]));

	auto entity_a = registry.Create();

	registry.Destroy(registry.Create());
	registry.Destroy(entity_a);
	registry.Destroy(registry.Create());

	ASSERT_FALSE(registry.Valid(entities[0]));
	ASSERT_FALSE(registry.Valid(entities[1]));
	ASSERT_FALSE(registry.Valid(entities[2]));

	registry.Create(std::begin(entities), std::end(entities));

	ASSERT_TRUE(registry.Valid(entities[0]));
	ASSERT_TRUE(registry.Valid(entities[1]));
	ASSERT_TRUE(registry.Valid(entities[2]));

	ASSERT_EQ(registry.Entity(entities[0]), EntityType(0));
	ASSERT_EQ(registry.Version(entities[0]), VersionType(2));

	ASSERT_EQ(registry.Entity(entities[1]), EntityType(1));
	ASSERT_EQ(registry.Version(entities[1]), VersionType(1));

	ASSERT_EQ(registry.Entity(entities[2]), EntityType(2));
	ASSERT_EQ(registry.Version(entities[2]), VersionType(0));
}

TEST_F(ECS_Entity, Identifiers)
{
	Registry registry;

	const auto pre = registry.Create();

	ASSERT_EQ(pre, registry.Entity(pre));

	registry.Destroy(pre);
	const auto post = registry.Create();

	ASSERT_NE(pre, post);
	ASSERT_EQ(Registry::Entity(pre), Registry::Entity(post));
	ASSERT_NE(Registry::Version(pre), Registry::Version(post));
	ASSERT_NE(registry.Version(pre), registry.Current(pre));
	ASSERT_EQ(registry.Version(post), registry.Current(post));
}

TEST_F(ECS_Entity, CreateDestroy_Basic)
{
	Registry registry;
	EntityType pre{};
	EntityType post{};

	for (auto index = 0; index < 10; ++index)
	{
		const auto entity = registry.Create();
		registry.Emplace<double>(entity);
	}

	registry.Clear();

	for (auto index = 0; index < 7; ++index)
	{
		const auto entity = registry.Create();
		registry.Emplace<int>(entity);

		if (index == 3) { pre = entity; }
	}

	registry.Clear();

	for (auto index = 0; index < 5; ++index)
	{
		const auto entity = registry.Create();

		if (index == 3) { post = entity; }
	}

	ASSERT_FALSE(registry.Valid(pre));
	ASSERT_TRUE(registry.Valid(post));
	ASSERT_NE(registry.Version(pre), registry.Version(post));
	ASSERT_EQ(registry.Version(pre) + 1, registry.Version(post));
	ASSERT_EQ(registry.Current(pre), registry.Current(post));
}

TEST_F(ECS_Entity, CreateDestroy)
{
	Registry registry;

	const auto entity_a = registry.Create();
	const auto entity_b = registry.Create();

	registry.Destroy(entity_a);
	registry.Destroy(entity_b);

	registry.Each([](auto) { FAIL(); });

	ASSERT_EQ(registry.Current(entity_a), VersionType(1));
	ASSERT_EQ(registry.Current(entity_b), VersionType(1));
}

TEST_F(ECS_Entity, Destroy)
{
	Registry registry;

	const auto entity_a = registry.Create();
	const auto entity_b = registry.Create();

	ASSERT_EQ(registry.Current(entity_a), VersionType{});
	ASSERT_EQ(registry.Current(entity_b), VersionType{});

	registry.Destroy(entity_a);
	registry.Destroy(entity_b, 3);

	ASSERT_EQ(registry.Current(entity_a), VersionType(1));
	ASSERT_EQ(registry.Current(entity_b), VersionType(3));
}

TEST_F(ECS_Entity, VersionOverflow)
{
	Registry registry;

	const auto entity = registry.Create();

	registry.Destroy(entity, VersionType(VersionMask));
	registry.Destroy(registry.Create());

	ASSERT_EQ(registry.Current(entity), registry.Version(entity));
}

TEST_F(ECS_Entity, Each)
{
	Registry registry;
	std::size_t total;
	std::size_t match;

	registry.Create();
	registry.Emplace<int>(registry.Create());
	registry.Create();
	registry.Emplace<int>(registry.Create());
	registry.Create();

	total = 0u;
	match = 0u;

	registry.Each
	(
		[&](auto entity)
		{
			if (registry.Has<int>(entity))
			{
				++match;
			}

			registry.Create();
			++total;
		}
	);

	ASSERT_EQ(total, 5u);
	ASSERT_EQ(match, 2u);

	total = 0u;
	match = 0u;

	registry.Each
	(
		[&](auto entity)
		{
			if (registry.Has<int>(entity))
			{
				registry.Destroy(entity);
				++match;
			}
			++total;
		}
	);

	ASSERT_EQ(total, 10u);
	ASSERT_EQ(match, 2u);

	total = 0u;
	match = 0u;

	registry.Each
	(
		[&](auto entity)
		{
			if (registry.Has<int>(entity))
			{
				++match;
			}

			registry.Destroy(entity);
			++total;
		}
	);

	ASSERT_EQ(total, 8u);
	ASSERT_EQ(match, 0u);

	registry.Each([&](auto) { FAIL(); });
}

TEST_F(ECS_Entity, Orphans)
{
	Registry registry;
	std::size_t total = 0;

	registry.Emplace<int>(registry.Create());
	registry.Create();
	registry.Emplace<int>(registry.Create());

	registry.Orphans([&](auto) { ++total; });
	ASSERT_EQ(total, 1u);

	total = 0;

	registry.Each([&](auto entity) { registry.RemoveAll(entity); });
	registry.Orphans([&](auto) { ++total; });
	ASSERT_EQ(total, 3u);

	registry.Clear();

	total = 0;

	registry.Orphans([&](auto) { ++total; });
	ASSERT_EQ(total, 0u);
}

TEST_F(ECS_Entity, Get)
{
	struct EmptyType
	{
	};

	Registry registry;

	EntityType entities[3];
	registry.Create(std::begin(entities), std::end(entities));

	int val = 42;
	int values[3];

	registry.Emplace<int>(entities[0], val++);
	registry.Emplace<int>(entities[1], val++);
	registry.Emplace<int>(entities[2], val++);

	values[0] = registry.Get<int>(entities[0]);
	values[1] = registry.Get<int>(entities[1]);
	values[2] = registry.Get<int>(entities[2]);

	ASSERT_EQ(values[0], (registry.Get<int>(entities[0])));
	ASSERT_EQ(values[1], (registry.Get<int>(entities[1])));
	ASSERT_EQ(values[2], (registry.Get<int>(entities[2])));

	registry.Emplace<EmptyType>(entities[0]);
	registry.Emplace<EmptyType>(entities[1]);
	registry.Emplace<EmptyType>(entities[2]);

	ASSERT_EQ(values[2], --val);
	ASSERT_EQ(values[1], --val);
	ASSERT_EQ(values[0], --val);

	registry.Remove<int>(entities[1]);

	ASSERT_NE(registry.TryGet<int>(entities[0]), nullptr);
	ASSERT_EQ(registry.TryGet<int>(entities[1]), nullptr);
	ASSERT_NE(registry.TryGet<int>(entities[2]), nullptr);
}

TEST_F(ECS_Entity, ClearRemove)
{
	struct EmptyType
	{
	};

	Registry registry;

	EntityType entities[3];
	registry.Create(std::begin(entities), std::end(entities));

	registry.Emplace<int>(entities[0], 0);
	registry.Emplace<int>(entities[1], 1);
	registry.Emplace<int>(entities[2], 2);

	registry.Emplace<char>(entities[0], 'a');
	registry.Emplace<char>(entities[1], 'b');
	registry.Emplace<char>(entities[2], 'c');

	ASSERT_TRUE(registry.Valid(entities[0]));
	ASSERT_TRUE(registry.Valid(entities[1]));
	ASSERT_TRUE(registry.Valid(entities[2]));

	ASSERT_NE(registry.TryGet<int>(entities[0]), nullptr);
	ASSERT_NE(registry.TryGet<int>(entities[1]), nullptr);
	ASSERT_NE(registry.TryGet<int>(entities[2]), nullptr);

	registry.Clear();

	ASSERT_FALSE(registry.Valid(entities[0]));
	ASSERT_FALSE(registry.Valid(entities[1]));
	ASSERT_FALSE(registry.Valid(entities[2]));

	registry.Create(std::begin(entities), std::end(entities));

	ASSERT_TRUE(registry.Valid(entities[0]));
	ASSERT_TRUE(registry.Valid(entities[1]));
	ASSERT_TRUE(registry.Valid(entities[2]));

	ASSERT_EQ(registry.TryGet<int>(entities[0]), nullptr);
	ASSERT_EQ(registry.TryGet<int>(entities[1]), nullptr);
	ASSERT_EQ(registry.TryGet<int>(entities[2]), nullptr);

	registry.Emplace<int>(entities[0], 0);
	registry.Emplace<int>(entities[1], 1);
	registry.Emplace<int>(entities[2], 2);

	registry.Emplace<char>(entities[0], 'a');
	registry.Emplace<char>(entities[1], 'b');
	registry.Emplace<char>(entities[2], 'c');

	ASSERT_NE(registry.TryGet<int>(entities[0]), nullptr);
	ASSERT_NE(registry.TryGet<int>(entities[1]), nullptr);
	ASSERT_NE(registry.TryGet<int>(entities[2]), nullptr);

	registry.Clear<int>();

	ASSERT_EQ(registry.TryGet<int>(entities[0]), nullptr);
	ASSERT_EQ(registry.TryGet<int>(entities[1]), nullptr);
	ASSERT_EQ(registry.TryGet<int>(entities[2]), nullptr);

	ASSERT_NE(registry.TryGet<char>(entities[0]), nullptr);
	ASSERT_NE(registry.TryGet<char>(entities[1]), nullptr);
	ASSERT_NE(registry.TryGet<char>(entities[2]), nullptr);

	registry.Emplace<int>(entities[0], 0);
	registry.Emplace<int>(entities[1], 1);
	registry.Emplace<int>(entities[2], 2);

	ASSERT_NE(registry.TryGet<int>(entities[0]), nullptr);
	ASSERT_NE(registry.TryGet<int>(entities[1]), nullptr);
	ASSERT_NE(registry.TryGet<int>(entities[2]), nullptr);

	registry.Remove<int>(std::begin(entities) + 1, std::end(entities));
	registry.Remove<char>(std::begin(entities), std::end(entities) - 1);

	ASSERT_NE(registry.TryGet<int>(entities[0]), nullptr);
	ASSERT_EQ(registry.TryGet<int>(entities[1]), nullptr);
	ASSERT_EQ(registry.TryGet<int>(entities[2]), nullptr);

	ASSERT_EQ(registry.TryGet<char>(entities[0]), nullptr);
	ASSERT_EQ(registry.TryGet<char>(entities[1]), nullptr);
	ASSERT_NE(registry.TryGet<char>(entities[2]), nullptr);

	ASSERT_EQ(registry.Get<int>(entities[0]), 0);
	ASSERT_EQ(registry.Get<char>(entities[2]), 'c');
}