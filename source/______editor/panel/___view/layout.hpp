#pragma once

#include "_editor.hpp"

#include "engine/context.hpp"
#include "panel/debug/debug.hpp"

namespace Im3d
{
	void RandSeed(int _seed);
	int RandInt(int _min, int _max);
	float RandFloat(float _min, float _max);
	Vec3 RandVec3(float _min, float _max);
	Mat3 RandRotation();
	Color RandColor(float _min, float _max);
}

namespace madX::Editor::Panel
{
	class Layout final : public PanelClass
	{
		struct Constants
		{
			//Im3d::Mat4 view_projection_;
			XMFLOAT4X4 view_projection_;
			Im3d::Vec2 viewport_;
		};
		Constants constants_;
		
		//TODO: ImGui based
		std::uint32_t width_ = 0;
		std::uint32_t height_ = 0;
			
		std::shared_ptr<madX::Engine::Pipeline> depth_points_;
		std::shared_ptr<madX::Engine::Pipeline> depth_lines_;
		std::shared_ptr<madX::Engine::Pipeline> depth_triangles_;

		std::shared_ptr<madX::Engine::Pipeline> raw_points_;
		std::shared_ptr<madX::Engine::Pipeline> raw_lines_;
		std::shared_ptr<madX::Engine::Pipeline> raw_triangles_;

		struct Output
		{
			std::shared_ptr<madX::Engine::Texture> rt_;
			std::shared_ptr<madX::Engine::Texture> ds_;

			madX::Engine::TextureView rtv_;
			madX::Engine::TextureView srv_;
			madX::Engine::TextureView dsv_;
		};

		Output output_;

		Root::Component::CameraInfinity camera_;
		//Im3d::Vec2 cursor_prev_;

		bool cursor_clip_ = false;

		////////////////////////////////////////////////////////////////////////////////

		std::shared_ptr<Panel::Debug> debug_;
		
		std::unique_ptr<madX::Engine::Context> context_;
		std::shared_ptr<madX::Engine::Renderer> renderer_;

		std::shared_ptr<Root::Component::FrameGraph> frame_graph_;

		std::shared_ptr<madX::Engine::Pipeline> CreateResources_Points(const madX::Engine::PipelineDesc&);
		std::shared_ptr<madX::Engine::Pipeline> CreateResources_Lines(const madX::Engine::PipelineDesc&);
		std::shared_ptr<madX::Engine::Pipeline> CreateResources_Triangles(const madX::Engine::PipelineDesc&);
		
		void CreateSizeDependet();
		void Update(bool resize);
		void End(const std::shared_ptr<Root::Component::Project>&);
	public:
		Layout(const std::shared_ptr<madX::Engine::Renderer>& renderer, const std::shared_ptr<Panel::Debug>& debug)
			: debug_(debug)
		{
			renderer_ = renderer;	
			context_ = std::make_unique<madX::Engine::Context>(renderer.get());

			frame_graph_ = std::make_shared<Root::Component::FrameGraph>();
			frame_graph_->pass_.emplace_back(std::make_unique<Root::Component::GBuffer>());
			frame_graph_->pass_.emplace_back(std::make_unique<Root::Component::Deferred>());
			//frame_graph_->pass_.emplace_back(std::make_unique<Root::Component::SparseVoxelOctree>());
		}

		virtual ~Layout()
		{
			for (auto& pass : frame_graph_->pass_)
			{
				pass->Destroy();
				//pass->Update();
			}
		}

		void CreatePanel(std::shared_ptr<Root::Component::Project>&) override;
		//void UpdatePanel() override;
		void BeginPanel() override;
		void RenderPanel(const std::shared_ptr<Root::Component::Project>&) override;
		//void EndPanel() override;

		////////////////////////////////////////////////////////////////////////////////

	//	const std::shared_ptr<Root::Component::FrameGraph>& GetFrameGraph() const
	//	{
	//		return frame_graph_;
	//	}
	};
}
