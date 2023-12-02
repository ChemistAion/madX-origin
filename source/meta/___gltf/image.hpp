#pragma once

namespace madX::Meta::glTF
{
	struct Image
	{
		struct DataURI
		{
			std::string uri_;
			Data data_;
		};

		struct DataBufferView
		{
			std::string mime_type_;
			Index buffer_view_;
		};

		std::variant<DataURI, DataBufferView> data_;

		Name name_;
		Extras extras_;
	};

	using Images = std::vector<Image>;
	Images GetImages(const simdjson::dom::element& element);
}