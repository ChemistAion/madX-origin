namespace madX
{
	class Frontend
	{
	public:
		Frontend()
		{
		};

		virtual ~Frontend()
		{
		}

		virtual void Create() = 0;
		virtual void Destroy() = 0;

		virtual void Update() = 0;
		virtual void Begin() = 0;
		virtual void Process() = 0;
		virtual void End() = 0;


	//	void OnActivated() {};
	//	void OnDeactivated() {};
	//	void OnSuspending() {};
	//	void OnResuming() {};
	//	void OnWindowSizeChanged(std::uint32_t width, std::uint32_t height);
	};
}