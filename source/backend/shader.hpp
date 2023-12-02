import <cassert>;

import stl.core;
import stl.filesystem;
import microsoft.windows;
import microsoft.directx;
import madX.Core.Logger;
import madX.Backend.Exception;

//#include "d3d12shader.h"
//#include "dxcapi.h"
//#include "dxc\support\dxcapi.use.h"

using namespace Microsoft::WRL;

import <winerror.h>;
import <combaseapi.h>;

namespace madX::Backend
{
	class Processor;

	////////////////////////////////////////////////////////////////////////////////

	enum class ShaderType
	{
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		Amplification,
		Mesh,
		Compute,
		Unknown
	};

	enum class ShaderInputType
	{
		CBV,
		BufferSRV,
		BufferUAV,
		TextureSRV,
		TextureUAV,
		Sampler,
		Unknown,
	};

	constexpr ShaderInputType GetShaderInputType(D3D_SHADER_INPUT_TYPE type, D3D_SRV_DIMENSION dimension) noexcept
	{
		switch (type)
		{
		case D3D_SIT_CBUFFER:
			return ShaderInputType::CBV;
		case D3D_SIT_TEXTURE:
			assert(dimension != D3D_SRV_DIMENSION_UNKNOWN);
			return dimension == D3D_SRV_DIMENSION_BUFFER
				? ShaderInputType::BufferSRV
				: ShaderInputType::TextureSRV;
		case D3D_SIT_SAMPLER:
			return ShaderInputType::Sampler;
		case D3D_SIT_UAV_RWTYPED:
			assert(dimension != D3D_SRV_DIMENSION_UNKNOWN);
			return dimension == D3D_SRV_DIMENSION_BUFFER
				? ShaderInputType::BufferUAV
				: ShaderInputType::TextureUAV;
		case D3D_SIT_STRUCTURED:
		case D3D_SIT_BYTEADDRESS:
			return ShaderInputType::BufferSRV;
		case D3D_SIT_UAV_RWSTRUCTURED:
		case D3D_SIT_UAV_RWBYTEADDRESS:
			return ShaderInputType::BufferUAV;
		case D3D_SIT_TBUFFER:
		case D3D_SIT_UAV_APPEND_STRUCTURED:
		case D3D_SIT_UAV_CONSUME_STRUCTURED:
		case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
		default:
			assert(false);
		}

		return ShaderInputType::Unknown;
	}

	////////////////////////////////////////////////////////////////////////////////

	struct ShaderInputAttributes
	{
		const ShaderInputType type_ = ShaderInputType::Unknown;
		const UINT space_ = 0;
		const UINT point_ = 0;
		const UINT count_ = 0;
	public:
		constexpr ShaderInputAttributes() noexcept = default;
		constexpr explicit ShaderInputAttributes(const D3D12_SHADER_INPUT_BIND_DESC& desc) noexcept
			: type_{ GetShaderInputType(desc.Type, desc.Dimension) }
			, space_{ desc.Space }
			, point_{ desc.BindPoint }
			, count_{ desc.BindCount }
		{
		}

		////////////////////////////////////////////////////////////////////////////////

		friend bool operator<(const ShaderInputAttributes& lhs, const ShaderInputAttributes& rhs) noexcept
		{
			if ((int)(lhs.type_) < (int)(rhs.type_))
			{
				return true;
			}

			auto type = lhs.type_ == rhs.type_;

			if (type && lhs.space_ < rhs.space_)
			{
				return true;
			}

			auto space = lhs.space_ == rhs.space_;

			if (type && space && lhs.point_ < rhs.point_)
			{
				return true;
			}

			return false;
		}
	};

	////////////////////////////////////////////////////////////////////////////////

	struct ShaderDesc final
	{
		ShaderType type_ = ShaderType::Unknown;

		std::wstring entry_;

		std::vector<std::wstring> defines_;

		std::filesystem::path file_;
		std::string name_;
	};

	////////////////////////////////////////////////////////////////////////////////

	class Shader final
	{
	private:
		std::string name_;
		ComPtr<IDxcBlob> dxc_object_;

		ComPtr<IDxcBlob> dxc_reflection_;
		ComPtr<ID3D12ShaderReflection> reflection_;

		ComPtr<ID3D12VersionedRootSignatureDeserializer> deserializer_;
		const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* root_signature_ = nullptr;
	public:
		Shader(Processor*, const ShaderDesc&);

//		ShaderType GetShaderType() const noexcept
//		{
//			return desc_.type_;
//		}

		IDxcBlob* GetShaderObject() noexcept
		{
			return dxc_object_.Get();
		}

		ID3D12ShaderReflection* GetReflection() noexcept
		{
			return reflection_.Get();
		}

		const D3D12_ROOT_SIGNATURE_DESC1& GetSignature() noexcept
		{
			return root_signature_->Desc_1_1;
		}

		//	const D3D12_ROOT_SIGNATURE_DESC& GetSignature() noexcept
		//	{
		//		return root_signature_->Desc_1_0;		
		//	}

		bool HasSignature() const noexcept
		{
			return root_signature_ != nullptr;
		}
	};
}