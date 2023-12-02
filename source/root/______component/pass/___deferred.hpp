#pragma once

namespace madX::Root::Component
{
	class Deferred : public Pass
	{
		struct Constants
		{
			XMFLOAT4 perspective_;
			XMFLOAT4 direction_;
			XMFLOAT4X4 inv_vp_;
 		};

		Constants constants_;

	public:
		void Create(const std::shared_ptr<madX::Engine::Renderer>&, std::shared_ptr<Root::Component::Project>&) override;
				
		void Render() override;

		void Update(const Camera& camera) override;

	//	void Begin(ID3D12GraphicsCommandList* command_list) override
	//	{
	//		Pass::Begin(command_list);
	//	
	//		context_->SetPipeline(pipeline_);
	//		context_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//		context_->CommitShaderResources();
	//	}
	};
}