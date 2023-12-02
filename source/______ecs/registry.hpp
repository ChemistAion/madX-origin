#pragma once

#include "pool.hpp"
#include "view.hpp"
#include "system.hpp"

namespace madX::ECS
{
	class Registry final : public Core::Singleton<Registry>
	{
		friend Core::Singleton<Registry>;

		using StorageType = Core::SparseSet<EntityType>;
		using StorageDescriptor = std::pair<Core::TypeInfoType, std::unique_ptr<StorageType>>;
		mutable std::vector<StorageDescriptor> storages_;
	
		//Core::SparseMap<EntityType, FeatureType> features_;
		std::vector<std::unique_ptr<System<>>> systems_;
	
		std::vector<EntityType> entities_;
		EntityType destroyed_ { EntityInvalid };

		////////////////////////////////////////////////////////////////////////////////

		template <typename Entity, typename Component>
		[[nodiscard]] const PoolType<Entity, Component>& GetStorage() const
		{
			const StorageType* storage;
	
			const auto index = Core::TypeIndex<Registry>::Value<Component>();
	
			if (false == (index < storages_.size()))
			{
				storages_.resize(std::size_t(index) + 1u);
			}
	
			if (auto& [type_id, pool] = storages_[index]; !pool)
			{
				type_id = Core::TypeInfo<Component>::Value();
				pool.reset(new PoolType<Entity, Component>());
			}
	
			storage = storages_[index].second.get();
	
			return *static_cast<const PoolType<Entity, Component>*>(storage);
		}
	
		template <typename Entity, typename Component>
		[[nodiscard]] PoolType<Entity, Component>& GetStorage()
		{
			return const_cast<PoolType<Entity, Component>&>(std::as_const(*this).template GetStorage<Entity, Component>());
		}

	public:
		Registry() = default;

		Registry(Registry&&) noexcept = default;
		Registry& operator=(Registry&&) noexcept = default;

		////////////////////////////////////////////////////////////////////////////////

		[[nodiscard]] bool Valid(const EntityType entity) const
		{
			const auto index = Indexer(entity);
			return (index < entities_.size() && entities_[index] == entity);
		}

		[[nodiscard]] static EntityType Entity(const EntityType entity) noexcept
		{
			return EntityType(entity & EntityMask);
		}

		[[nodiscard]] static VersionType Version(const EntityType entity) noexcept
		{
			return VersionType(entity >> MaskShift);
		}

		[[nodiscard]] VersionType Current(const EntityType entity) noexcept
		{
			const auto index = Indexer(entity);
			assert(index < entities_.size());

			return VersionType(entities_[index] >> MaskShift);
		}

		////////////////////////////////////////////////////////////////////////////////

		template <typename Component>
		[[nodiscard]] auto Size() const noexcept
		{
			return GetStorage<EntityType, Component>().Size();
		}

		[[nodiscard]] auto Size() const noexcept
		{
			return entities_.size();
		}

		[[nodiscard]] auto Alive() const -> std::size_t
		{
			auto size = Size();
			auto current = destroyed_;

			for (; current != EntityInvalid; --size)
			{
				current = entities_[current & EntityMask];
			}

			return size;
		}

		template <typename... Component>
		[[nodiscard]] bool Empty() const
		{
			if constexpr (sizeof...(Component) == 0)
			{
				return Alive() == 0;
			}
			else
			{
				return (GetStorage<EntityType, Component>().Empty() && ...);
			}
		}

		template <typename... Component>
		void Reserve(const std::size_t capacity)
		{
			if constexpr (sizeof...(Component) == 0)
			{
				entities_.reserve(capacity);
			}
			else
			{
				(GetStorage<EntityType, Component>().Reserve(capacity), ...);
			}
		}

		template <typename Component>
		[[nodiscard]] auto Capacity() const noexcept
		{
			return GetStorage<EntityType, Component>().Capacity();
		}

		[[nodiscard]] auto Capacity() const noexcept
		{
			return entities_.capacity();
		}

		////////////////////////////////////////////////////////////////////////////////

		EntityType Create()
		{
			EntityType entity;

			if (destroyed_ == EntityInvalid)
			{
				entity = entities_.emplace_back(static_cast<EntityType>(entities_.size()));
				assert(entity < EntityMask);
			}
			else
			{
				const auto index = destroyed_;
				const auto version = (entities_[index]) & (VersionMask << MaskShift);
				destroyed_ = EntityType(entities_[index] & EntityMask);
				entity = entities_[index] = EntityType(version | index);
			}

			return entity;
		}

		template <typename Iterator>
		void Create(Iterator first, Iterator last)
		{
			std::generate
			(
				first, last,
				[this]() { return Create(); }
			);
		}

		void RemoveAll(const EntityType entity)
		{
			assert(Valid(entity));

			for (auto index = storages_.size(); index; --index)
			{
				if (auto& [type_id, storage] = storages_[index - 1]; storage && storage->Contains(entity))
				{
					storage->Erase(entity);
				}
			}
		}

		//TODO: private
		void Destroy(const EntityType entity, const VersionType version)
		{
			RemoveAll(entity);

			const auto index = Indexer(entity);

			entities_[index] = EntityType(destroyed_ | (EntityType(version) << MaskShift));
			destroyed_ = EntityType(index);
		}

		void Destroy(const EntityType entity)
		{
			Destroy(entity, (VersionType(entity >> MaskShift) + 1));
		}

		template <typename Iterator>
		void Destroy(Iterator first, Iterator last)
		{
			while (first != last)
			{
				Destroy(*(first++));
			}
		}

