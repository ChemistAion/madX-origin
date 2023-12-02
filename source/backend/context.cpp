#include "context.hpp"
#include "d3d12.h"

import madX.Backend.Processor;

namespace madX::Backend
{
	void Context::EndFrame()
	{
		FlushResourceBarriers();
		
		//TODO: on behalf on Processor 
		auto result = command_list_->Close();
		ThrowIfFailed(result, "[Context::Flush] ID3D12GraphicsCommandList::Close failed");

		ID3D12CommandList* const lists[] = { command_list_ };
		GetProcessor ()->GetGraphicsQueue()->ExecuteCommandLists(1, lists);
	}

	////////////////////////////////////////////////////////////////////////////////

//	void Context::CommitRenderTargets()
//	{
//		//TODO: static assert
//		assert(bound_rtv_.size() <= D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
//		//assert(!bound_rtv_.empty() || bound_dsv_); //TODO: no clearing???
//		
//		D3D12_CPU_DESCRIPTOR_HANDLE rtv[8];
//		//D3D12_CPU_DESCRIPTOR_HANDLE dsv = {};
//			
//		auto index_rtv = 0;
//		
//		assert(bound_rtv_.empty());
//		assert(bound_dsv_ == nullptr);
//	
//		//	for (auto& bound : bound_rtv_)
//		//	{					
//		//		if (auto texture = bound->GetTexture(); texture->CheckTransitionState(D3D12_RESOURCE_STATE_RENDER_TARGET))
//		//		{
//		//			TransitionResource(texture, D3D12_RESOURCE_STATE_RENDER_TARGET);
//		//		}
//		//		
//		//		rtv[index_rtv++] = bound->GetDescriptorHandle();
//		//	}
//	
//		//	if (bound_dsv_ != nullptr)
//		//	{			
//		//		if (auto texture = bound_dsv_->GetTexture(); texture->CheckTransitionState(D3D12_RESOURCE_STATE_DEPTH_WRITE))
//		//		{
//		//			TransitionResource(texture, D3D12_RESOURCE_STATE_DEPTH_WRITE);
//		//		}
//		//	
//		//		dsv = bound_dsv_->GetDescriptorHandle();
//		//		assert(dsv.ptr != 0);
//		//	}
//		
//		////////////////////////////////////////////////////////////////////////////////
//		
//		assert(command_list_ != nullptr);
//		
//		if (bound_rtv_.empty())
//		{
//			return;
//		}
//	
//		command_list_->OMSetRenderTargets
//		(
//			(UINT)bound_rtv_.size(),
//			rtv, //TODO: Using uninitialized memory 'rtv'
//			FALSE,
//			nullptr //bound_dsv_ ? &dsv : nullptr
//		);
//	}

	void Context::CommitViewports()
	{
		assert(command_list_ != nullptr);

		std::span<D3D12_VIEWPORT> viewports(viewports_);
		command_list_->RSSetViewports((UINT)viewports.size(), viewports.data());
	}

	void Context::CommitScisors()
	{	
		assert(command_list_ != nullptr);
		command_list_->RSSetScissorRects((UINT)scisors_.size(), scisors_.data());
	}

	//TODO: there is Buffer::method for that
	void Context::CommitVertexBuffers()
	{
		assert(pipeline_);			
		assert(vertex_stream_.size() <= cContext_MaxSlotsCount);
		//assert(false == vertex_stream_.empty());

		if (vertex_stream_.empty())
		{
			return;
		}

		D3D12_VERTEX_BUFFER_VIEW views[cContext_MaxSlotsCount];
		
		for (std::size_t index = 0; auto& stream : vertex_stream_)
		{
			//auto& view = views[index++] = stream.first->GetVertexBufferView();
			views[index++] = stream.first->GetVertexBufferView();
		}
			
		//FlushResourceBarriers();
		command_list_->IASetVertexBuffers(0, UINT(vertex_stream_.size()), views);
	}

	//TODO: there is Buffer::method for that
	void Context::CommitIndexBuffer()
	{
		assert(index_buffer_);

		auto ib_view = index_buffer_->GetIndexBufferView();
		command_list_->IASetIndexBuffer(&ib_view);
	}

