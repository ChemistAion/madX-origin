import <cassert>;

import stl.core;
import microsoft.windows;
import microsoft.directx;
import madX.Backend.Exception;
import madX.Backend.Shader;
import madX.Backend.RootSignature;

using namespace Microsoft::WRL;

namespace madX::Backend
{
	class Processor;

	////////////////////////////////////////////////////////////////////////////////

	struct PipelineDesc final
	{	
		std::map<ShaderType, std::shared_ptr<Shader>> shaders_;
		
		D3D12_BLEND_DESC blend_state_ = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT());
		UINT sample_mask_ = ::DefaultSampleMask();
		
		D3D12_RASTERIZER_DESC rasterizer_state_ = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT());
		D3D12_DEPTH_STENCIL_DESC depth_stencil_state_ = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT());

		std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout_;

		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		
		//TODO: as vector
		//TODO: as struct: count and std::array<DXGI_FORMAT>[8] D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT
		UINT RT_count_ = 0;
		
		DXGI_FORMAT RTV_format[8] = { DXGI_FORMAT_UNKNOWN };
		DXGI_FORMAT DSV_format = DXGI_FORMAT_UNKNOWN;
		
		//TODO: check it
		DXGI_SAMPLE_DESC sample_desc_ = ::DefaultSampleDesc();

		std::string name_;
	};

	////////////////////////////////////////////////////////////////////////////////

	class Buffer;
	class Texture;
	class ContextOld;
	class CommandContextOld;

	class Pipeline final
	{
		ComPtr<ID3D12PipelineState> state_;
		Signature signature_;
	public:
		explicit Pipeline(Processor* processor, const PipelineDesc& desc);

		ID3D12PipelineState* GetPipelineState() const
		{
			return state_.Get();
		}

		ID3D12RootSignature* GetRootSignature() const noexcept
		{
			return signature_.GetRootSignature();
		}

		Signature& GetSignature()
		{
			return signature_;
		}

		//void Bind(const std::shared_ptr<Buffer>& buffer);
		//D3D12_GPU_DESCRIPTOR_HANDLE Bind(Processor*, TextureView&);

		//void EndFrame();

		void CommitShaderResources(Context&);
		void CommitConstantBuffers(ID3D12GraphicsCommandList*, const std::shared_ptr<Buffer>&);
	};
}