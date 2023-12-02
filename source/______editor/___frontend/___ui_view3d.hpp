#pragma once

//#include "_editor.hpp"
#include "engine/context.hpp"
#include "camera.hpp"

namespace madX::Editor::Frontend
{
	class View3D final
	{
		struct Constants
		{
			Im3d::Mat4 view_projection_;
			Im3d::Vec2 viewport_;
		};
		Constants constants_;

		std::uint32_t width_ = 0;
		std::uint32_t height_ = 0;


		std::shared_ptr<madX::Engine::Pipeline> pipeline_points_;
		std::shared_ptr<madX::Engine::Pipeline> pipeline_lines_;
		std::shared_ptr<madX::Engine::Pipeline> pipeline_triangles_;

		std::unique_ptr<madX::Engine::Context> context_;
		std::shared_ptr<madX::Engine::Renderer> renderer_;

		Camera camera_;

		void CreateResources_Points(const madX::Engine::PipelineDesc&);
		void CreateResources_Lines(const madX::Engine::PipelineDesc&);
		void CreateResources_Triangles(const madX::Engine::PipelineDesc&);
	public:
		Im3d::Vec2 prevCursorPos_;

		explicit View3D(const std::shared_ptr<madX::Engine::Renderer>&);

		void CreateResources();
		void NewFrame(ID3D12GraphicsCommandList*);
		//void EndFrame(const XMMATRIX& view_projection);
		void EndFrame();

		void Resize(std::uint32_t width, std::uint32_t height)
		{
			if (width_ != width || height_ != height)
			{
				width_ = width;
				height_ = height;
			}

			const float aspect_ratio = (float)width / (float)height;
			camera_.Update(aspect_ratio);
		}

		auto& GetCamera() noexcept
		{
			return camera_;
		}
	};
}