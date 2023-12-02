#include "_root.hpp"

#include "engine/texture.hpp"

namespace madX::Root::Component
{
	void GBuffer::CreateSizeDependet(const std::shared_ptr<madX::Engine::Renderer>& renderer, const Event::Resize resize)
	{	
		madX::Engine::TextureDesc depth_desc;
		depth_desc.name_ = "GBufferDepth";
		depth_desc.width_ = resize.width_;
		depth_desc.height_ = (std::uint16_t)resize.height_;
		depth_desc.dimension_ = madX::Engine::ResourceDimension::Texture2D;
		depth_desc.format_ = DXGI_FORMAT_D32_FLOAT;
		depth_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		depth_desc.bind_ = madX::Engine::ResourceBind::DepthStencil;
		//color_desc.clear_value_;
		
		depth_ = renderer->CreateTexture(depth_desc, nullptr);
		
		madX::Engine::TextureViewDesc depth_dsv_desc;
		depth_dsv_desc.type_ = madX::Engine::ResourceView::DepthStencil;
		depth_->CreateView(renderer.get(), depth_dsv_desc, depth_dsv_);

		madX::Engine::TextureViewDesc depth_srv_desc;
		depth_srv_desc.type_ = madX::Engine::ResourceView::ShaderResource;
		depth_srv_desc.format_ = DXGI_FORMAT_R32_FLOAT;
		depth_->CreateView(renderer.get(), depth_srv_desc, depth_srv_);

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::TextureDesc color_desc;
		color_desc.name_ = "GBufferColor";
		color_desc.width_ = resize.width_;
		color_desc.height_ = (std::uint16_t)resize.height_;
		color_desc.dimension_ = madX::Engine::ResourceDimension::Texture2D;
		color_desc.format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
		color_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		color_desc.bind_ = madX::Engine::ResourceBind::RenderTarget;

		color_ = renderer->CreateTexture(color_desc, nullptr);

		madX::Engine::TextureViewDesc color_rtv_desc;
		color_rtv_desc.type_ = madX::Engine::ResourceView::RenderTarget;
		color_->CreateView(renderer.get(), color_rtv_desc, color_rtv_);

		madX::Engine::TextureViewDesc color_srv_desc;
		color_srv_desc.type_ = madX::Engine::ResourceView::ShaderResource;
		color_->CreateView(renderer.get(), color_srv_desc, color_srv_);

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::TextureDesc distortion_desc;
		distortion_desc.name_ = "GBufferDistortion";
		distortion_desc.width_ = resize.width_;
		distortion_desc.height_ = (std::uint16_t)resize.height_;
		distortion_desc.dimension_ = madX::Engine::ResourceDimension::Texture2D;
		distortion_desc.format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
		distortion_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		distortion_desc.bind_ = madX::Engine::ResourceBind::RenderTarget;

		distortion_ = renderer->CreateTexture(distortion_desc, nullptr);

		madX::Engine::TextureViewDesc distortion_rtv_desc;
		distortion_rtv_desc.type_ = madX::Engine::ResourceView::RenderTarget;
		distortion_->CreateView(renderer.get(), distortion_rtv_desc, distortion_rtv_);

		madX::Engine::TextureViewDesc distortion_srv_desc;
		distortion_srv_desc.type_ = madX::Engine::ResourceView::ShaderResource;
		distortion_->CreateView(renderer.get(), distortion_srv_desc, distortion_srv_);

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::TextureDesc normal_desc;
		normal_desc.name_ = "GBufferNormal";
		normal_desc.width_ = resize.width_;
		normal_desc.height_ = (std::uint16_t)resize.height_;
		normal_desc.dimension_ = madX::Engine::ResourceDimension::Texture2D;
		normal_desc.format_ = DXGI_FORMAT_R10G10B10A2_UNORM;
		normal_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		normal_desc.bind_ = madX::Engine::ResourceBind::RenderTarget;

		normal_ = renderer->CreateTexture(normal_desc, nullptr);

		madX::Engine::TextureViewDesc normal_rtv_desc;
		normal_rtv_desc.type_ = madX::Engine::ResourceView::RenderTarget;
		normal_->CreateView(renderer.get(), normal_rtv_desc, normal_rtv_);

		madX::Engine::TextureViewDesc normal_srv_desc;
		normal_srv_desc.type_ = madX::Engine::ResourceView::ShaderResource;
		normal_->CreateView(renderer.get(), normal_srv_desc, normal_srv_);

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::TextureDesc occlusion_desc;
		occlusion_desc.name_ = "GBufferOcclusion";
		occlusion_desc.width_ = resize.width_;
		occlusion_desc.height_ = (std::uint16_t)resize.height_;
		occlusion_desc.dimension_ = madX::Engine::ResourceDimension::Texture2D;
		occlusion_desc.format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
		occlusion_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		occlusion_desc.bind_ = madX::Engine::ResourceBind::RenderTarget;

		occlusion_ = renderer->CreateTexture(occlusion_desc, nullptr);

		madX::Engine::TextureViewDesc occlusion_rtv_desc;
		occlusion_rtv_desc.type_ = madX::Engine::ResourceView::RenderTarget;
		occlusion_->CreateView(renderer.get(), occlusion_rtv_desc, occlusion_rtv_);

		madX::Engine::TextureViewDesc occlusion_srv_desc;
		occlusion_srv_desc.type_ = madX::Engine::ResourceView::ShaderResource;
		occlusion_->CreateView(renderer.get(), occlusion_srv_desc, occlusion_srv_);

		////////////////////////////////////////////////////////////////////////////////

		return;
	}

