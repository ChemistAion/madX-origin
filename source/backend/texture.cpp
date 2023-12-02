#include "texture.hpp"

import madX.Backend.Processor;

//https://stackoverflow.com/questions/67548981/how-to-render-to-texture-in-directx12-c-what-is-the-process
namespace madX::Backend
{
	TextureView::TextureView
	(
		Processor* processor,
		const TextureViewDesc& desc,
		const std::shared_ptr<Texture>& texture,
		HeapDescriptor::Element&& element
	)
	//	: texture_ { texture }
		: element_ { std::move(element) }
		, type_ { desc.type_ }
	{
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC Texture::CreateSRV(Processor* processor, TextureViewDesc& desc, TextureView& view)
	{
		assert(processor != nullptr);
		assert(desc.type_ == ResourceView::ShaderResource);

		//TODO: improve checks here
		//assert(bind_ == ResourceBind::ShaderResource);

		assert(format_ != DXGI_FORMAT_UNKNOWN);

		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		//std::memset(&srv_desc, 0, sizeof(srv_desc));

		srv_desc.Format = desc.format_ == DXGI_FORMAT_UNKNOWN ? format_ : desc.format_;
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		//TODO: hard-fix
		//desc.mip_levels_ = 1;

		switch (dimension_)
		{
		//TODO: case ResourceDimension::Buffer
		case ResourceDimension::Texture1D:
		{
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
			srv_desc.Texture1D.MostDetailedMip = desc.most_mip_level_;
			srv_desc.Texture1D.MipLevels = desc.mip_levels_;
			srv_desc.Texture1D.ResourceMinLODClamp = 0.0f;
			break;
		}
		case ResourceDimension::Texture1D_Array:
		{
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
			srv_desc.Texture1DArray.ArraySize = desc.count_slice_;
			srv_desc.Texture1DArray.FirstArraySlice = desc.first_slice_;
			srv_desc.Texture1DArray.MostDetailedMip = desc.most_mip_level_;
			srv_desc.Texture1DArray.MipLevels = desc.mip_levels_;
			srv_desc.Texture1DArray.ResourceMinLODClamp = 0.0f;
			break;
		}
		case ResourceDimension::Texture2D:
		{
			if (sample_count_ > 1)
			{
				assert(false);
				//srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
				//srv_desc.Texture2DMS.UnusedField_NothingToDefine = 0;
			}
			else
			{
				srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srv_desc.Texture2D.MostDetailedMip = desc.most_mip_level_;
				srv_desc.Texture2D.MipLevels = desc.mip_levels_;
				srv_desc.Texture2D.PlaneSlice = 0;
				srv_desc.Texture2D.ResourceMinLODClamp = 0.0f;
			}
			break;
		}
		case ResourceDimension::Texture2D_Array:
		{
			if (sample_count_ > 1)
			{
				assert(false);
				//srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
				//srv_desc.Texture2DMSArray.ArraySize = desc.count_slice_;
				//srv_desc.Texture2DMSArray.FirstArraySlice = desc.first_slice_;
				//srv_desc.Texture2DArray.PlaneSlice = 0;
				//srv_desc.Texture2DArray.ResourceMinLODClamp = 0.0f;
			}
			else
			{
				srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				srv_desc.Texture2DArray.ArraySize = desc.count_slice_;
				srv_desc.Texture2DArray.FirstArraySlice = desc.first_slice_;
				srv_desc.Texture2DArray.MostDetailedMip = desc.most_mip_level_;
				srv_desc.Texture2DArray.MipLevels = desc.mip_levels_;
			}
			break;
		}
		case ResourceDimension::Texture3D:
		{
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			srv_desc.Texture3D.MostDetailedMip = desc.most_mip_level_;
			srv_desc.Texture3D.MipLevels = desc.mip_levels_;
			srv_desc.Texture3D.ResourceMinLODClamp = 0.0f;
			break;
		}
		case ResourceDimension::TextureCube:
		{
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srv_desc.TextureCube.MostDetailedMip = desc.most_mip_level_;
			srv_desc.TextureCube.MipLevels = desc.mip_levels_;
			srv_desc.TextureCube.ResourceMinLODClamp = 0.0f;
			break;
		}
		case ResourceDimension::TextureCube_Array:
		{
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
			srv_desc.TextureCubeArray.MostDetailedMip = desc.most_mip_level_;
			srv_desc.TextureCubeArray.MipLevels = desc.mip_levels_;
			srv_desc.TextureCubeArray.First2DArrayFace = desc.first_slice_;
			srv_desc.TextureCubeArray.NumCubes = desc.count_slice_ / 6;
			srv_desc.TextureCubeArray.ResourceMinLODClamp = 0.0f;
			break;
		}
		default:
		{
			ThrowIfFailed(E_INVALIDARG, "Invalid TextureViewDesc dimension type.");
			break;
		}
		}

		auto& element = view.GetElement();

		//assert(false == element.IsValid());
		if (false == element.IsValid())
		{
			element = processor->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
		}

		auto device = processor->GetDevice();
		device->CreateShaderResourceView(GetResource(), &srv_desc, element.GetHandleCPU(0));

		//SetObjectName(GetResource(), desc.name_);
		//result = mem_gpu_.Get()->SetName(L"PoolStatic::System");
		//ThrowIfFailed(result, "PoolStatic::System::SetName failed");

		view = std::move( TextureView { processor, desc, shared_from_this(), std::move(element) });

		return srv_desc;
	}
	
	D3D12_RENDER_TARGET_VIEW_DESC Texture::CreateRTV(Processor* processor, TextureViewDesc& desc, TextureView& view)
	{
		assert(processor != nullptr);

		assert(desc.type_ == ResourceView::RenderTarget);
		assert(bind_ == ResourceBind::RenderTarget || bind_ == ResourceBind::RenderTargetUnorderedAccess);

		assert(format_ != DXGI_FORMAT_UNKNOWN);	

		D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
		//std::memset(&rtv_desc, 0, sizeof(rtv_desc));

		rtv_desc.Format = desc.format_ == DXGI_FORMAT_UNKNOWN ? format_ : desc.format_;

		switch (dimension_)
		{
		case ResourceDimension::Texture1D:
		{
			rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
			rtv_desc.Texture1D.MipSlice = desc.most_mip_level_;
			break;
		}
		case ResourceDimension::Texture1D_Array:
		{
			rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
			rtv_desc.Texture1DArray.ArraySize = desc.count_slice_;
			rtv_desc.Texture1DArray.FirstArraySlice = desc.first_slice_;
			rtv_desc.Texture1DArray.MipSlice = desc.most_mip_level_;
			break;
		}
		case ResourceDimension::Texture2D:
		{
			if (sample_count_ > 1)
			{
				assert(false);
				//rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
				//rtv_desc.Texture2DMS.UnusedField_NothingToDefine = 0;
			}
			else
			{
				rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				rtv_desc.Texture2D.MipSlice = desc.most_mip_level_;
				rtv_desc.Texture2D.PlaneSlice = 0;
			}
			break;
		}
		case ResourceDimension::Texture2D_Array:
		{
			if (sample_count_ > 1)
			{
				assert(false);
				//rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
				//rtv_desc.Texture2DMSArray.ArraySize = desc.count_slice_;
				//rtv_desc.Texture2DMSArray.FirstArraySlice = desc.first_slice_;
				//rtv_desc.Texture2DArray.PlaneSlice = 0;
			}
			else
			{
				rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				rtv_desc.Texture2DArray.ArraySize = desc.count_slice_;
				rtv_desc.Texture2DArray.FirstArraySlice = desc.first_slice_;
				rtv_desc.Texture2DArray.MipSlice = desc.most_mip_level_;
			}
			break;
		}
		case ResourceDimension::Texture3D:
		{
			rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
			rtv_desc.Texture3D.FirstWSlice = desc.first_slice_;
			rtv_desc.Texture3D.WSize = desc.count_slice_;
			rtv_desc.Texture3D.MipSlice = desc.most_mip_level_;
			break;
		}
		default:
		{
			ThrowIfFailed(E_INVALIDARG, "Invalid TextureViewDesc dimension type.");
		}
		}

		////////////////////////////////////////////////////////////////////////////////
		
		auto& element = view.GetElement();
		
		//assert(false == element.IsValid());
		if (false == element.IsValid())
		{
			element = processor->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
		}

		auto device = processor->GetDevice();
		device->CreateRenderTargetView(GetResource(), &rtv_desc, element.GetHandleCPU(0));

		//SetObjectName(GetResource(), desc.name_);
		//result = mem_gpu_.Get()->SetName(L"PoolStatic::System");
		//ThrowIfFailed(result, "PoolStatic::System::SetName failed");

		view = std::move(TextureView(processor, std::move(desc), shared_from_this(), std::move(element)));
	
		return rtv_desc;
	}
	
	D3D12_DEPTH_STENCIL_VIEW_DESC Texture::CreateDSV(Processor* processor, TextureViewDesc& desc, TextureView& view)
	{
		assert(processor != nullptr);

		assert(desc.type_ == ResourceView::DepthStencil);
		assert(bind_ == ResourceBind::DepthStencil);

		assert(format_ != DXGI_FORMAT_UNKNOWN);
		
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
		//std::memset(&dsv_desc, 0, sizeof(dsv_desc));

		dsv_desc.Format = desc.format_ == DXGI_FORMAT_UNKNOWN ? format_ : desc.format_;
		dsv_desc.Flags = D3D12_DSV_FLAG_NONE;

		switch (dimension_)
		{
		case ResourceDimension::Texture1D:
		{
			dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
			dsv_desc.Texture1D.MipSlice = desc.most_mip_level_;
			break;
		}
		case ResourceDimension::Texture1D_Array:
		{
			dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
			dsv_desc.Texture1DArray.ArraySize = desc.count_slice_;
			dsv_desc.Texture1DArray.FirstArraySlice = desc.first_slice_;
			dsv_desc.Texture1DArray.MipSlice = desc.most_mip_level_;
			break;
		}
		case ResourceDimension::Texture2D:
		{
			if (sample_count_ > 1)
			{
				assert(false);
				//dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
				//dsv_desc.Texture2DMS.UnusedField_NothingToDefine = 0;
			}
			else
			{
				dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsv_desc.Texture2D.MipSlice = desc.most_mip_level_;
			}
			break;
		}
		case ResourceDimension::Texture2D_Array:
		{
			if (sample_count_ > 1)
			{
				assert(false);
				//dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
				//dsv_desc.Texture2DMSArray.ArraySize = desc.count_slice_;
				//dsv_desc.Texture2DMSArray.FirstArraySlice = desc.first_slice_;
			}
			else
			{
				dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				dsv_desc.Texture2DArray.ArraySize = desc.count_slice_;
				dsv_desc.Texture2DArray.FirstArraySlice = desc.first_slice_;
				dsv_desc.Texture2DArray.MipSlice = desc.most_mip_level_;
			}
			break;
		}
		default:
		{
			ThrowIfFailed(E_INVALIDARG, "Invalid TextureViewDesc dimension type.");
		}
		}

		////////////////////////////////////////////////////////////////////////////////

		auto& element = view.GetElement();

		//assert(false == element.IsValid());
		if (false == element.IsValid())
		{
			element = processor->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
		}

		auto device = processor->GetDevice();
		device->CreateDepthStencilView(GetResource(), &dsv_desc, element.GetHandleCPU(0));

		//SetObjectName(GetResource(), desc.name_);
		//result = mem_gpu_.Get()->SetName(L"PoolStatic::System");
		//ThrowIfFailed(result, "PoolStatic::System::SetName failed");

		view = std::move(TextureView(processor, desc, shared_from_this(), std::move(element)));
		return dsv_desc;
	}
	
	D3D12_UNORDERED_ACCESS_VIEW_DESC Texture::CreateUAV(Processor* processor, TextureViewDesc& desc, TextureView& view)
	{
		assert(processor != nullptr);

		//assert(desc.type_ == ResourceView::UnorderedAccess);
		//assert(bind_ == ResourceBind::UnorderedAccess);

		assert(format_ != DXGI_FORMAT_UNKNOWN);
		
		D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
		//std::memset(&uav_desc, 0, sizeof(uav_desc));

		uav_desc.Format = desc.format_ == DXGI_FORMAT_UNKNOWN ? format_ : desc.format_;

		switch (dimension_)
		{
		case ResourceDimension::Texture1D:
		{
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
			uav_desc.Texture1D.MipSlice = desc.most_mip_level_;
			break;
		}
		case ResourceDimension::Texture1D_Array:
		{
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
			uav_desc.Texture1DArray.ArraySize = desc.count_slice_;
			uav_desc.Texture1DArray.FirstArraySlice = desc.first_slice_;
			uav_desc.Texture1DArray.MipSlice = desc.most_mip_level_;
			break;
		}
		case ResourceDimension::Texture2D:
		{
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uav_desc.Texture2D.MipSlice = desc.most_mip_level_;
			uav_desc.Texture2D.PlaneSlice = 0;
			break;
		}
		case ResourceDimension::Texture2D_Array:
		{
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			uav_desc.Texture2DArray.ArraySize = desc.count_slice_;
			uav_desc.Texture2DArray.FirstArraySlice = desc.first_slice_;
			uav_desc.Texture2DArray.MipSlice = desc.most_mip_level_;
			uav_desc.Texture2DArray.PlaneSlice = 0;
			break;
		}
		case ResourceDimension::Texture3D:
		{
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
			uav_desc.Texture3D.FirstWSlice = desc.first_slice_;
			uav_desc.Texture3D.WSize = desc.count_slice_;
			uav_desc.Texture3D.MipSlice = desc.most_mip_level_;
			break;
		}
		default:
		{
			ThrowIfFailed(E_INVALIDARG, "Invalid TextureViewDesc dimension type.");
		}
		}

		auto& element = view.GetElement();

		assert(false == element.IsValid());
		//if (false == element.IsValid())
		{
			element = processor->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
		}

		//additional heap for UAVs with GPU shader-visible and CPU non-shader-visible
		if (desc.type_ == ResourceView::UnorderedAccessNSV)
		{
			const auto nsv = processor->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES, 1);
			element.cpu_ = nsv.cpu_;
			//element.heap_nsv_ = nsv.heap_nsv_;
		}

		auto device = processor->GetDevice();
		device->CreateUnorderedAccessView(GetResource(), nullptr, &uav_desc, element.GetHandleCPU(0));

		//SetObjectName(GetResource(), desc.name_);
		//result = mem_gpu_.Get()->SetName(L"PoolStatic::System");
		//ThrowIfFailed(result, "PoolStatic::System::SetName failed");

		view = std::move(TextureView{ processor, desc, shared_from_this(), std::move(element) });
		return uav_desc;
	}

