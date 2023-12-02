#pragma once

namespace madX::Root::Component
{
	using PassBase = std::unique_ptr<class Pass>;

	class Pass
	{
	protected:
		std::shared_ptr<madX::Engine::Pipeline> pipeline_;
		std::unique_ptr<madX::Engine::Context> context_;
		const bool drawable_ = false;

		std::uint32_t width_ = 0;
		std::uint32_t height_ = 0;
	public:
		Pass(bool drawable = false)
			: drawable_{ drawable }
		{
		}

		virtual void Create(const std::shared_ptr<madX::Engine::Renderer>&, std::shared_ptr<Root::Component::Project>&) = 0;
		virtual void Destroy()
		{
			pipeline_.reset();
			context_.reset();
		}

		virtual void Resize(const std::shared_ptr<madX::Engine::Renderer>&, const Event::Resize resize)
		{
			if (width_ != resize.width_ || height_ != resize.height_)
			{
				width_ = resize.width_;
				height_ = resize.height_;
			}
		}

		virtual void Update(const Camera& camera) {}

		virtual void Begin(ID3D12GraphicsCommandList* command_list)
		{
			context_->SetCommandList(command_list);
		}

		virtual void Render() = 0;
		virtual void End() {}

		auto& GetContext() noexcept
		{
			return context_;
		}

		bool IsDrawable() const noexcept
		{
			return drawable_;
		}

		std::uint32_t GetWidth() const noexcept
		{
			return width_;
		}

		std::uint32_t GetHeight() const noexcept
		{
			return height_;
		}
	};
}