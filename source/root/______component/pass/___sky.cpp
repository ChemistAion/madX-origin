#include "_root.hpp"

namespace madX::Root::Component
{
	void Sky::Create(const std::shared_ptr<madX::Engine::Renderer>& renderer, std::shared_ptr<Root::Component::Project>&)
	{
		madX::Engine::PipelineDesc pipeline_desc;
	
		pipeline_desc.name_ = "Sky";
	
		pipeline_desc.RT_count_ = 1;
		pipeline_desc.RTV_format[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		pipeline_desc.DSV_format = DXGI_FORMAT_D32_FLOAT;
		pipeline_desc.depth_stencil_state_.DepthEnable = TRUE;
		pipeline_desc.depth_stencil_state_.DepthFunc = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_EQUAL;
		//pipeline_desc.depth_stencil_state_.DepthFunc = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS;
	
		pipeline_desc.rasterizer_state_.CullMode = D3D12_CULL_MODE_NONE;

		{
			madX::Engine::ShaderDesc shader_desc;
	
			shader_desc.name_ = "FullScreenVS";
			shader_desc.type_ = madX::Engine::ShaderType::Vertex;
			shader_desc.file_ = "shaders\\fullscreen.vsh";
			shader_desc.entry_ = L"main";
			//TODO: template for automatic creation based on <shader_desc>
			pipeline_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);
	
			shader_desc.name_ = "SkyPS";
			shader_desc.type_ = madX::Engine::ShaderType::Pixel;
			shader_desc.file_ = "shaders\\sky.psh";
			shader_desc.entry_ = L"main";
			//TODO: template for automatic creation based on <shader_desc>
			pipeline_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);
		}
	
		pipeline_desc.input_layout_;
		pipeline_desc.primitive_topology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		////////////////////////////////////////////////////////////////////////////////
	
		pipeline_ = renderer->CreatePipeline(pipeline_desc);
		context_ = std::make_unique<madX::Engine::Context>(renderer.get());

		////////////////////////////////////////////////////////////////////////////////

		constants_.invViewProj; // = math::inverse(pPerFrame->mCameraCurrViewProj);
		constants_.vSunDirection = XMFLOAT4(1.0f, 0.05f, 0.0f, 0.0f);
		constants_.turbidity = 10.0f;
		constants_.rayleigh = 2.0f;
		constants_.mieCoefficient = 0.005f;
		constants_.mieDirectionalG = 0.8f;
		constants_.luminance = 1.0f;
	}

	void Sky::Destroy()
	{
		pipeline_.reset();
		context_.reset();
	}

	void Sky::Render()
	{
		auto renderer = context_->GetRenderer();

		std::vector<D3D12_VIEWPORT> viewports;
		viewports.emplace_back(CD3DX12_VIEWPORT(0.0f, 0.0f, (float)width_, (float)height_));
		context_->SetViewports(viewports);
		
		auto scisors = CD3DX12_RECT(0, 0, (LONG)width_, (LONG)height_);
		context_->SetScissorRects(1, &scisors);

		context_->SetPipeline(pipeline_);
		context_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		madX::Engine::BufferDesc buffer_desc;
		buffer_desc.name_ = "cbPerFrame";
		buffer_desc.count_ = 1;
		buffer_desc.stride_ = sizeof(Constants);
		buffer_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		buffer_desc.bind_ = madX::Engine::ResourceBind::ShaderResource;

		auto vs_constants = renderer->CreateBuffer(buffer_desc);

		Constants* cb_constants = (Constants*)vs_constants->GetAddressCPU();
		std::memcpy(cb_constants, &constants_, sizeof(Constants));

		assert(false);

		context_->CommitShaderResources();
		context_->CommitConstantBuffers(vs_constants);

		madX::Engine::DrawAttributes draw;
		draw.vertices_ = 3;
		context_->Draw(draw);	
	}
}