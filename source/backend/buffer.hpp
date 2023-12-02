import <cassert>;

import stl.core;
import microsoft.directx;
import madX.Backend.Extensions;
import madX.Backend.Exception;
import madX.Backend.Resource;

import <winerror.h>;
import <combaseapi.h>;

namespace madX::Backend
{
	struct BufferDesc final
	{
		ResourceBind bind_ = ResourceBind::None;
		ResourceUsage usage_ = ResourceUsage::Default;

		std::uint32_t count_ = 0;
		std::uint32_t stride_ = 0;

		const void* data_ = nullptr;

		std::string name_;
	};

	////////////////////////////////////////////////////////////////////////////////

	class Processor;

	class Buffer final : public Resource, public std::enable_shared_from_this<Buffer>
	{
		//TODO: no allocation per context, rather per "frame"
		//std::vector<PoolDynamic::Allocation> allocation_;
		Allocation allocation_;

		//TODO: state
		mutable std::uint32_t size_ = 0;
		const std::uint32_t count_ = 0;
		const std::uint32_t stride_ = 0;
		const ResourceUsage usage_ = ResourceUsage::Default;
		const ResourceBind bind_ = ResourceBind::None;

		std::string name_;
	public:
		//Buffer(Processor* processor, const BufferDesc& desc, const BufferData* data);
		Buffer(Processor*, const BufferDesc&);

		////////////////////////////////////////////////////////////////////////////////

		const auto GetBufferCount() const noexcept
		{
			return count_;
		}

		const auto GetBufferStride() const noexcept
		{
			return stride_;
		}

		const auto GetBufferUsage() const noexcept 
		{
			return usage_;
		}

		const auto GetBufferBind() const noexcept 
		{
			return bind_;
		}

		////////////////////////////////////////////////////////////////////////////////

		[[nodiscard]] const D3D12_CPU_VIRTUAL_ADDRESS GetAddressCPU() const noexcept
		{
			return allocation_.cpu_;
		}
		
		[[nodiscard]] const D3D12_GPU_VIRTUAL_ADDRESS GetAddressGPU() const noexcept
		{
			return allocation_.gpu_;
		}

		////////////////////////////////////////////////////////////////////////////////

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const noexcept
		{
			//assert(usage_ == ResourceUsage::Static);
			assert(bind_ == ResourceBind::VertexBuffer);

			D3D12_VERTEX_BUFFER_VIEW view;
			view.BufferLocation = allocation_.gpu_;
			view.SizeInBytes = size_;
			view.StrideInBytes = stride_;

			return view;
		}

		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const noexcept
		{
			//assert(usage_ == ResourceUsage::Static);
			assert(bind_ == ResourceBind::IndexBuffer);
			assert(stride_ == 2 || stride_ == 4);

			D3D12_INDEX_BUFFER_VIEW view;
			view.BufferLocation = allocation_.gpu_;
			view.SizeInBytes = size_;
			view.Format = stride_ == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

			return view;
		}

		const auto& GetName() const noexcept
		{
			assert(false == name_.empty());
			return name_;
		}
	};
}