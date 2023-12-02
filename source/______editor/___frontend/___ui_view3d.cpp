#include "_editor.hpp"

#include "ui_view3d.hpp"
#include "im3d.h"
#include "imgui.h"

namespace madX::Editor::Frontend
{
	void View3D::CreateResources_Points(const madX::Engine::PipelineDesc& desc)
	{
		auto pipeline_desc = desc;

		pipeline_desc.name_ = "EditorView3D_Points";
		pipeline_desc.primitive_topology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::ShaderDesc shader_desc;
		
		shader_desc.name_ = "im3dVS_Points";
		shader_desc.type_ = madX::Engine::ShaderType::Vertex;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"VERTEX_SHADER");
		shader_desc.defines_.push_back(L"POINTS");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);
		
		shader_desc.name_ = "im3dGS_Points";
		shader_desc.type_ = madX::Engine::ShaderType::Geometry;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"GEOMETRY_SHADER");
		shader_desc.defines_.push_back(L"POINTS");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);
		
		shader_desc.name_ = "im3dPS_Points";
		shader_desc.type_ = madX::Engine::ShaderType::Pixel;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"PIXEL_SHADER");
		shader_desc.defines_.push_back(L"POINTS");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);
		
		////////////////////////////////////////////////////////////////////////////////

		pipeline_points_ = renderer_->CreatePipeline(pipeline_desc);
	}

	void View3D::CreateResources_Lines(const madX::Engine::PipelineDesc& desc)
	{
		auto pipeline_desc = desc;

		pipeline_desc.name_ = "EditorView3D_Lines";
		pipeline_desc.primitive_topology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::ShaderDesc shader_desc;

		shader_desc.name_ = "im3dVS_Lines";
		shader_desc.type_ = madX::Engine::ShaderType::Vertex;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"VERTEX_SHADER");
		shader_desc.defines_.push_back(L"LINES");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);

		shader_desc.name_ = "im3dGS_Lines";
		shader_desc.type_ = madX::Engine::ShaderType::Geometry;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"GEOMETRY_SHADER");
		shader_desc.defines_.push_back(L"LINES");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);

		shader_desc.name_ = "im3dPS_Lines";
		shader_desc.type_ = madX::Engine::ShaderType::Pixel;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"PIXEL_SHADER");
		shader_desc.defines_.push_back(L"LINES");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);

		////////////////////////////////////////////////////////////////////////////////

		pipeline_lines_ = renderer_->CreatePipeline(pipeline_desc);
	}

	void View3D::CreateResources_Triangles(const madX::Engine::PipelineDesc& desc)
	{
		auto pipeline_desc = desc;

		pipeline_desc.name_ = "EditorView3D_Triangles";
		pipeline_desc.primitive_topology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::ShaderDesc shader_desc;

		shader_desc.name_ = "im3dVS_Triangles";
		shader_desc.type_ = madX::Engine::ShaderType::Vertex;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"VERTEX_SHADER");
		shader_desc.defines_.push_back(L"TRIANGLES");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);

		shader_desc.name_ = "im3dPS_Triangles";
		shader_desc.type_ = madX::Engine::ShaderType::Pixel;
		shader_desc.file_ = "shaders\\im3d.hlsl";
		shader_desc.entry_ = L"main";
		shader_desc.defines_.clear();
		shader_desc.defines_.push_back(L"PIXEL_SHADER");
		shader_desc.defines_.push_back(L"TRIANGLES");
		pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);

		////////////////////////////////////////////////////////////////////////////////

		pipeline_triangles_ = renderer_->CreatePipeline(pipeline_desc);
	}

	////////////////////////////////////////////////////////////////////////////////

	View3D::View3D(const std::shared_ptr<madX::Engine::Renderer>& renderer)
	{
		renderer_ = renderer;

		context_ = std::make_unique<madX::Engine::Context>(renderer.get());
	}

	void View3D::CreateResources()
	{
		madX::Engine::PipelineDesc pipeline_desc;

		pipeline_desc.RT_count_ = 1;
		pipeline_desc.RTV_format[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		pipeline_desc.depth_stencil_state_.DepthEnable = TRUE;
		pipeline_desc.DSV_format = DXGI_FORMAT_D32_FLOAT;

		pipeline_desc.rasterizer_state_.CullMode = D3D12_CULL_MODE_NONE;

		////////////////////////////////////////////////////////////////////////////////

		auto& blend_state = pipeline_desc.blend_state_.RenderTarget[0];

		blend_state.BlendEnable = TRUE;
		blend_state.LogicOpEnable = FALSE;
		blend_state.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blend_state.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blend_state.BlendOp = D3D12_BLEND_OP_ADD;
		blend_state.SrcBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		blend_state.DestBlendAlpha = D3D12_BLEND_ZERO;
		blend_state.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blend_state.LogicOp = D3D12_LOGIC_OP_NOOP;
		blend_state.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		////////////////////////////////////////////////////////////////////////////////

		pipeline_desc.input_layout_.push_back(
		  { "POSITION_SIZE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		pipeline_desc.input_layout_.push_back(
		  { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

		CreateResources_Points(pipeline_desc);
		CreateResources_Lines(pipeline_desc);
		CreateResources_Triangles(pipeline_desc);
	}

	void View3D::NewFrame(ID3D12GraphicsCommandList* command_list)
	{
		Im3d::NewFrame();
		context_->SetCommandList(command_list);

		return;
	}

	//void View3D::EndFrame(const XMMATRIX& view_projection)
	void View3D::EndFrame()
	{
		Im3d::EndFrame();

		std::vector<D3D12_VIEWPORT> viewports;
		viewports.emplace_back(CD3DX12_VIEWPORT(0.0f, 0.0f, (float)width_, (float)height_));
		context_->SetViewports(viewports);
			
		auto scisors = CD3DX12_RECT(0, 0, (LONG)width_, (LONG)height_);
		context_->SetScissorRects(1, &scisors);

		const float blend_factors[4] = { 0.f, 0.f, 0.f, 0.f };
		context_->SetBlendFactors(blend_factors);

		madX::Engine::BufferDesc buffer_desc;
		buffer_desc.name_ = "cbContextData";
		buffer_desc.size_ = sizeof(Constants);
		buffer_desc.stride_ = sizeof(Constants);
		buffer_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		buffer_desc.bind_ = madX::Engine::ResourceBind::UniformBuffer;

		constants_.view_projection_ = camera_.GetViewProjection();
		constants_.viewport_.x = (float)width_;
		constants_.viewport_.y = (float)height_;

		auto constants = renderer_->CreateBuffer(buffer_desc);

		Constants* cb_constants = (Constants*)constants->GetAddressCPU();
		std::memcpy(cb_constants, &constants_, sizeof(Constants));

		for (Im3d::U32 index = 0, count = Im3d::GetDrawListCount(); index < count; ++index)
		{
			auto& drawList = Im3d::GetDrawLists()[index];
		
			if (drawList.m_layerId == Im3d::MakeId("NamedLayer"))
			{
				// The application may group primitives into layers, which can be used to change the draw state (e.g. enable depth testing, use a different shader)
			}
			
			madX::Engine::BufferDesc vb_desc;
			vb_desc.name_ = "im3D_VB::" + std::to_string(index);
			vb_desc.size_ = drawList.m_vertexCount * sizeof(Im3d::VertexData);
			vb_desc.stride_ = sizeof(Im3d::VertexData);
			vb_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
			vb_desc.bind_ = madX::Engine::ResourceBind::VertexBuffer;
			auto vb = renderer_->CreateBuffer(vb_desc);

			auto* vtx_dst = (Im3d::VertexData*)vb->GetAddressCPU();
			std::memcpy(vtx_dst, drawList.m_vertexData, drawList.m_vertexCount * sizeof(Im3d::VertexData));

			switch (drawList.m_primType)
			{
			case Im3d::DrawPrimitive_Points:
			{
				context_->SetPipeline(pipeline_points_);
				context_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
				break;
			}
			case Im3d::DrawPrimitive_Lines:
			{
				context_->SetPipeline(pipeline_lines_);
				context_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
				break;
			}
			case Im3d::DrawPrimitive_Triangles:
			{
				context_->SetPipeline(pipeline_triangles_);
				context_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				break;
			}
			default:
				assert(false);
				return;
			};

			auto test = std::make_pair<std::shared_ptr<madX::Engine::Buffer>, std::size_t>(std::move(vb), (std::size_t)0);
			std::vector<madX::Engine::VertexStream> test2 = { test };
			context_->SetVertexBuffer(0, test2);

			context_->CommitShaderResources();
			context_->CommitConstantBuffers(constants);

			madX::Engine::DrawAttributes draw;
			draw.vertices_ = drawList.m_vertexCount;
			context_->Draw(draw);	
		}	

		//g_Example->drawTextDrawListsImGui(Im3d::GetTextDrawLists(), Im3d::GetTextDrawListCount());
	}
}