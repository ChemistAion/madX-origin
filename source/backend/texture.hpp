import <cassert>;

import stl.core;
import microsoft.directx;
import madX.Backend.Exception;
import madX.Backend.Resource;
import madX.Backend.HeapDescriptor;

import <winerror.h>;
import <combaseapi.h>;

namespace madX::Backend
{
	class Processor;
	class Buffer;

	struct TextureDepthClearValue
	{
		//float depth_ = 1.0f;
		float depth_ = 0.0f;
		std::uint8_t stencil_ = 0;

		TextureDepthClearValue() noexcept = default;

		TextureDepthClearValue(float depth, std::uint8_t stencil) noexcept
			: depth_{ depth }
			, stencil_{ stencil }
		{
		}
	};

	//TODO: make it C-style array
	constexpr std::array<float, 4> cTextureClearValue_ColorZero = { 0.0f, 0.0f, 0.0f, 1.0f };

	struct TextureClearValue
	{
		//DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
		std::array<float, 4> color_ = cTextureClearValue_ColorZero;
		TextureDepthClearValue depth_stencil_;

		TextureClearValue() noexcept = default;

		//bool operator == (const TextureClearValue& rhs) const
		//{
		//	return
		//		format_ == rhs.format_ &&
		//		color_[0] == rhs.color_[0] &&
		//		color_[1] == rhs.color_[1] &&
		//		color_[2] == rhs.color_[2] &&
		//		color_[3] == rhs.color_[3] &&
		//		depth_stencil_.depth_ == rhs.depth_stencil_.depth_ &&
		//		depth_stencil_.stencil_ == rhs.depth_stencil_.stencil_;
		//}
	};

	////////////////////////////////////////////////////////////////////////////////

	struct TextureDesc
	{
		std::uint64_t width_ = 0;
		std::uint16_t height_ = 0;	
		std::uint16_t depth_ = 1;
		std::uint16_t mip_levels_ = 1;
		std::uint32_t sample_count_ = 1; //TODO: 16b

		ResourceDimension dimension_ = ResourceDimension::Undefined;

		DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
		ResourceUsage usage_ = ResourceUsage::Default;
		ResourceBind bind_ = ResourceBind::None;

		TextureClearValue clear_value_;

		std::string name_;
	};

	////////////////////////////////////////////////////////////////////////////////

	struct TextureResourceData
	{
		const void* data_ = nullptr;
	
		std::uint32_t offset_ = 0;
		std::uint32_t stride_ = 0;
		std::uint32_t depth_stride_ = 0;
	
		TextureResourceData() noexcept = default;
	
		TextureResourceData
		(
			const void* data,
			std::uint32_t stride,
			std::uint32_t depth_stride
		) noexcept
			: data_{ data }
			, stride_{ stride }
			, depth_stride_{ depth_stride }
		{
		}
	};
	
	struct TextureData
	{
		TextureResourceData* sub_resources_ = nullptr;
		std::uint32_t count_ = 0;
	
		TextureData() noexcept = default;
	
		//TODO: vector based
		TextureData(TextureResourceData* sub_resources, std::uint32_t count) noexcept
			: sub_resources_{ sub_resources }
			, count_{ count }
		{
		}
	};
	
	////////////////////////////////////////////////////////////////////////////////

	class Texture;

	using NativeViewDesc = std::variant<std::monostate, D3D12_SHADER_RESOURCE_VIEW_DESC, D3D12_RENDER_TARGET_VIEW_DESC, D3D12_DEPTH_STENCIL_VIEW_DESC, D3D12_UNORDERED_ACCESS_VIEW_DESC>;

	struct TextureViewDesc
	{
		ResourceView type_ = ResourceView::Undefined;

		//TODO: check dimension_/format_ superfluous?
		ResourceDimension dimension_ = ResourceDimension::Undefined;
		DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
		
		std::uint32_t most_mip_level_ = 0;
		std::uint32_t mip_levels_ = 1;

		std::uint32_t first_slice_ = 0;
		std::uint32_t count_slice_ = 0;
	};

	class TextureView final
	{
		//TODO: hard-fix to be removed due to special case (Binding)
		//std::shared_ptr<Texture> texture_;
		HeapDescriptor::Element element_ = {};

		//STATE
		ResourceView type_ = ResourceView::Undefined;
	public:
		TextureView() = default;

		explicit TextureView
		(
			Processor* processor,
			const TextureViewDesc& desc,
			const std::shared_ptr<Texture>& texture,
			HeapDescriptor::Element&& element
		);

		TextureView(const TextureView&) = delete;
		TextureView& operator=(const TextureView&) = delete;

		TextureView(TextureView&&) = default;
		TextureView& operator=(TextureView&&) = default;

	//	Texture* GetTexture()
	//	{
	//		assert(texture_);
	//		return texture_.get();
	//	}

		auto GetResourceViewType() const noexcept
		{
			//assert(texture_);
			return type_;
		}

		auto& GetElement() noexcept
		{
			return element_;
		}

		const D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const noexcept
		{
			//assert(texture_);
			assert(element_.IsValid());
			return element_.GetHandleCPU(0);
		}

		const D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const noexcept
		{
			//assert(texture_);
			assert(element_.IsValid());
			return element_.GetHandleGPU(0);
		}
	};

	////////////////////////////////////////////////////////////////////////////////

	class Texture final : public Resource, public std::enable_shared_from_this<Texture>
	{
		//TODO: static? move it to Processor 
		D3D12_SHADER_RESOURCE_VIEW_DESC CreateSRV(Processor*, TextureViewDesc&, TextureView&);
		D3D12_RENDER_TARGET_VIEW_DESC CreateRTV(Processor*, TextureViewDesc&, TextureView&);
		D3D12_DEPTH_STENCIL_VIEW_DESC CreateDSV(Processor*, TextureViewDesc&, TextureView&);
		D3D12_UNORDERED_ACCESS_VIEW_DESC CreateUAV(Processor*, TextureViewDesc&, TextureView&);

		//STATE
		ResourceUsage usage_ = ResourceUsage::Default;
		ResourceBind bind_ = ResourceBind::None;
		DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
		ResourceDimension dimension_ = ResourceDimension::Undefined;
		
		std::uint64_t width_ = 0;
		std::uint16_t height_ = 0;
		std::uint16_t depth_ = 0;
		std::uint16_t mip_levels_ = 0;
		std::uint32_t sample_count_ = 0;

		std::string name_;
	public:
		//Texture() = default;

		explicit Texture(Processor* processor, const TextureDesc& desc, const TextureData* data);
		//explicit Texture(Processor* processor, const TextureDesc& desc, ID3D12Resource* resource, D3D12_RESOURCE_STATES state);

		////////////////////////////////////////////////////////////////////////////////
		
		NativeViewDesc CreateView(Processor*, const TextureViewDesc&, TextureView&);

		static void CreateNullBind(Processor*, NativeViewDesc);
		
		const auto& GetName() const noexcept
		{
			assert(false == name_.empty());
			return name_;
		}
	};
}
