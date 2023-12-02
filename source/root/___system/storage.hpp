#pragma once

namespace madX::Root::System
{
	constexpr double cSystem_StorageDefaultUpdateRate = 1.0;

	class Storage final : public ECS::System<Event::Touch, Event::Tick, Event::Process, Event::Terminate>
	{
		using State = Core::Filesystem::EntryState;
		
		//TODO: std::chrono me
		double timer_ = 0.0;
		const double update_ = cSystem_StorageDefaultUpdateRate;

		std::map<std::filesystem::path, ECS::EntityType> files_;

		bool Contains(const ECS::Registry& registry, const Component::File& file) const noexcept
		{
			bool contains = false;

			//	[&registry, this](const auto entity, Component::Volume& volume)
			//	{
			//	}

			return contains;
		}
	public:
		explicit Storage(double update = cSystem_StorageDefaultUpdateRate)
			: update_{ update }
		{
		}

		void Process(ECS::Registry& registry, Event::Touch&&) override;
		void Process(ECS::Registry& registry, Event::Tick&&) override;
		void Process(ECS::Registry& registry, Event::Process&&) override;
		void Process(ECS::Registry& registry, Event::Terminate&&) override;
	};
}