	void GBuffer::Create(const std::shared_ptr<madX::Engine::Renderer>& renderer, std::shared_ptr<Root::Component::Project>& project)
	{
		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::PipelineDesc pipeline_desc;

		pipeline_desc.name_ = "GBuffer";

		pipeline_desc.RT_count_ = 4;
		pipeline_desc.RTV_format[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipeline_desc.RTV_format[1] = DXGI_FORMAT_R10G10B10A2_UNORM;
		pipeline_desc.RTV_format[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipeline_desc.RTV_format[3] = DXGI_FORMAT_R8G8B8A8_UNORM;

		pipeline_desc.depth_stencil_state_.DepthEnable = TRUE;
		pipeline_desc.depth_stencil_state_.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;//D3D12_COMPARISON_FUNC_LESS;
		pipeline_desc.DSV_format = DXGI_FORMAT_D32_FLOAT;

		pipeline_desc.rasterizer_state_.CullMode = D3D12_CULL_MODE_BACK;

		{
			madX::Engine::ShaderDesc shader_desc;

			shader_desc.name_ = "GBufferVS";
			shader_desc.type_ = madX::Engine::ShaderType::Vertex;
			shader_desc.file_ = "shaders\\gbuffer.vsh";
			shader_desc.entry_ = L"main";
			//TODO: template for automatic creation based on <shader_desc>
			pipeline_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);

			shader_desc.name_ = "GBufferPS";
			shader_desc.type_ = madX::Engine::ShaderType::Pixel;
			shader_desc.file_ = "shaders\\gbuffer.psh";
			shader_desc.entry_ = L"main";
			//TODO: template for automatic creation based on <shader_desc>
			pipeline_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);
		}

		////////////////////////////////////////////////////////////////////////////////

		pipeline_desc.primitive_topology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		pipeline_desc.input_layout_.push_back(
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

		pipeline_desc.input_layout_.push_back(
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

		pipeline_desc.input_layout_.push_back(
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

		pipeline_desc.input_layout_.push_back(
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	
		pipeline_ = renderer->CreatePipeline(pipeline_desc);

		////////////////////////////////////////////////////////////////////////////////

		context_ = std::make_unique<madX::Engine::Context>(renderer.get());
		return;
	}

	void GBuffer::Render()
	{
		std::vector<D3D12_VIEWPORT> viewports;
		viewports.emplace_back(CD3DX12_VIEWPORT(0.0f, 0.0f, (float)width_, (float)height_));
		context_->SetViewports(viewports);

		auto scisors = CD3DX12_RECT(0, 0, (LONG)width_, (LONG)height_);
		context_->SetScissorRects(1, &scisors);

		context_->SetPipeline(pipeline_);
		context_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto cmd_list = context_->GetCommandList();

		const auto dsv_handle = depth_dsv_.GetHandleCPU();
		const auto color_rtv_handle = color_rtv_.GetHandleCPU();
		const auto normal_rtv_handle = normal_rtv_.GetHandleCPU();
		const auto distortion_rtv_handle = distortion_rtv_.GetHandleCPU();
		const auto occlusion_rtv_handle = occlusion_rtv_.GetHandleCPU();

		D3D12_CPU_DESCRIPTOR_HANDLE rtv_handles[4];
		rtv_handles[0] = color_rtv_handle;
		rtv_handles[1] = normal_rtv_handle;
		rtv_handles[2] = distortion_rtv_handle;
		rtv_handles[3] = occlusion_rtv_handle;

		cmd_list->OMSetRenderTargets(4, rtv_handles, false, &dsv_handle); //TODO: continous handle

		{	
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				depth_->GetResource(),
				depth_->GetResourceState(),
				D3D12_RESOURCE_STATE_DEPTH_WRITE
			);
			cmd_list->ResourceBarrier(1, &bb_barier);
			depth_->SetResourceState(D3D12_RESOURCE_STATE_DEPTH_WRITE);
		
			//cmd_list->ClearDepthStencilView(dsv_handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
			cmd_list->ClearDepthStencilView(dsv_handle, D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0, 0, nullptr);
		}

		{	
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				color_->GetResource(),
				color_->GetResourceState(),
				D3D12_RESOURCE_STATE_RENDER_TARGET
			);
			cmd_list->ResourceBarrier(1, &bb_barier);
			color_->SetResourceState(D3D12_RESOURCE_STATE_RENDER_TARGET);	
		
			float clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			cmd_list->ClearRenderTargetView(color_rtv_handle, clear_color, 0, NULL);
		}

		{	
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				normal_->GetResource(),
				normal_->GetResourceState(),
				D3D12_RESOURCE_STATE_RENDER_TARGET
			);
			cmd_list->ResourceBarrier(1, &bb_barier);
			normal_->SetResourceState(D3D12_RESOURCE_STATE_RENDER_TARGET);	
			
			float clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			cmd_list->ClearRenderTargetView(normal_rtv_handle, clear_color, 0, NULL);
		}

		{	
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				distortion_->GetResource(),
				distortion_->GetResourceState(),
				D3D12_RESOURCE_STATE_RENDER_TARGET
			);
			cmd_list->ResourceBarrier(1, &bb_barier);
			distortion_->SetResourceState(D3D12_RESOURCE_STATE_RENDER_TARGET);	
			
			float clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			cmd_list->ClearRenderTargetView(distortion_rtv_handle, clear_color, 0, NULL);
		}

		{	
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				occlusion_->GetResource(),
				occlusion_->GetResourceState(),
				D3D12_RESOURCE_STATE_RENDER_TARGET
			);
			cmd_list->ResourceBarrier(1, &bb_barier);
			occlusion_->SetResourceState(D3D12_RESOURCE_STATE_RENDER_TARGET);	
			
			float clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			cmd_list->ClearRenderTargetView(occlusion_rtv_handle, clear_color, 0, NULL);
		}
	}

	void GBuffer::End()
	{
		auto cmd_list = context_->GetCommandList();
		
		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				depth_->GetResource(),
				depth_->GetResourceState(),
				D3D12_RESOURCE_STATE_COMMON 
			);
		
			cmd_list->ResourceBarrier(1, &bb_barier);
			depth_->SetResourceState(D3D12_RESOURCE_STATE_COMMON);
		}

		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				color_->GetResource(),
				color_->GetResourceState(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);
		
			cmd_list->ResourceBarrier(1, &bb_barier);
			color_->SetResourceState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
		
		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				normal_->GetResource(),
				normal_->GetResourceState(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);
		
			cmd_list->ResourceBarrier(1, &bb_barier);
			normal_->SetResourceState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}

		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				distortion_->GetResource(),
				distortion_->GetResourceState(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);
		
			cmd_list->ResourceBarrier(1, &bb_barier);
			distortion_->SetResourceState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}

		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				occlusion_->GetResource(),
				occlusion_->GetResourceState(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);
		
			cmd_list->ResourceBarrier(1, &bb_barier);
			occlusion_->SetResourceState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
	}
}