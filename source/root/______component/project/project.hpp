#pragma once

namespace madX::Root::Component
{
	class Project final
	{
		std::map<Meta::glTF::Index, std::shared_ptr<madX::Engine::Buffer>> vb_;
		std::map<Meta::glTF::Index, std::shared_ptr<madX::Engine::Buffer>> ib_;

		struct Texture
		{
			std::shared_ptr<madX::Engine::Texture> tex_;
			madX::Engine::TextureView srv_;
			//Engine::NativeViewDesc null_desc_;
		};

		std::map<Meta::glTF::Index, Texture> texture_;

		//XMMATRIX mvp_;
		//XMMATRIX mv_;
		//Camera camera_;

		using Vertex = XMFLOAT3;
		using Vindex = std::uint32_t;

		std::unique_ptr<Meta::glTF::Source> source_;

		Texture hdr_;

		std::shared_ptr<madX::Engine::Texture> CreateTexture(const std::shared_ptr<madX::Engine::Renderer>&, Meta::glTF::Index);

		void BuildBuffers(const std::shared_ptr<madX::Engine::Renderer>&);
		void BuildTextures(const std::shared_ptr<madX::Engine::Renderer>&);
		//void BuildTexturesOrder();
	public:
		explicit Project(const std::shared_ptr<madX::Engine::Renderer>&);

		void Destroy();

		void Update(const Event::Frame::Update);
		void Draw(std::unique_ptr<madX::Engine::Context>& context, const float* world, const float* view_projection);

		const Meta::glTF::Source* GetSource() const noexcept
		{
			return source_.get();
		}

		const madX::Engine::TextureView& GetHDR() const noexcept
		{
			return hdr_.srv_;
		}
	//	madX::Engine::TextureView& GetHardCoded()
	//	{
	//		return texture_.at(0).srv_;
	//	}
	};
}