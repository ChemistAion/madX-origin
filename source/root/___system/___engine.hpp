#pragma once

#include "engine/renderer.hpp"

namespace madX::Root::System
{
	class Engine final : public ECS::System<Event::Create, Event::Resize, Event::Process, Event::Frame::Update, Event::Frame::Begin, Event::Frame::Render, Event::Frame::End, Event::Terminate>
	{
		std::shared_ptr<madX::Engine::Renderer> renderer_;
		std::shared_ptr<madX::Engine::SwapChain> swapchain_;

		ID3D12GraphicsCommandList2* command_list_ = nullptr;

		std::vector<ECS::EntityType> pass_;

		std::unique_ptr<Component::Editor> editor_;

		static LRESULT Pump(HWND handle, UINT message, WPARAM w_param, LPARAM l_param);
	public:
		Engine();

		void Process(ECS::Registry& registry, Event::Create&&) override;

		void Process(ECS::Registry& registry, Event::Resize&&) override;

		void Process(ECS::Registry& registry, Event::Process&&) override;
		void Process(ECS::Registry& registry, Event::Frame::Update&&) override;
		void Process(ECS::Registry& registry, Event::Frame::Begin&&) override;
		void Process(ECS::Registry& registry, Event::Frame::Render&&) override;
		void Process(ECS::Registry& registry, Event::Frame::End&&) override;

		void Process(ECS::Registry& registry, Event::Terminate&&) override;

		template <typename... Arguments>
		void RegisterPass(std::size_t index, Arguments&&... arguments)
		{
			assert(index < pass_.size());

			auto registry = ECS::Registry::GetInstance();
			registry->Emplace<Component::PassConcept>(pass_[index], std::forward<Arguments>(arguments)...);
		}

		std::shared_ptr<madX::Engine::Renderer>& GetRenderer() noexcept
		{
			return renderer_;
		}
	};
}