	void Context::PrepareDraw()
	{
		assert(pipeline_);
		
		//TODO: is pso_desc.RT_count_ necessary?
		//TODO: artificial, handle SwapChain RTV
		//const auto& pso_desc = pipeline_->GetDesc();
		//assert(pso_desc.RT_count_ == (UINT)bound_rtv_.size());

		CommitVertexBuffers();

		command_list_->SetGraphicsRootSignature(pipeline_->GetRootSignature());
		//pipeline_->CommitConstantBuffers(this);
	}

	void Context::PrepareDrawIndexed()
	{
		PrepareDraw();
		CommitIndexBuffer();
	}

    void Context::SetDescriptorHeaps(DescriptorHeaps& desc) noexcept
	{
		assert(desc.generic_ != nullptr || desc.sampler_ != nullptr);
		assert(desc.generic_ == nullptr || desc.generic_->GetDesc().Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		assert(desc.sampler_ == nullptr || desc.sampler_->GetDesc().Type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		
		//if (desc == bound_descriptors_)
		//{
		//	return;
		//}
		
		bound_descriptors_ = desc;
		
		////////////////////////////////////////////////////////////////////////////////
		
		//TODO: MAGIC
		ID3D12DescriptorHeap** heaps = reinterpret_cast<ID3D12DescriptorHeap**>(&desc);
		UINT heaps_count = (heaps[0] != nullptr ? 1 : 0) + (heaps[1] != nullptr ? 1 : 0);
		
		if (heaps[0] == nullptr)
		{
			++heaps;
		}
		
		command_list_->SetDescriptorHeaps(heaps_count, heaps);

	}

	////////////////////////////////////////////////////////////////////////////////

	//TODO: Variable 'madX::Backend::Context::scisors_' is uninitialized
	//TODO: Variable 'madX::Backend::Context::viewports_' is uninitialized

	Context::Context(Processor* processor)
	    : processor_ { processor }
		, viewports_{ D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE, D3D12_VIEWPORT() }
		, scisors_{ D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE , D3D12_RECT() }
	{
		D3D12_COMMAND_SIGNATURE_DESC signature_desc = {};
		D3D12_INDIRECT_ARGUMENT_DESC argument_desc = {};

		//auto device = processor->GetDevice();

		signature_desc.NodeMask = processor->GetNode();
		signature_desc.NumArgumentDescs = 1;
		signature_desc.pArgumentDescs = &argument_desc;

		//TODO: rething this...
		vertex_stream_.reserve(cContext_MaxSlotsCount);

		////////////////////////////////////////////////////////////////////////////////

		//	HRESULT result;
		//	
		//	signature_desc.ByteStride = sizeof(UINT) * 4; //TODO: check it
		//	argument_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;
		//
		//	result = GetDevice()->CreateCommandSignature(&signature_desc, nullptr, IID_PPV_ARGS(signature_draw_indirect_.ReleaseAndGetAddressOf()));
		//	ThrowIfFailed(result, "CreateCommandSignature failed on CommandSignature::IndirectDraw");
		//	
		//	signature_desc.ByteStride = sizeof(UINT) * 5; //TODO: check it
		//	argument_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
		//	
		//	result = GetDevice()->CreateCommandSignature(&signature_desc, nullptr, IID_PPV_ARGS(signature_draw_indexed_.ReleaseAndGetAddressOf()));
		//	ThrowIfFailed(result, "CreateCommandSignature failed on CommandSignature::IndexedIndirectDraw");
		//	
		//	signature_desc.ByteStride = sizeof(UINT) * 3; //TODO: check it
		//	argument_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
		//	
		//	result = GetDevice()->CreateCommandSignature(&signature_desc, nullptr, IID_PPV_ARGS(signature_dispatch_indirect_.ReleaseAndGetAddressOf()));
		//	ThrowIfFailed(result, "CreateCommandSignature failed on CommandSignature::DispatchIndirectDraw");

		////////////////////////////////////////////////////////////////////////////////

		//	for (unsigned index = 0; index < D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE; ++index)
		//	{
		//		viewports_[index] = CD3DX12_VIEWPORT(0.0f, 0.0f, 0.0f, 0.0f);
		//		scisors_[index] = CD3DX12_RECT(0, 0, D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION, D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION);
		//	}
	}

	Context::~Context()
	{
		//	auto logger = Core::Logger::GetInstance();
		//	
		//	if (IsDeffered())
		//	{
		//		if (command_context_)
		//		{
		//			logger->Warning(name + ": CommandContextOld not executed yet, will be disposed now");
		//			render_->DisposeCommandContext(std::move(command_context_));
		//		}
		//	}
		//	else
		//	{
		//		Flush(false);
		//	}
		//	
		//	FinishFrame();
	}

//	void Context::ClearRenderTarget(TextureView* view, const float* rgba) 
//	{
//		assert(view && view->GetResourceViewType() == ResourceView::RenderTarget);
//		
//		if (rgba == nullptr)
//		{
//			rgba = cTextureClearValue_ColorZero.data();
//		}
//			
//		if (auto texture = view->GetTexture(); texture->CheckTransitionState(D3D12_RESOURCE_STATE_RENDER_TARGET))
//		{
//			//	TransitionResource(texture, D3D12_RESOURCE_STATE_RENDER_TARGET);
//			assert(false);
//		}
//	
//		FlushResourceBarriers();
//	
//		command_list_->ClearRenderTargetView
//		(
//			view->GetHandleCPU(),
//			rgba,
//			0,
//			nullptr
//		);
//	}

//	void Context::ClearDepthStencil(TextureView* view, float depth, std::uint8_t stencil, D3D12_CLEAR_FLAGS flags)
//	{
//		assert(view && view->GetResourceViewType() == ResourceView::DepthStencil);
//			
//		if (auto texture = view->GetTexture(); texture->CheckTransitionState(D3D12_RESOURCE_STATE_DEPTH_WRITE))
//		{
//			TransitionResource(texture, D3D12_RESOURCE_STATE_DEPTH_WRITE);
//		}
//		
//		FlushResourceBarriers();
//		command_list_->ClearDepthStencilView
//		(
//			view->GetDescriptorHandle(),
//			flags,
//			depth,
//			stencil,
//			0,
//			nullptr
//		);
//	}

	void Context::SetPipeline(std::shared_ptr<Pipeline>& pipeline)
	{	
		assert(command_list_ != nullptr);
		
		pipeline_ = pipeline; 	

		command_list_->SetPipelineState(pipeline->GetPipelineState());
		//command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		command_list_->OMSetStencilRef(stencil_);
		command_list_->OMSetBlendFactor(blend_factor_);
		
		//CommitRenderTargets();
		CommitViewports();
		CommitScisors();
	}

	void Context::CommitShaderResources()
	{
		assert(pipeline_);
		assert(command_list_);
		pipeline_->CommitShaderResources(*this);
	}

	void Context::CommitConstantBuffers(const std::shared_ptr<Buffer>& buffer)
	{
		assert(pipeline_);
		assert(command_list_);
		pipeline_->CommitConstantBuffers(command_list_, buffer);
	}

	void Context::Draw(const DrawAttributes& attributes)
	{
		assert(pipeline_);

		PrepareDraw();
		FlushResourceBarriers();

		command_list_->DrawInstanced
		(
			attributes.vertices_,
			attributes.instances_,
			attributes.first_vertex_index_,
			attributes.first_instance_index_
		);
	}

	void Context::DrawIndexed(const DrawIndexedAttributes& attributes)
	{
		assert(pipeline_);		
		//assert(index_buffer_->GetBufferSize() >= index_buffer_->GetBufferStride() * attributes.indices_);

		PrepareDrawIndexed();
		FlushResourceBarriers();

		command_list_->DrawIndexedInstanced
		(
			attributes.indices_,
			attributes.instances_,
			attributes.first_index_index_,
			attributes.base_vertex_,
			attributes.first_instance_index_
		);
	}

	void Context::SetScissorRects(std::size_t count, const D3D12_RECT* scissors)
	{
		assert(count < scisors_.size());
		assert(count > 0 && scissors != nullptr);
		
		auto scisors_count = std::min(count, scisors_.size());
			
		for (auto index = 0; index < scisors_count; ++index)
		{
			assert(scissors[index].left <= scissors[index].right);
			assert(scissors[index].top <= scissors[index].bottom);
			scisors_[index] = scissors[index];
		}
			
		//assert(count == scisors_count_);
			
		//	if (pipeline_)
		//	{
		//		CommitScisors();
		//	}

		CommitScisors();
	}

	void Context::SetBlendFactors(const float* factors)
	{
		assert(command_list_ != nullptr);
		command_list_->OMSetBlendFactor(factors);
	}

	//TODO: improve me: Core::Array
	//TODO: check start_slot vs reset
	void Context::SetVertexBuffer(std::uint32_t start_slot,	std::vector<VertexStream>& vertex_stream)
	{
		assert(start_slot == 0);
		vertex_stream_ = std::move(vertex_stream);

		//	//TODO: actual SetVertexBuffer machine...
		//	{
		//		vertex_stream_ = std::move(vertex_stream);
		//		for (auto& stream : vertex_stream_)
		//		{
		//		//TODO: check it
		//		//	TransitionResource
		//		//	(
		//		//		stream.first.get(),
		//		//		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		//		//	);
		//		}
		//	}

		//	if (reset)
		//	{
		//		for (std::uint32_t slot = 0; slot < start_slot; ++slot)
		//		{
		//			vertex_stream_[slot] = {};
		//		}
		//	
		//		for (auto slot = start_slot + buffers.size(); slot < vertex_stream_count_; ++slot)
		//		{
		//			vertex_stream_[slot] = {};
		//		}
		//	
		//		vertex_stream_count_ = 0;
		//	}
		//	
		//	vertex_stream_count_ = std::max(vertex_stream_count_, start_slot + (std::uint32_t)buffers.size());
		//	
		//	for (std::uint32_t index = 0; index < (std::uint32_t)buffers.size(); ++index)
		//	{
		//		auto& stream = vertex_stream_[start_slot + index];
		//		stream.buffer_ = buffers.size() > index ? buffers[index] : nullptr;
		//		stream.offset_ = offsets.size() > index ? offsets[index] : 0;
		//	
		//		if (stream.buffer_)
		//		{
		//			const auto bind = stream.buffer_->GetBufferBind();
		//			assert(bind == ResourceBind::VertexBuffer);
		//		}
		//	}
		//		
		//	while (vertex_stream_count_ > 0 && !vertex_stream_[vertex_stream_count_ - 1].buffer_)
		//	{
		//		vertex_stream_[vertex_stream_count_--] = {};
		//	}
		//	
		//	////////////////////////////////////////////////////////////////////////////////
		//	
		//	for (std::uint32_t index = 0; index < vertex_stream_count_; ++index)
		//	{	
		//		if (auto& stream = vertex_stream_[index]; stream.buffer_)
		//		{
		//			TransitionResource(stream.buffer_.get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		//		}
		//	}
	}

	//TODO: const me
	void Context::SetIndexBuffer(std::shared_ptr<Buffer>& buffer, std::uint32_t offset)
	{
		index_buffer_ = buffer;
		index_data_offset_ = offset;
		
		//TODO: check it
		//	if (buffer)
		//	{
		//		assert(index_buffer_->GetBufferBind() == ResourceBind::IndexBuffer);	
		//		//TODO: check it
		//		//TransitionResource(index_buffer_.get(), D3D12_RESOURCE_STATE_INDEX_BUFFER);
		//	}
	}
}



//	void Context::SetRenderTargets(std::vector<TextureView*>& rtv_vector, TextureView* dsv)
//	{
//		bound_rtv_ = std::move(rtv_vector);
//
//		//TODO: bound_dsv_ = dsv;
//		assert(dsv == nullptr);
//
//		CommitRenderTargets();
//
//		auto resource = (*bound_rtv_.begin())->GetTexture()->GetResource();
//
//		D3D12_VIEWPORT viewports[1];
//		viewports[0] = CD3DX12_VIEWPORT(resource);
//
//		SetViewports(std::span{ viewports });
//	}

//	void Context::SetViewports(std::vector<D3D12_VIEWPORT>& viewports)
//	{
//		//SetViewports(viewports.size(), viewports.data());
//
//		//assert(count < _countof(viewports_));
//		//assert(count < viewports_.size());
//		//assert(count > 0 && viewports != nullptr);
//
//		//assert(!viewports.empty());
//		//assert(viewports.size() < viewports_.size());
//
//		//viewports_count_ = std::min(count, (std::uint32_t)(_countof(viewports_)));
//
//		//for (unsigned index = 0; index < viewports.size(); ++index)
//		//{
//		//	//assert(viewports[index].Width >= 0.0f);
//		//	//assert(viewports[index].Height >= 0.0f);
//		//	assert(viewports[index].MaxDepth >= viewports[index].MinDepth);
//		//	viewports_[index] = viewports[index];
//		//}
//
//		//assert(count == viewports_count_);
//		//assert(count == viewports_.size());
//
//		//CommitViewports();
//	}

//	void Context::TransitionResource(Buffer* resource, D3D12_RESOURCE_STATES state)
//	{
//		assert(resource != nullptr);
//
//		if (resource->IsResourceStateValid() && !resource->CheckResourceState(state))
//		{
//			ResourceBarrierDesc barrier(resource, D3D12_RESOURCE_STATE_INVALID, state);
//			TransitionResource(barrier, ResourceType::Buffer);
//		}
//	}

//	void Context::TransitionResource(Texture* resource, D3D12_RESOURCE_STATES state)
//	{
//		assert(resource != nullptr);
//
//		if (resource->IsResourceStateValid() && !resource->CheckResourceState(state))
//		{
//			ResourceBarrierDesc barrier(resource, D3D12_RESOURCE_STATE_INVALID, state);
//			TransitionResource(barrier, ResourceType::Texture);
//		}
//	}

//	void Context::TransitionResource(const ResourceBarrierDesc& barrier_desc, ResourceType type)
//	{
//		//TODO: invatigate this...
//
//		assert(type != ResourceType::Undefined);
//		assert(barrier_desc.resource_ != nullptr);
//		assert(barrier_desc.after_ != D3D12_RESOURCE_STATE_INVALID);
//
//		////////////////////////////////////////////////////////////////////////////////
//
//		//TODO: move it to Resource/Buffer
//		if (type == ResourceType::Buffer)
//		{
//			auto* buffer = (Buffer*)(barrier_desc.resource_);
//
//			const auto usage = buffer->GetBufferUsage();
//			const auto bind = buffer->GetBufferBind();
//			if (usage == ResourceUsage::Dynamic && bind != ResourceBind::ShaderResource && bind != ResourceBind::UnorderedAccess)
//			{
//				assert(buffer->GetResourceState() == D3D12_RESOURCE_STATE_GENERIC_READ);
//				assert((barrier_desc.after_ & D3D12_RESOURCE_STATE_GENERIC_READ) == barrier_desc.after_);
//			}
//		}
//
//		////////////////////////////////////////////////////////////////////////////////
//
//		auto resource = barrier_desc.resource_->GetResource();
//		auto before = barrier_desc.resource_->GetResourceState();
//
//		assert(before != D3D12_RESOURCE_STATE_INVALID);
//		assert(barrier_desc.before_ == D3D12_RESOURCE_STATE_INVALID || barrier_desc.before_ == before);
//
//		if (before != barrier_desc.after_ && ((before | barrier_desc.after_) != before || barrier_desc.after_ == D3D12_RESOURCE_STATE_COMMON))
//		{
//			auto after = barrier_desc.after_;
//
//			if ((after & D3D12_RESOURCE_STATE_GENERIC_READ) == after && (before & D3D12_RESOURCE_STATE_GENERIC_READ) == before)
//			{
//				after |= before;
//			}
//
//			assert(before != after);
//
//			//TODO: check it
//			//assert(barrier_desc.transition_ == D3D12_RESOURCE_BARRIER_FLAG_NONE || barrier_desc.transition_ == D3D12_RESOURCE_BARRIER_FLAG_END_ONLY);
//			assert(barrier_desc.transition_ == D3D12_RESOURCE_BARRIER_FLAG_NONE);
//
//			//TODO: check it
//			//	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
//			//	(
//			//		resource,
//			//		before,
//			//		after,
//			//		D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
//			//		barrier_desc.transition_
//			//	);
//
//			D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
//			(
//				resource,
//				before,
//				after
//			);
//
//			barriers_.emplace_back(std::move(barrier));
//
//			barrier_desc.resource_->SetResourceState(after);
//		}
//
//		if (before == D3D12_RESOURCE_STATE_UNORDERED_ACCESS && barrier_desc.after_ == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
//		{
//			assert(barrier_desc.transition_ == D3D12_RESOURCE_BARRIER_FLAG_NONE);
//
//			D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(resource);
//			barriers_.emplace_back(std::move(barrier));
//		}
//	}