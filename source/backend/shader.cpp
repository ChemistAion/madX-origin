#include "shader.hpp"

//https://simoncoenen.com/blog/programming/graphics/DxcCompiling
namespace madX::Backend
{
	Shader::Shader(Processor* processor, const ShaderDesc& desc)
		: name_ { desc.name_}
	{
		assert(!desc.entry_.empty());
		assert(!desc.file_.empty());

		////////////////////////////////////////////////////////////////////////////////
		
		std::wstring profile;
		
		switch (desc.type_) //TODO: make static pairs desc.type_ and profile
		{
		case ShaderType::Vertex:
			profile = L"vs_6_6";
			break;
		case ShaderType::Pixel:
			profile = L"ps_6_6";
			break;
		case ShaderType::Geometry:
			profile = L"gs_6_6";
			break;
		case ShaderType::Compute:
			profile = L"cs_6_6";
			break;
		case ShaderType::Hull:
			profile = L"hs_6_6";
			break;
		case ShaderType::Domain:
			profile = L"ds_6_6";
			break;
		default:
			assert(false);
			break;
		}
		
		std::string code;

		{
			std::ifstream stream(desc.file_.c_str());
			
			if (!stream.is_open())
			{
				ThrowIfFailed(E_FAIL, "[Shader::Shader] File not found: " + desc.file_.string());
			}
	
			code = std::move(std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()));
		}

		////////////////////////////////////////////////////////////////////////////////

		std::vector<LPCWSTR> arguments;

		arguments.push_back(L"-E");
		arguments.push_back(desc.entry_.c_str());

		arguments.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);

		arguments.push_back(L"-T");
		arguments.push_back(profile.c_str());

		arguments.push_back(DXC_ARG_DEBUG);
		arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);
		arguments.push_back(DXC_ARG_PACK_MATRIX_COLUMN_MAJOR);

		for (const std::wstring& define : desc.defines_)
		{
			arguments.push_back(L"-D");
			arguments.push_back(define.c_str());
		}

		//arguments.push_back(L"-Fd");
		//arguments.push_back(L" \\");

		////////////////////////////////////////////////////////////////////////////////

		HRESULT hr;

		ComPtr<IDxcUtils> utils;
		hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.ReleaseAndGetAddressOf()));
		auto test = SUCCEEDED(hr);
		//ThrowIfFailed(hr, "Shader ctor: DxcCreateInstance(CLSID_DxcUtils, IDxcUtils) failed");
		
		ComPtr<IDxcIncludeHandler> include_handler;
		hr = utils->CreateDefaultIncludeHandler(include_handler.ReleaseAndGetAddressOf());
		ThrowIfFailed(hr, "Shader ctor: CreateDefaultIncludeHandler failed");

		ComPtr<IDxcCompiler3> compiler;
		hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler.ReleaseAndGetAddressOf()));
		ThrowIfFailed(hr, "Shader ctor: DxcCreateInstance(CLSID_DxcCompiler, IDxcCompiler3) failed");

//		ComPtr<IDxcLibrary> library;
//		hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(library.ReleaseAndGetAddressOf()));
//		ThrowIfFailed(hr, "Shader ctor: DxcCreateInstance(CLSID_DxcLibrary, IDxcLibrary) failed");

		////////////////////////////////////////////////////////////////////////////////

		auto logger = Core::Logger::GetInstance();
		
		DxcBuffer source;
		source.Ptr = code.c_str();
		source.Size = code.size();
		source.Encoding = 0U;
		
		{
			std::string profile_name(profile.cbegin(), profile.cend());
		
			logger->Info("Compiling shader: " + desc.name_);
		}
		
		ComPtr<IDxcResult> result;
		
		try
		{
			hr = compiler->Compile
			(
				&source,
				arguments.data(),
				(UINT32)arguments.size(),
				include_handler.Get(),
				IID_PPV_ARGS(result.ReleaseAndGetAddressOf())
			);
		}
		catch (const _com_error& error)
		{
			std::wstring error_message(error.ErrorMessage());
			logger->Critical("D3D12CreateDevice:" + std::string(error_message.cbegin(), error_message.cend()));
		}
		catch (...)
		{
			logger->Critical("D3D12CreateDevice: Unhandled exception");
		}
		
		
		ThrowIfFailed(hr, "[Shader] IDxcCompiler3::Compile failed");
		ThrowIfFailed(result->GetStatus(&hr), "[Shader] IDxcResult::GetStatus failed");
		
		ComPtr<IDxcBlobUtf8> error;
		hr = result->GetOutput
		(
			DXC_OUT_ERRORS,
			IID_PPV_ARGS(error.ReleaseAndGetAddressOf()),
			nullptr
		);
		
		ThrowIfFailed(hr, "[Shader] IDxcResult::GetOutput(DXC_OUT_ERRORS) failed");
		
		if (error && error->GetStringLength() > 0)
		{
			logger->Error((char*)error->GetBufferPointer());
			assert(false);
		}
		
		if (FALSE == result->HasOutput(DXC_OUT_REFLECTION))
		{
			ThrowIfFailed(E_NOINTERFACE, "[Shader] IDxcResult::HasOutput(DXC_OUT_REFLECTION) failed");
		}
		
		bool has_signature = result->HasOutput(DXC_OUT_ROOT_SIGNATURE);
		//TODO: warning
		//	if (FALSE == result->HasOutput(DXC_OUT_ROOT_SIGNATURE))
		//	{
		//		ThrowIfFailed(E_NOINTERFACE, "[Shader] IDxcResult::HasOutput(DXC_OUT_REFLECTION) failed");
		//	}
		
		////////////////////////////////////////////////////////////////////////////////
		
		hr = result->GetOutput
		(
			DXC_OUT_OBJECT,
			IID_PPV_ARGS(dxc_object_.ReleaseAndGetAddressOf()),
			nullptr
		);
		
		ThrowIfFailed(hr, "[Shader] IDxcResult::GetOutput(DXC_OUT_OBJECT) failed");
		
		////////////////////////////////////////////////////////////////////////////////
			
		hr = result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(dxc_reflection_.ReleaseAndGetAddressOf()), nullptr);
		ThrowIfFailed(hr, "[Shader] IDxcResult::GetOutput(DXC_OUT_REFLECTION) failed");
		
		DxcBuffer dxc_reflection_data = { dxc_reflection_->GetBufferPointer(), dxc_reflection_->GetBufferSize(), 0U };
		
		hr = utils->CreateReflection(&dxc_reflection_data, IID_PPV_ARGS(reflection_.ReleaseAndGetAddressOf()));
		ThrowIfFailed(hr, "Shader ctor: CreateReflection failed");
		
		if (has_signature)
		{		
			hr = D3D12CreateVersionedRootSignatureDeserializer
			(
				dxc_object_->GetBufferPointer(),
				dxc_object_->GetBufferSize(),
				IID_PPV_ARGS(deserializer_.ReleaseAndGetAddressOf())
			);
		
			ThrowIfFailed(hr, "[Shader] D3D12CreateRootSignatureDeserializer failed");
			root_signature_ = deserializer_->GetUnconvertedRootSignatureDesc();
		
			assert(root_signature_ != nullptr);
			assert(root_signature_->Version == D3D_ROOT_SIGNATURE_VERSION_1_1);
		}

		return;
	}
}