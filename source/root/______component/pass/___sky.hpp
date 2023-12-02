#pragma once

namespace madX::Root::Component
{
	class Sky : public Pass
	{
	public:
		struct Constants
		{
			XMMATRIX invViewProj;
			XMFLOAT4 vSunDirection;
			float rayleigh;
			float turbidity;
			float mieCoefficient;
			float luminance;
			float mieDirectionalG;
		};
	private:	
		Constants constants_;

		std::uint32_t width_ = 0;
		std::uint32_t height_ = 0;
	public:
		void Create(const std::shared_ptr<madX::Engine::Renderer>&, std::shared_ptr<Root::Component::Project>&) override;

		void Destroy() override;
		
		void Resize(const std::shared_ptr<madX::Engine::Renderer>&, const Event::Resize resize) override
		{
			if (width_ != resize.width_ || height_ != resize.height_)
			{
				width_ = resize.width_;
				height_ = resize.height_;
			}
		}
		
		void Update(const Event::Frame::Update update) override
		{
		}
		
		void Begin(ID3D12GraphicsCommandList* command_list) override
		{
			context_->SetCommandList(command_list);
		};
		
		void Render() override;
		
		void End() override
		{
		}

		////////////////////////////////////////////////////////////////////////////////

		void SetInverseMVP(const float* i_mvp) noexcept
		{
			constants_.invViewProj = XMMATRIX(i_mvp);
		}
	};

//	class Deferred : public Pass
//	{
//		std::shared_ptr<madX::Engine::Pipeline> pipeline_;
//		std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout_;
//	
//		struct Output
//		{	
//			std::shared_ptr<madX::Engine::Texture> rt_;
//			std::shared_ptr<madX::Engine::Texture> ds_;
//	
//			madX::Engine::TextureView rtv_;
//			madX::Engine::TextureView srv_;
//			madX::Engine::TextureView dsv_;
//	
//			std::uint32_t width_ = 0;
//			std::uint32_t height_ = 0;
//		};
//	
//		Output output_;
//	
//		void CreateSizeDependet(const std::shared_ptr<madX::Engine::Renderer>&, const Event::Resize);
//	public:
//		Deferred() = default;
//	
//		void Create(const std::shared_ptr<madX::Engine::Renderer>&);
//		void Destroy() override;
//	
//		void Resize(const std::shared_ptr<madX::Engine::Renderer>&, const Event::Resize) override;
//	
//		void Update(const Event::Frame::Update update) override
//		{
//			return;
//		}
//	
//		void Begin(ID3D12GraphicsCommandList* command_list) override
//		{
//			context_->SetCommandList(command_list);
//		};
//	
//		void Render() override;
//	
//		void End() override;
//	
//		auto& GetOutput() noexcept
//		{
//			return output_;
//		}
//	};
}