		//TODO: check Functor&&
		template<typename Functor>
		void Each(Functor functor) const
		{
			if (destroyed_ == EntityInvalid)
			{
				for (auto index = entities_.size(); index; --index)
				{
					functor(entities_[index - 1]);
				}
			}
			else
			{
				for (auto index = entities_.size(); index; --index)
				{
					if (const auto entity = entities_[index - 1]; (entity & EntityMask) == (index - 1))
					{
						functor(entity);
					}
				}
			}
		}

		[[nodiscard]] bool Orphan(const EntityType entity) const
		{
			assert(Valid(entity));

			return std::none_of
			(
				storages_.cbegin(), storages_.cend(),
				[entity](auto&& desc) { return desc.second && desc.second->Contains(entity); }
			);
		}

		//TODO: check Functor&&
		template<typename Functor>
		void Orphans(Functor functor) const
		{
			Each
			(
				[this, &functor](const auto entity)
				{
					if (Orphan(entity))
					{
						functor(entity);
					}
				}
			);
		}

		////////////////////////////////////////////////////////////////////////////////

		//TODO: ctor must be defined 
		template <typename Component, typename... Arguments>
		[[maybe_unused]] EntityType Emplace(const EntityType entity, Arguments&&... arguments)
		{
			assert(Valid(entity));
			//TODO: has value already?
			GetStorage<EntityType, Component>().Emplace(entity, std::forward<Arguments>(arguments)...);
			
			return entity;
		}

		template <typename... Component>
		void Remove(const EntityType entity)
		{
			assert(Valid(entity));
			(GetStorage<EntityType, Component>().Erase(entity), ...);
		}

		template<typename... Component, typename Iterator>
		void Remove(Iterator first, Iterator last)
		{
			assert(std::all_of(first, last, [this](const auto entity) { return Valid(entity); }));
			(GetStorage<EntityType, Component>().Remove(first, last), ...);
		}

		template <typename... Component>
		void Clear()
		{
			if constexpr (sizeof...(Component) == 0)
			{
				Each([this](const auto entity) { this->Destroy(entity); });
			}
			else
			{
				([](auto&& pool){ pool.Remove(pool.SparseSet<EntityType>::begin(), pool.SparseSet<EntityType>::end()); }(GetStorage<EntityType, Component>()), ...);
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		template <typename... Component>
		[[nodiscard]] bool Has(const EntityType entity) const
		{
			assert(Valid(entity));
			return (GetStorage<EntityType, Component>().Contains(entity) && ...);
		}

		template <typename... Component>
		[[nodiscard]] bool Any(const EntityType entity) const
		{
			assert(Valid(entity));
			return (GetStorage<EntityType, Component>().Contains(entity) || ...);
		}

		template <typename... Component>
		[[nodiscard]] decltype(auto) Get(const EntityType entity) const
		{
			assert(Valid(entity));

			if constexpr (sizeof...(Component) == 1)
			{
				return (GetStorage<EntityType, Component>().Get(entity), ...);
			}
			else
			{
				return std::forward_as_tuple(Get<Component>(entity)...);
			}
		}

		template <typename... Component>
		[[nodiscard]] decltype(auto) Get(const EntityType entity)
		{
			assert(Valid(entity));

			if constexpr (sizeof...(Component) == 1)
			{
				return (GetStorage<EntityType, Component>().Get(entity), ...);
			}
			else
			{
				return std::forward_as_tuple(Get<Component>(entity)...);
			}
		}

		template<typename... Component>
		[[nodiscard]] auto TryGet(const EntityType entity) const
		{
			assert(Valid(entity));

			if constexpr (sizeof...(Component) == 1)
			{
				return (GetStorage<EntityType, Component>().TryGet(entity), ...);
			}
			else
			{
				return std::make_tuple(TryGet<Component>(entity)...);
			}
		}

		template<typename... Component>
		[[nodiscard]] auto TryGet(const EntityType entity)
		{
			assert(Valid(entity));

			if constexpr (sizeof...(Component) == 1)
			{
				return (GetStorage<EntityType, Component>().TryGet(entity), ...);
			}
			else
			{
				return std::make_tuple(TryGet<Component>(entity)...);
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		//TODO: check if System
		template <typename Type, typename... Arguments>
		auto Mount(Arguments&&... arguments)
		{
			systems_.emplace_back(std::make_unique<Type>(std::forward<Arguments>(arguments)...));
			return dynamic_cast<Type*>(systems_.back().get());
		}

		//TODO: consider noexcept, const it
		template <typename Event>
		void Process(Event&& event)
		{
			using SystemType = System<std::decay_t<decltype(event)>>;

			for (const auto& base_system : systems_)
			{		
				if (auto system = dynamic_cast<SystemType*>(base_system.get()))
				{
					system->Process(*this, std::forward<Event>(event));
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		template<typename... Component>
		[[nodiscard]] View<EntityType, Component...> GetView() const
		{
			static_assert(sizeof...(Component) > 0);
			return { GetStorage<EntityType, std::decay_t<Component>>()... };
		}
		
		template<typename... Component>
		[[nodiscard]] View<EntityType, Component...> GetView()
		{
			static_assert(sizeof...(Component) > 0);
			return { GetStorage<EntityType, std::decay_t<Component>>()... };
		}

		////////////////////////////////////////////////////////////////////////////////

		//TODO: one Component only?
		template <typename... Component>
		void Sort()
		{
			static_assert(sizeof...(Component) > 0);
			(GetStorage<EntityType, std::decay_t<Component>>().Sort(), ...);
			
			return;
		}
	};
}