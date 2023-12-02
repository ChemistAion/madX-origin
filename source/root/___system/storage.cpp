#include "_root.hpp"

namespace madX::Root::System
{
 	//TODO: names to Event::Touch
	void Storage::Process(ECS::Registry& registry, Event::Touch&&)
	{
		timer_ += update_;
	}

	void Storage::Process(ECS::Registry& registry, Event::Tick&& tick)
	{
		timer_ += tick.delta_;
	}

	void Storage::Process(ECS::Registry& registry, Event::Process&&)
	{
		if (timer_ > update_) [[unlikely]]
		{
			registry.GetView<Component::Volume>().Each
			(
				[&registry, this](const auto entity, Component::Volume& volume)
				{
					if (volume.IsValid())
					{
						return;
					}

					auto [task, future] = Core::Task::CreateTask(&Component::Volume::Touch, volume);

					volume.GetFuture() = std::move(future);
					registry.Process(std::move(task));
				}
			);

			for (; timer_ > update_; timer_ -= update_);

			return; // enough work, process result next time
		}

		////////////////////////////////////////////////////////////////////////////////

		registry.GetView<Component::Volume>().Each
		(
			[&registry, this](const auto, Component::Volume& volume)
			{
				if (false == volume.IsReady()) [[likely]]
				{
					return;
				}

				auto logger = madX::Core::Logger::GetInstance();

				const auto& name = volume.GetName();

				for (auto taken = std::move(volume.GetFuture()); const auto& element : taken.Get())
				{			
					const auto& [state, route, path] = element;

					if (state == State::Created)
					{
						const auto entity = registry.Emplace<Component::File>(registry.Create(), element);
						const auto& insertion = files_.try_emplace(path, entity);
				
						if (false == insertion.second)
						{
							registry.Destroy(entity);

							auto logger = madX::Core::Logger::GetInstance();
							logger->Error(name + " [ERROR] Already created: " + route);
						}
						else
						{
							logger->Info(name + " [C] " + route);
						}				
				
						continue;
					}
				
					////////////////////////////////////////////////////////////////////////////////
				
					if (auto file = files_.find(path); file == files_.end()) [[unlikely]]
					{
						auto logger = madX::Core::Logger::GetInstance();
						logger->Error(name + " [ERROR] Cannot find: " + route);
						//TODO: ofrans check...					
					}
					else
					{
						registry.Get<Component::File>(file->second).state_ = state;
				
						if (state == State::Modified)
						{
							logger->Info(name + " [M] " + route);
							//TODO: do somewthing...
						}
						else if (state == State::Unknown)
						{						
							logger->Info(name + " [U] " + route);

							const auto entity = file->second;
							registry.Destroy(entity);

							files_.erase(path);
						}
					}

					////////////////////////////////////////////////////////////////////////////////
				}		
			}		
		);
	}

	void Storage::Process(ECS::Registry& registry, Event::Terminate&&)
	{
		//TODO: all the file savings
	}
}