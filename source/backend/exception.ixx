export module madX.Backend.Exception;

import stl.core;
import microsoft.windows;
import madX.Core.Logger;

import <winerror.h>;

export namespace madX::Backend
{
	inline void ThrowIfFailed(HRESULT result, std::string message)
	{
		if (FAILED(result))
		{
			//_com_error error(result);
		
			auto logger = Core::Logger::GetInstance();
			
			//std::wstring error_message(error.ErrorMessage());
			//std::string outout = message + " | " + std::string(error_message.cbegin(), error_message.cend());
			
			//logger->Error("Throw " + outout);
			logger->Error("[Throw] " + message);
			
			throw std::runtime_error(message);
		}
	}
}