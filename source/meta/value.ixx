export module madX.Meta.Value;

import utils.cista;

export namespace madX::Meta
{
	namespace Detail
	{
		namespace CISTA = cista::raw;
		constexpr auto const MODE = cista::mode::NONE;
	}

	////////////////////////////////////////////////////////////////////////////////

	struct Array;
	struct Object;
	struct State;

	template <class Type>
	using Ptr = Detail::CISTA::ptr<Type>;

	template <class Type>
	using UniquePtr = Detail::CISTA::unique_ptr<Type>;

	template <class Type>
	auto MakeUniquePtr = Detail::CISTA::make_unique<Type>;

	using Null = std::monostate;
	using String = Detail::CISTA::string;
	
	template <typename T1, typename T2>
	using Pair = Detail::CISTA::pair<T1, T2>;

	//TODO: check if unique_ptr is necessary
	using MetaArray = Detail::CISTA::unique_ptr<Array>;
	using MetaObject = Detail::CISTA::unique_ptr<Object>;
	using MetaState = Detail::CISTA::unique_ptr<State>;

	using Value = Detail::CISTA::variant
		<
		Null,
		String,
		std::int64_t,
		std::uint64_t,
		double,
		bool,
		MetaArray,
		MetaObject
		>;
	
	template <class Type>
	using Vector = Detail::CISTA::vector<Type>;

	template <class Key, class Value>
	using Map = Detail::CISTA::hash_map<Key, Value>;

	template <class Type>
	using Index = Detail::CISTA::indexed_vector<Type>;

	////////////////////////////////////////////////////////////////////////////////

	struct Array : public Vector<Value>
	{
	};

	struct Object : public Map<String, Value>
	{
	};

	struct State
	{
		const String name_;

		State()
			: name_ { "NULL" }
		{
		}

		State(const char* name)
			: name_(name)
		{
		}
	};

	////////////////////////////////////////////////////////////////////////////////

	//	template <typename Context>
	//	inline void Serialize(Context& context, const Array* origin, const Offset offset)
	//	{
	//		cista::serialize(context, static_cast<const Vector*>(origin), offset);
	//	}
	//	
	//	template <typename Context>
	//	inline void Deserialize(const Context& context, Array* element)
	//	{
	//		cista::deserialize(context, static_cast<Vector*>(element));
	//	}
	//	
	//	template <typename Context>
	//	inline void Serialize(Context& context, const Object* origin, const Offset offset)
	//	{
	//		cista::serialize(context, static_cast<const Map*>(origin), offset);
	//	}
	//	
	//	template <typename Context>
	//	inline void Deserialize(const Context& context, Object* element)
	//	{
	//		cista::deserialize(context, static_cast<Map*>(element));
	//	}
}