#include "_meta.hpp"

#include "accessor.hpp"

namespace madX::Meta::glTF
{
	Accessors GetAccessors(const simdjson::dom::element& element)
	{
		Accessors accessors;

		const auto& items = JSON::Get<simdjson::dom::array>(element);
		accessors.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = JSON::Get<simdjson::dom::object>(item);
			auto& accessor = accessors.emplace_back();
	
			for (const auto [key, value] : object)
			{
				if (key == "bufferView")
				{
					accessor.view_ = JSON::Get<Index>(value);
					continue;
				}
			
				if (key == "byteOffset")
				{
					accessor.offset_ = JSON::Get<Offset>(value);
					continue;
				}
			
				if (key == "componentType")
				{
					//TODO: redundant pair<Type, sizeof(Type)>
					auto component = JSON::Get<Type>(value);
			
					switch (component)
					{
					case 5120:
					case 5121:
					{
						accessor.component_ = 1;
						break;
					}
					case 5122:
					case 5123:
					{
						accessor.component_ = 2;
						break;
					}
					case 5124:
					case 5125:
					case 5126:
					{
						accessor.component_ = 4;
						break;
					}
					default:
					{
						assert(false);
						accessor.component_ = 0;
						break;
					}
					}
			
					continue;
				}

				if (key == "normalized")
				{
					//TODO: check it...
					assert(false);
					continue;
				}

				if (key == "count")
				{
					accessor.count_ = JSON::Get<Count>(value);
					continue;
				}

				//TODO: redundant pair<Type, sizeof(Type)>
				if (key == "type")
				{
					auto type = JSON::Get<std::string_view>(value);
				
					if (type == "SCALAR")
					{
						accessor.element_ = 1; //{ Accessor::ElementType::Scalar, 1 };
						continue;
					}
				
					if (type == "VEC2")
					{
						accessor.element_ = 2; //{ Accessor::ElementType::Vec2, 2 };
						continue;
					}
				
					if (type == "VEC3")
					{
						accessor.element_ = 3; //{ Accessor::ElementType::Vec3, 3 };
						continue;
					}
				
					if (type == "VEC4")
					{
						accessor.element_ = 4; //{ Accessor::ElementType::Vec4, 4 };
						continue;
					}
				
					if (type == "MAT2")
					{
						accessor.element_ = 4; //{ Accessor::ElementType::Mat2, 2 * 2 };
						continue;
					}
				
					if (type == "MAT3")
					{
						accessor.element_ = 9; //{ Accessor::ElementType::Mat3, 3 * 3 };
						continue;
					}
				
					if (type == "MAT4")
					{
						accessor.element_ = 16; //{ Accessor::ElementType::Mat4, 4 * 4 };
						continue;
					}
	
					assert(false);
					accessor.element_ = 0;
					continue;
				}
	
				if (key == "min")
				{
					accessor.min_ = JSON::GetVector<Float>(value);
					continue;
				}
			
				if (key == "max")
				{
					accessor.max_ = JSON::GetVector<Float>(value);
					continue;
				}
	
				if (key == "sparse")
				{
					//TODO: check it...
					assert(false);
					continue;
				}
	
				////////////////////////////////////////////////////////////////////////////////
	
				if (key == "name")
				{
					accessor.name_ = JSON::Get<std::string_view>(value);
					continue;
				}
	
				if (key == "extensions")
				{
					continue;
				}
	
				if (key == "extras")
				{
					accessor.extras_ = std::move(GetExtras(value));
					continue;
				}
	
				assert(false);
			}
		}

		//TODO: checks...
		//min/max size equal?

		return accessors;
	}
}

//	if (component == 5120)
//	{
//		accessor.component_ = { Accessor::ComponentType::Byte, 1 };
//		continue;
//	}
//	if (component == 5121)
//	{
//		//accessor.component_ = { Accessor::ComponentType::UnsignedByte, 1 };
//		continue;
//	}
//	if (component == 5122)
//	{
//		accessor.component_ = { Accessor::ComponentType::Short, 2 };
//		continue;
//	}
//	if (component == 5123)
//	{
//		accessor.component_ = { Accessor::ComponentType::UnsignedShort, 2 };
//		continue;
//	}
//	//	if (component == 5124)
//	//	{
//	//		accessor.component_type_ = { Accessor::ComponentType::Integer, 4 };
//	//		continue;
//	//	}
//	if (component == 5125)
//	{
//		accessor.component_ = { Accessor::ComponentType::UnsignedInteger, 4 };
//		continue;
//	}
//	if (component == 5126)
//	{
//		accessor.component_ = { Accessor::ComponentType::Float, 4 };
//		continue;
//	}