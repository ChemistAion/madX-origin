#pragma once

namespace madX::Core
{
	constexpr std::size_t cAsync_TaskModelCapacity = 256 - sizeof(void*);

	class Task final
	{
		std::aligned_storage_t<cAsync_TaskModelCapacity> model_ = {};

		////////////////////////////////////////////////////////////////////////////////

		struct Concept
		{
			bool (*ready_)(void* self) noexcept;
			bool (*valid_)(void* self) noexcept;
			void (*move_)(void* self, void* destination) noexcept;

			void (*invoke_)(void* self);
			void* (*data_)(void* self) noexcept;

			void (*dtor_)(void* self);
		};

		////////////////////////////////////////////////////////////////////////////////

		template <class...>
		struct Model;

		template <class Result, class... Archetypes, class... Futures>
		struct Model<Result(Archetypes...), Futures...>
		{
			std::packaged_task<Result(Archetypes...)> trampoline_;
			std::tuple<DecayReferenceWrapperType<Futures>...> arguments_;

			template <class Callable, class... Arguments>
			explicit Model(Callable&& callable, Arguments&&... arguments) noexcept
			  : trampoline_(std::forward<Callable>(callable))
			  , arguments_(std::forward<Arguments>(arguments)...)
			{
			}

			Model(const Model&) = delete;
			Model& operator=(const Model&) = delete;

			Model(Model&& other) noexcept = default;
			Model& operator=(Model&& other) noexcept = default;

			decltype(auto) GetFuture()
			{
				return Future<Result>(trampoline_.get_future().share());
			}

			////////////////////////////////////////////////////////////////////////////////

			static bool ready_(void* self) noexcept
			{
				const auto model = static_cast<Model*>(self);
				return model->ReadyImpl(std::index_sequence_for<Futures...>());
			}

			static bool valid_(void* self) noexcept
			{
				const auto model = static_cast<Model*>(self);
				return model->trampoline_.valid();
			}

			static void move_(void* self, void* destination) noexcept
			{
				auto model = static_cast<Model*>(self);
				new (destination) Model(std::move(*model));

				//NOTE: arguments_ are left
				return;
			}

			static void invoke_(void* self)
			{
				auto model = static_cast<Model*>(self);

				assert(model->trampoline_.valid());

				model->InvokeImpl(std::index_sequence_for<Futures...>());
				model->trampoline_ = {};
				//TODO: arguments_??? optimization
			}

			static void* data_(void* self) noexcept
			{
				return self;
			}

			static void dtor_(void* self)
			{
				static_cast<Model*>(self)->~Model();
			}

			static constexpr Concept vtable_ = { ready_, valid_, move_, invoke_, data_, dtor_ };

		private:
			template <typename Type>
			inline decltype(auto) CallGet(Type&& type)
			{
				if constexpr (IsFuture<Type>)
				{
					return type.Get();
				}
				else
				{
					return std::forward<Type>(type);
				}
			}

			template <std::size_t... Index>
			inline void InvokeImpl(std::index_sequence<Index...>)
			{
				std::invoke(trampoline_, CallGet(std::get<Index>(std::move(arguments_)))...);
			}

			////////////////////////////////////////////////////////////////////////////////

			template <typename Type>
			inline bool CallReady(const Type& type) const
			{
				if constexpr (IsFuture<Type>)
				{
					return type.IsReady();
				}
				else
				{
					(void)type;
					return true;
				}
			}

			inline bool CheckIfReady() const
			{
				return true;
			}

			template <typename... Arguments>
			inline bool CheckIfReady(bool first, Arguments... arguments) const
			{
				return first & CheckIfReady(arguments...);
			}

			template <std::size_t... Index>
			inline bool ReadyImpl(std::index_sequence<Index...>) const
			{
				return CheckIfReady(CallReady(std::get<Index>(arguments_))...);
			}
		};

		////////////////////////////////////////////////////////////////////////////////

		static bool ready_(void*) noexcept
		{
			return true;
		}

		static bool valid_(void*) noexcept
		{
			return true;
		}

		static void move_(void*, void*) noexcept
		{
			return;
		}

		static void invoke_(void*)
		{
			//throw std::bad_function_call();
			return;
		}

		static void* data_(void*) noexcept
		{
			return nullptr;
		}

		static void dtor_(void*)
		{
			return;
		}

		static constexpr Concept vtable_ = { ready_, valid_, move_, invoke_, data_, dtor_ };
		const Concept* vtable_ptr_ = &vtable_;

		////////////////////////////////////////////////////////////////////////////////

		template <class Callable, class... Arguments>
		Task(Callable&& callable, Arguments&&... arguments)
		{
			using result = std::invoke_result_t<Callable, DecayIfFuture<Arguments>...>;
			using model = Model<result(DecayIfFuture<Arguments>...), Arguments...>;

			const auto size = sizeof(model);
			static_assert(size <= cAsync_TaskModelCapacity);

			new (&model_) model(std::forward<Callable>(callable), std::forward<Arguments>(arguments)...);
			vtable_ptr_ = &model::vtable_;
		}

		inline void* Data() noexcept
		{
			return vtable_ptr_->data_(&model_);
		}

	public:
		Task() noexcept = default;

		~Task()
		{
			vtable_ptr_->dtor_(&model_);
		}

		////////////////////////////////////////////////////////////////////////////////

		Task(const Task&) = delete;
		Task& operator=(const Task&) = delete;

		Task(Task&& other) noexcept
		{
			vtable_ptr_ = std::move(other.vtable_ptr_);
			vtable_ptr_->move_(&other.model_, &model_);
		}

		Task& operator=(Task&& other) noexcept
		{
			vtable_ptr_->dtor_(&model_);

			vtable_ptr_ = std::move(other.vtable_ptr_);
			vtable_ptr_->move_(&other.model_, &model_);

			return *this;
		}

		////////////////////////////////////////////////////////////////////////////////

		template <class Callable, class... Arguments>
		static decltype(auto) CreateTask(Callable&& callable, Arguments&&... arguments)
		{
			using result = std::invoke_result_t<Callable, DecayIfFuture<Arguments>...>;
			using model = Model<result(DecayIfFuture<Arguments>...), Arguments...>;

			Task task(std::forward<Callable>(callable), std::forward<Arguments>(arguments)...);
			auto future = static_cast<model*>(task.Data())->GetFuture();

			return std::pair<Task, Future<result>>(std::move(task), std::move(future));
		}

		////////////////////////////////////////////////////////////////////////////////

		inline bool IsReady() noexcept
		{
			return vtable_ptr_->ready_(&model_);
		}

		inline bool IsValid() noexcept
		{
			return vtable_ptr_->valid_(&model_);
		}

		inline void Invoke()
		{
			vtable_ptr_->invoke_(&model_);
		}
	};
}