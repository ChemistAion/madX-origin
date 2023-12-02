#pragma once

namespace madX::Meta::Nodes
{
	struct PropertyDesc
	{
		const Meta::String name_;
		Meta::Vector<ParameterDesc> parameters_;
	};

	struct Property
	{
		const char* name_ = nullptr;
		Meta::Vector<Parameter> parameters_;
	};

	using PropertyDescMap = Meta::Map<Meta::String, PropertyDesc>;
	
	using Properties = Meta::Vector<Property>;
}
