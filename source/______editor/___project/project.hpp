#pragma once

namespace madX::Root::Component
{
	class Project final
	{
		std::shared_ptr<madX::Engine::Buffer> vb_;
		std::shared_ptr<madX::Engine::Buffer> ib_;

		std::uint32_t indices_count_ = 0;

		XMMATRIX mvp_;
		Camera camera_;

		using Vertex = XMFLOAT3;
		using Vindex = std::uint32_t;

		std::unique_ptr<Meta::glTF::Source> source_;

		void CreateBuffers(const std::shared_ptr<madX::Engine::Renderer>&);
	public:
		explicit Project(const std::shared_ptr<madX::Engine::Renderer>&);

		//	Project(const Project&) = delete;
		//	Project& operator=(const Project&) = delete;
		//	
		//	Project(Project&&) = default;
		//	Project& operator=(Project&&) = default;

		void Destroy();

		void Update(const Event::Frame::Update);
		void Draw(std::unique_ptr<madX::Engine::Context>& context);
	};
}