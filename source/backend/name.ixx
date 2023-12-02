export module madX.Backend.Name;

import stl.core;
import microsoft.directx;
import madX.Backend.Exception;

namespace madX::Backend
{
	static void SetObjectName(ID3D12Object* object, const std::string& name)
	{
		std::wstring w_name(name.cbegin(), name.cend());
	
		auto result = object->SetName(w_name.c_str());
		ThrowIfFailed(result, "ID3D12Object::SetName failed, to: " + name);
	}
}