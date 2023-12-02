import <cassert>;

import stl.core;
import microsoft.directx;
import madX.Backend.Settings;
import madX.Backend.Exception;
import madX.Backend.Fence;
import madX.Backend.Draw;
import madX.Backend.Commands;
import madX.Backend.HeapDescriptor;
import madX.Backend.Pipeline;

#include "d3d12.h"

namespace madX::Backend
{
	class Processor; 
	class Pipeline;
	class Buffer;
	class Texture;
	class TextureView;

	using VertexStream = std::pair<std::shared_ptr<Buffer>, std::size_t>;

	class Context final
	{
		//ComPtr<ID3D12CommandSignature> signature_draw_indirect_;
		//ComPtr<ID3D12CommandSignature> signature_draw_indexed_;
		//ComPtr<ID3D12CommandSignature> signature_dispatch_indirect_;

		Processor* processor_ = nullptr;

		std::shared_ptr<Pipeline> pipeline_;
		std::shared_ptr<Buffer> index_buffer_;
		std::vector<VertexStream> vertex_stream_;
		std::vector<TextureView*> bound_rtv_;
		TextureView* bound_dsv_ = nullptr;

		////////////////////////////////////////////////////////////////////////////////

		std::vector<D3D12_VIEWPORT> viewports_;
		std::vector<D3D12_RECT> scisors_;

		UINT stencil_ = 0;
		float blend_factor_[4] = { -1.0f, -1.0f, -1.0f, -1.0f };

		std::uint32_t index_data_offset_ = 0;

		////////////////////////////////////////////////////////////////////////////////

		DescriptorHeaps bound_descriptors_;
		ID3D12GraphicsCommandList* command_list_ = nullptr;

		std::vector<D3D12_RESOURCE_BARRIER> barriers_;

		////////////////////////////////////////////////////////////////////////////////

		void CommitViewports();
		void CommitScisors();
		void CommitVertexBuffers();
		void CommitIndexBuffer();
		//void CommitRenderTargets(); //TODO: implement usage

		void PrepareDraw();
		void PrepareDrawIndexed();

		//void TransitionResource(const ResourceBarrierDesc& barrier_desc, ResourceType type);

		void FlushResourceBarriers()
		{
			if (!barriers_.empty())
			{
				assert(false);
				command_list_->ResourceBarrier((UINT)barriers_.size(), barriers_.data());
				barriers_.clear();
			}
		}

	public:
		explicit Context(Processor* processor);
		~Context();

		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;

		Context(Context&&) = default;
		Context& operator=(Context&&) = default;

		////////////////////////////////////////////////////////////////////////////////

		Processor* GetProcessor () const noexcept
		{
			assert(processor_ != nullptr);
			return processor_;
		}

		Pipeline* GetPipeline() noexcept
		{
			return pipeline_.get();
		}

		void SetCommandList(ID3D12GraphicsCommandList* command_list) noexcept
		{
			assert(command_list);
			command_list_ = command_list;
		}

		ID3D12GraphicsCommandList* GetCommandList() noexcept
		{
			return command_list_;
		}

		//TODO: add pure ID3D12Resource, combine Buffer/Texture?
		//TODO: move it to Resource
		//void TransitionResource(class Buffer* resource, D3D12_RESOURCE_STATES state);
		//void TransitionResource(class Texture* resource, D3D12_RESOURCE_STATES state);

		////////////////////////////////////////////////////////////////////////////////

		void EndFrame();

		void SetDescriptorHeaps(DescriptorHeaps& desc_heaps) noexcept;

		//void ClearRenderTarget(TextureView* view, const float* rgba);
		//void ClearDepthStencil(TextureView* view, float depth, std::uint8_t stencil, D3D12_CLEAR_FLAGS flags);

		void SetPipeline(std::shared_ptr<Pipeline>& pipeline);

		void CommitShaderResources();
		void CommitConstantBuffers(const std::shared_ptr<Buffer>&);

		//void SetRenderTargets(std::vector<TextureView*>& rtv_vector, TextureView* dsv);
		void SetViewports(std::vector<D3D12_VIEWPORT>& viewports)
		{
			viewports_ = std::move(viewports);
		}

		void SetScissorRects(std::size_t count, const D3D12_RECT* scissors);
		void SetBlendFactors(const float* factors);

		void SetVertexBuffer(std::uint32_t start_slot, std::vector<VertexStream>& vertex_stream);
		void SetIndexBuffer(std::shared_ptr<Buffer>& buffer, std::uint32_t offset);

		void Draw(const DrawAttributes& attributes);
		void DrawIndexed(const DrawIndexedAttributes& attributes);

		////////////////////////////////////////////////////////////////////////////////

		void MarkBeginEventPIX(std::wstring format) const noexcept
		{
		}

		void MarkEndEventPIX() const noexcept
		{
		}
	};
}
