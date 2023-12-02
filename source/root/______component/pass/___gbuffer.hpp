#pragma once

namespace madX::Root::Component
{
	class GBuffer : public Pass
	{
	public:
		std::shared_ptr<madX::Engine::Texture> depth_;
		std::shared_ptr<madX::Engine::Texture> color_;
		std::shared_ptr<madX::Engine::Texture> normal_;
		std::shared_ptr<madX::Engine::Texture> distortion_;
		std::shared_ptr<madX::Engine::Texture> occlusion_;

		madX::Engine::TextureView depth_dsv_;
		madX::Engine::TextureView depth_srv_;
		madX::Engine::TextureView color_rtv_;
		madX::Engine::TextureView color_srv_;
		madX::Engine::TextureView normal_rtv_;
		madX::Engine::TextureView normal_srv_;
		madX::Engine::TextureView distortion_rtv_;
		madX::Engine::TextureView distortion_srv_;
		madX::Engine::TextureView occlusion_rtv_;
		madX::Engine::TextureView occlusion_srv_;

		void CreateSizeDependet(const std::shared_ptr<madX::Engine::Renderer>&, const Event::Resize);
	public:
		GBuffer() : Pass(true) {}

		void Create(const std::shared_ptr<madX::Engine::Renderer>&, std::shared_ptr<Root::Component::Project>&) override;

		void Destroy() override
		{
			Pass::Destroy();

			depth_.reset();
			color_.reset();
			normal_.reset();
			distortion_.reset();
			occlusion_.reset();
		}
	
		void Resize(const std::shared_ptr<madX::Engine::Renderer>& renderer, const Event::Resize resize) override
		{
			Pass::Resize(renderer, resize);
			CreateSizeDependet(renderer, resize);
		}

		void Render() override;

		void End() override;

		auto GetDepthSRV() const noexcept
		{
			return depth_srv_.GetHandleGPU();
		}

		auto GetColorSRV() const noexcept
		{
			return color_srv_.GetHandleGPU();
		}

		auto GetNormalSRV() const noexcept
		{
			return normal_srv_.GetHandleGPU();
		}

		auto GetDistortionSRV() const noexcept
		{
			return distortion_srv_.GetHandleGPU();
		}
	};
}