	////////////////////////////////////////////////////////////////////////////////

	Texture::Texture(Processor* processor, const TextureDesc& desc, const TextureData* data)
		: usage_{ desc.usage_ }
		, bind_{ desc.bind_ }
		, format_{ desc.format_ }
		, dimension_{ desc.dimension_ }
		, width_{ desc.width_ }
		, height_{ desc.height_ }
		, depth_{ desc.depth_ }
		, mip_levels_{ desc.mip_levels_ }
		, sample_count_{ desc.sample_count_ }
	    , name_ {desc.name_}
	{
		if (usage_ == ResourceUsage::Static && (data == nullptr || data->sub_resources_ == nullptr))
		{
			auto error = name_ + " as StaticTexture is not initialized";
			ThrowIfFailed(E_INVALIDARG, error.c_str());
		}

		////////////////////////////////////////////////////////////////////////////////

		CD3DX12_RESOURCE_DESC resource_desc = {};

		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

		//TODO: possible multi bind
		switch (bind_)
		{
		case ResourceBind::RenderTarget:
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			break;
		}
		case ResourceBind::RenderTargetUnorderedAccess:
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			break;
		}
		case ResourceBind::UnorderedAccess:
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			break;
		}
		case ResourceBind::DepthStencil:
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			//flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
			break;
		}
		default:
			break;
		}

		//TODO: check as STATE
		switch (desc.dimension_)
		{
		case ResourceDimension::Texture1D:
		case ResourceDimension::Texture1D_Array:
		{
			resource_desc = CD3DX12_RESOURCE_DESC::Tex1D
			(
				format_,
				width_,
				depth_,
				mip_levels_,
				flags
			);

			break;
		}
		case ResourceDimension::Texture2D:
		case ResourceDimension::Texture2D_Array:
		case ResourceDimension::TextureCube:
		case ResourceDimension::TextureCube_Array:
		{
			resource_desc = CD3DX12_RESOURCE_DESC::Tex2D
			(
				format_,
				width_,
				height_,
				depth_,
				mip_levels_,
				sample_count_,
				0, //TODO: fixed
				flags
			);

			break;
		}
		case ResourceDimension::Texture3D:
		{
			resource_desc = CD3DX12_RESOURCE_DESC::Tex3D
			(
				format_,
				width_,
				height_,
				depth_,
				mip_levels_,
				flags
			);

			break;
		}
		default:
		{
			ThrowIfFailed(E_INVALIDARG, "Invalid TextureDesc dimension type.");
		}
		}

		assert(usage_ != ResourceUsage::Staging);

		////////////////////////////////////////////////////////////////////////////////

		auto device = processor->GetDevice();

		D3D12_CLEAR_VALUE clear_value;
		D3D12_CLEAR_VALUE* clear_value_ptr = nullptr;

		//TODO: separete cases
		if (resource_desc.Flags & (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL))
		{
			//TODO: check it
			//assert(resource_desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);
			//assert(desc.clear_value_.format_ != DXGI_FORMAT_UNKNOWN);
			//clear_value.Format = desc.clear_value_.format_;

			clear_value.Format = desc.format_;
			//TODO: check it
			if (resource_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
			{
				std::memcpy(clear_value.Color, desc.clear_value_.color_.data(), sizeof(clear_value.Color));
			}
			else if (resource_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
			{
				clear_value.DepthStencil.Depth = desc.clear_value_.depth_stencil_.depth_;
				clear_value.DepthStencil.Stencil = desc.clear_value_.depth_stencil_.stencil_;
			}

			clear_value_ptr = &clear_value;
		}

		auto resource_heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		//TODO: improve me for rt/v
		bool initialize = (data != nullptr && data->sub_resources_ != nullptr && data->count_ > 0);
		SetResourceState(D3D12_RESOURCE_STATE_COMMON);

		auto result = device->CreateCommittedResource
		(
			&resource_heap,
			D3D12_HEAP_FLAG_NONE,
			&resource_desc,
			D3D12_RESOURCE_STATE_COMMON,
			clear_value_ptr,
			IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
		);

		ThrowIfFailed(result, "Texture failed");
		//SetObjectName(resource_.Get(), name_);

		////////////////////////////////////////////////////////////////////////////////

		//TODO: split ctor to static/dynamic...
		//TODO: fixed to 1 subresource
		if (initialize)
		{
			std::uint32_t count = resource_desc.MipLevels;
			count *= resource_desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D
				? 1u
				: resource_desc.DepthOrArraySize;
			
			if (data->count_ != count)
			{
				ThrowIfFailed(E_INVALIDARG, "Wrong number of Texture SubResources");
			}
			
			UINT64 upload_size = 0;
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT placed[1] = {};
			UINT rows[1] = {};
			UINT64 sizes[1] = {};
			
			device->GetCopyableFootprints
			(
				&resource_desc,
				0,
				count,
				0,
				placed,
				rows,
				sizes,
				&upload_size
			);

			auto& upload = processor->GetHeapUpload();
			auto place = upload.Allocate(upload_size, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);

			assert(place != nullptr);
			placed[0].Offset += UINT64(place - upload.GetBasePointer());

			assert(data->sub_resources_ != nullptr);
			assert(data->count_ == 1);


			std::memcpy
			(
				place,
				data->sub_resources_[0].data_,
				upload_size//resource_desc.Width * resource_desc.Height * 4
			);

			////////////////////////////////////////////////////////////////////////////////

			//TODO: improve me...

			auto command_list = upload.GetCommandList();

			{
				D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource_.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
				command_list->ResourceBarrier(1, &barrier);
			}

			CD3DX12_TEXTURE_COPY_LOCATION dst(resource_.Get(), 0);
			CD3DX12_TEXTURE_COPY_LOCATION src(upload.GetResource(), placed[0]);
			command_list->CopyTextureRegion(&dst, 0, 0, 0, &src, NULL);

			{
				D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				command_list->ResourceBarrier(1, &barrier);
			}

			////////////////////////////////////////////////////////////////////////////////

			upload.Flush(processor);
		}
	}

	NativeViewDesc Texture::CreateView(Processor* processor, const TextureViewDesc& desc, TextureView& view)
	{
		try
		{
			auto updated_desc = desc;

			if (desc.type_ == ResourceView::Undefined)
			{
				ThrowIfFailed(E_INVALIDARG, "TextureView type undefined");
			}

			assert(format_ != DXGI_FORMAT_UNKNOWN);

			if (desc.dimension_ == ResourceDimension::Undefined)
			{
				if (dimension_ == ResourceDimension::TextureCube || dimension_ == ResourceDimension::TextureCube_Array)
				{
					updated_desc.dimension_ = desc.type_ == ResourceView::ShaderResource ? dimension_ : ResourceDimension::Texture2D_Array;
				}
				else
				{
					updated_desc.dimension_ = dimension_;
				}
			}

			if (desc.most_mip_level_ >= mip_levels_)
			{
				ThrowIfFailed(E_INVALIDARG, "TextureView mip levels out of range");
			}

			if (desc.mip_levels_ != std::numeric_limits<std::uint32_t>::max() && desc.most_mip_level_ + desc.mip_levels_ > mip_levels_)
			{
				ThrowIfFailed(E_INVALIDARG, "TextureView mip levels out of range");
			}

			//	if (desc.mip_levels_ == 0 || desc.mip_levels_ == std::numeric_limits<std::uint32_t>::max())
			//	{
			//		//TODO: error?
			//		//updated_desc.mip_levels_ = desc.type_ == ResourceView::ShaderResource ? desc_.mip_levels_ - desc.most_mip_level_ : 1;
			//		updated_desc.mip_levels_ = desc.type_ == ResourceView::ShaderResource ? desc.mip_levels_ - desc.most_mip_level_ : 1;
			//	}

			assert(desc.most_mip_level_ == 0);
			updated_desc.mip_levels_ = desc.mip_levels_ - desc.most_mip_level_;

			if (desc.count_slice_ == 0 || desc.count_slice_ == std::numeric_limits<std::uint32_t>::max())
			{
				bool is_array = false;

				is_array |= desc.dimension_ == ResourceDimension::Texture1D_Array;
				is_array |= desc.dimension_ == ResourceDimension::Texture2D_Array;
				is_array |= desc.dimension_ == ResourceDimension::TextureCube;
				is_array |= desc.dimension_ == ResourceDimension::TextureCube_Array;

				if (is_array)
				{
					//TODO: check it
					//updated_desc.count_slice_ = desc_.array_size_ - desc.first_slice_;
					updated_desc.count_slice_ = depth_ - desc.first_slice_;
				}
				else if (desc.dimension_ == ResourceDimension::Texture3D)
				{
					//auto depth = desc_.depth_ >> desc.most_mip_level_;
					//updated_desc.count_depth_slices_ = depth - desc.first_depth_slice_;
					
					auto depth = depth_ >> desc.most_mip_level_;
					updated_desc.count_slice_ = depth - desc.first_slice_;
				}
				else
				{
					updated_desc.count_slice_ = 1;
				}
			}

			////////////////////////////////////////////////////////////////////////////////

			switch (desc.type_)
			{
			case ResourceView::ShaderResource:
			{
				return CreateSRV(processor, updated_desc, view);
			}
			case ResourceView::RenderTarget:
			{
				return CreateRTV(processor, updated_desc, view);
			}
			case ResourceView::DepthStencil:
			{
				return CreateDSV(processor, updated_desc, view);
			}
			case ResourceView::UnorderedAccess:
			case ResourceView::UnorderedAccessNSV:
			{
				return CreateUAV(processor, updated_desc, view);
			}
			default:
			{
				assert(false);
				break;
			}
			}

			//TODO: Mip generator
			return {};
		}

		catch (...)
		{
			assert(false);
		}

		//TODO: better error handling
		ThrowIfFailed(E_FAIL, "[Texture::CreateView] failed");

		return {};
	}

	void Texture::CreateNullBind(Processor* processor, NativeViewDesc native)
	{
		if (std::holds_alternative<std::monostate>(native))
		{
			return;
		}

		HeapDescriptor::Element element;

		if (std::holds_alternative<D3D12_SHADER_RESOURCE_VIEW_DESC>(native))
		{
			const auto desc = std::get< D3D12_SHADER_RESOURCE_VIEW_DESC>(native);
			element = processor->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);

			auto device = processor->GetDevice();
			device->CreateShaderResourceView(nullptr, &desc, element.GetHandleCPU(0));

			return;
		}

		assert(false);
	}
}