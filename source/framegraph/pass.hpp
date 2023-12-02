import stl.core;

namespace madX::Framegraph
{
	enum class PassType
	{
		Undefined,
        Graphics,
        Compute,
        Async,
        Copy,
        Resolve
	};

    enum class PassFlags
    {
        None = 0,
        Test = 1 << 0
    };

    ////////////////////////////////////////////////////////////////////////////////

    struct PassAsyncContext
    {
    };

    struct PassExecuteContext
    {
    };

    class PassBase// : public Core::GraphNode
    {
    private:
    //  void Begin();
    //  void End();
    protected:
        const std::string name_;
        const PassType type_ = PassType::Undefined;
        const PassFlags flags_ = PassFlags::None;
    public:
        PassBase() = default;

        explicit PassBase(const std::string& name, PassType type, PassFlags flags)//, Core::Graph& graph)
            : name_(name)
            , type_(type)
            , flags_(flags)
            //, GraphNode(graph)
        {
        }

//      void ResolveBarriers(const Core::Graph&);
//      void ResolveAsync(const Core::Graph&, PassAsyncContext&);
//      void Execute(const Core::Graph&, PassExecuteContext&);

        void BeginEvent(const std::string& name)
        {
        }

        void EndEvent()
        {
        }
    };

    ////////////////////////////////////////////////////////////////////////////////

    template <class PassData>
    class Pass : public PassBase
    {
    private:
        PassData data_;
    public:
        using PassSetup = std::function<void(PassData&)>;
        using PassExecute = std::function<void(const PassData&)>;

        explicit Pass(const std::string& name, PassType type, PassFlags flags, PassSetup&& setup, PassExecute&& execute)
            : PassBase(name, type, flags)
            , setup_(std::move(setup))
            , execute_(std::move(execute))
        {
        }

        const PassData& GetData() const
        {
            return data_;
        }
    private:
        PassSetup setup_;
        PassExecute execute_;
    };

    template <>
    class Pass<void> : public PassBase
    {
    public:
        using PassSetup = std::function<void()>;
        using PassExecute = std::function<void()>;

        explicit Pass(const std::string& name, PassType type, PassFlags flags, PassSetup&& setup, PassExecute&& execute)
            : PassBase(name, type, flags)
            , setup_(std::move(setup))
            , execute_(std::move(execute))
        {
        }

        void GetData() const
        {
            return;
        }
    private:
        PassSetup setup_;
        PassExecute execute_;
    };
}


//   public:
//       RenderGraphPass(char const* name, SetupFunc&& setup, ExecuteFunc&& execute, ERGPassType type = ERGPassType::Graphics, ERGPassFlags flags = ERGPassFlags::None)
//           : RenderGraphPassBase(name, type, flags), setup(std::move(setup)), execute(std::move(execute))
//       {}
//  
//       PassData const& GetPassData() const
//       {
//           return data;
//       }
//  
//   private:
//       PassData data;
//       SetupFunc setup;
//       ExecuteFunc execute;