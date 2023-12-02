#include "_root.hpp"

namespace madX::Root::Component
{
	void SparseVoxelOctree::Create(const std::shared_ptr<madX::Engine::Renderer>& renderer, std::shared_ptr<Root::Component::Project>&)
	{
		UINT voxel_count = volume_size_ * volume_size_ * volume_size_;
		UINT mips = static_cast<UINT>(std::log2(volume_size_));

		madX::Engine::TextureDesc v_desc;
		v_desc.width_ = volume_size_;
		v_desc.height_ = volume_size_;
		v_desc.depth_ = volume_size_;
		v_desc.dimension_ = madX::Engine::ResourceDimension::Texture3D;
		v_desc.format_ = DXGI_FORMAT_R16G16B16A16_FLOAT;
		v_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
		v_desc.bind_ = madX::Engine::ResourceBind::RenderTargetUnorderedAccess;
		v_desc.mip_levels_ = mips;

		v_desc.name_ = "SVO_Normal";
		v_normal_ = renderer->CreateTexture(v_desc, nullptr);

		v_desc.name_ = "SVO_RHO";
		v_rho_ = renderer->CreateTexture(v_desc, nullptr);

		madX::Engine::TextureViewDesc v_srv_desc;
		v_srv_desc.type_ = madX::Engine::ResourceView::ShaderResource;
		v_srv_desc.dimension_ = madX::Engine::ResourceDimension::Texture3D;
		v_srv_desc.mip_levels_ = mips;
		
		v_normal_->CreateView(renderer.get(), v_srv_desc, v_normal_srv_);
		v_rho_->CreateView(renderer.get(), v_srv_desc, v_rho_srv_);

		madX::Engine::TextureViewDesc v_uav_desc;
		v_uav_desc.type_ = madX::Engine::ResourceView::UnorderedAccessNSV;
		v_uav_desc.dimension_ = madX::Engine::ResourceDimension::Texture3D;
		v_uav_desc.mip_levels_ = mips;

		v_normal_->CreateView(renderer.get(), v_uav_desc, v_normal_uav_);
		v_rho_->CreateView(renderer.get(), v_uav_desc, v_rho_uav_);

		madX::Engine::TextureViewDesc v_rtv_desc;
		v_rtv_desc.type_ = madX::Engine::ResourceView::RenderTarget;
		v_rtv_desc.dimension_ = madX::Engine::ResourceDimension::Texture3D;
		v_rtv_desc.mip_levels_ = mips;

		v_normal_->CreateView(renderer.get(), v_rtv_desc, v_normal_rtv_);
		v_rho_->CreateView(renderer.get(), v_rtv_desc, v_rho_rtv_);

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::PipelineDesc voxelize_desc;
		voxelize_desc.name_ = "SVO_voxelize";

		voxelize_desc.RT_count_ = 1;
		voxelize_desc.RTV_format[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;

		voxelize_desc.DSV_format = DXGI_FORMAT_UNKNOWN;
		voxelize_desc.depth_stencil_state_.DepthEnable = FALSE;
		voxelize_desc.rasterizer_state_.CullMode = D3D12_CULL_MODE_NONE;

		{
			madX::Engine::ShaderDesc shader_desc;
		
			shader_desc.name_ = "SVO::VoxelizeVS";
			shader_desc.type_ = madX::Engine::ShaderType::Vertex;
			shader_desc.file_ = "shaders\\svo_voxelize.hlsl";
			shader_desc.entry_ = L"vs_svo_voxelize";
			voxelize_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);

			shader_desc.name_ = "SVO::VoxelizeGS";
			shader_desc.type_ = madX::Engine::ShaderType::Geometry;
			shader_desc.file_ = "shaders\\svo_voxelize.hlsl";
			shader_desc.entry_ = L"gs_svo_voxelize";
			voxelize_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);
		
			shader_desc.name_ = "SVO::VoxelizePS";
			shader_desc.type_ = madX::Engine::ShaderType::Pixel;
			shader_desc.file_ = "shaders\\svo_voxelize.hlsl";
			shader_desc.entry_ = L"ps_svo_voxelize";
			voxelize_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);
		}

		voxelize_desc.primitive_topology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		voxelize_desc.input_layout_.push_back(
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

		voxelize_desc.input_layout_.push_back(
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

		voxelize_desc.input_layout_.push_back(
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

		pipeline_voxelize_ = renderer->CreatePipeline(voxelize_desc);

		////////////////////////////////////////////////////////////////////////////////

		madX::Engine::PipelineDesc inject_desc;
		inject_desc.name_ = "SVO_inject";

		inject_desc.DSV_format = DXGI_FORMAT_UNKNOWN;
		inject_desc.depth_stencil_state_.DepthEnable = FALSE;
		inject_desc.rasterizer_state_.CullMode = D3D12_CULL_MODE_NONE;

		{
			madX::Engine::ShaderDesc shader_desc;
		
			shader_desc.name_ = "SVO::InjectVS";
			shader_desc.type_ = madX::Engine::ShaderType::Vertex;
			shader_desc.file_ = "shaders\\svo_inject.hlsl";
			shader_desc.entry_ = L"vs_svo_inject";
			inject_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);
		
			shader_desc.name_ = "SVO::InjectPS";
			shader_desc.type_ = madX::Engine::ShaderType::Pixel;
			shader_desc.file_ = "shaders\\svo_inject.hlsl";
			shader_desc.entry_ = L"ps_svo_inject";
			inject_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);
		}

		inject_desc.primitive_topology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	
		pipeline_inject_ = renderer->CreatePipeline(inject_desc);

		////////////////////////////////////////////////////////////////////////////////

		DirectX::XMStoreFloat4x4(&world_, DirectX::XMMatrixIdentity());

		context_ = std::make_unique<madX::Engine::Context>(renderer.get());
	}

	//https://www.gamedeveloper.com/programming/efficient-d3d12-descriptor-heap-management-system

	void SparseVoxelOctree::Render()
	{
		std::vector<D3D12_VIEWPORT> viewports;
		viewports.emplace_back(CD3DX12_VIEWPORT(0.0f, 0.0f, (float)volume_size_, (float)volume_size_));
		context_->SetViewports(viewports);
		
		auto scisors = CD3DX12_RECT(0, 0, (LONG)volume_size_, (LONG)volume_size_);
		context_->SetScissorRects(1, &scisors);
		
		context_->SetPipeline(pipeline_voxelize_);
		context_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		context_->CommitShaderResources();

		auto cmd_list = context_->GetCommandList();

		auto& element = v_normal_uav_.GetElement();
		ID3D12DescriptorHeap* heaps[1] = { element.GetDescriptorHeap() };
		cmd_list->SetDescriptorHeaps(1, heaps);

		D3D12_CPU_DESCRIPTOR_HANDLE rtv_handles[1];
		rtv_handles[0] = v_normal_rtv_.GetHandleCPU();
		cmd_list->OMSetRenderTargets(1, rtv_handles, false, nullptr);

		auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
		(
			v_normal_->GetResource(),
			v_normal_->GetResourceState(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);
		cmd_list->ResourceBarrier(1, &bb_barier);
		v_normal_->SetResourceState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		
	//	auto test_cpu = v_normal_uav_.GetHandleCPU();
	//	auto test_gpu = v_normal_uav_.GetHandleGPU();

		const auto handle = v_normal_srv_.GetHandleGPU();
		cmd_list->SetGraphicsRootDescriptorTable(3, handle);

		const float clear[4] = { 0.f, 0.f, 0.f, 0.f };
		cmd_list->ClearUnorderedAccessViewFloat
		(
			v_normal_uav_.GetHandleGPU(),
			v_normal_uav_.GetHandleCPU(),
			v_normal_->GetResource(),
			clear,
			0, NULL
		);
		
	}
	
	void SparseVoxelOctree::Update(const Camera& camera)
	{
	//	madX::Engine::BufferDesc buffer_desc;
	//	buffer_desc.name_ = "Constants";
	//	buffer_desc.count_ = 1;
	//	buffer_desc.stride_ = sizeof(Constants);
	//	buffer_desc.usage_ = madX::Engine::ResourceUsage::Dynamic;
	//	buffer_desc.bind_ = madX::Engine::ResourceBind::UniformBuffer;
	//	
	//	auto constants = context_->GetRenderer()->CreateBuffer(buffer_desc);
	//	
	//	const auto cam_projection = camera.GetProjection();
	//	constants_.perspective_.x = 1.0f / cam_projection.m[0][0];
	//	constants_.perspective_.y = 1.0f / cam_projection.m[1][1];
	//	constants_.perspective_.z = cam_projection.m[3][2];
	//	constants_.perspective_.w = -cam_projection.m[2][2];
	//	
	//	constants_.direction_ = camera.GetDirection();
	//	constants_.inv_vp_ = camera.GetInverseViewProjection();
	//	
	//	Constants* cb_constants = (Constants*)constants->GetAddressCPU();
	//	std::memcpy(cb_constants, &constants_, sizeof(Constants));
	//	
	//	////////////////////////////////////////////////////////////////////////////////
	//	
	//	context_->SetPipeline(pipeline_);
	//	context_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//	
	//	context_->CommitShaderResources();
	//	context_->CommitConstantBuffers(constants);
	}
}