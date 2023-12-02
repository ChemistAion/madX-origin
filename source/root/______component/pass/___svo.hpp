#pragma once

namespace madX::Root::Component
{
	class SparseVoxelOctree : public Pass
	{
		std::shared_ptr<madX::Engine::Texture> v_normal_;
		std::shared_ptr<madX::Engine::Texture> v_rho_;

		madX::Engine::TextureView v_normal_srv_;
		madX::Engine::TextureView v_rho_srv_;

		madX::Engine::TextureView v_normal_uav_;
		madX::Engine::TextureView v_rho_uav_;
		
		madX::Engine::TextureView v_normal_rtv_;
		madX::Engine::TextureView v_rho_rtv_;

		std::shared_ptr<madX::Engine::Pipeline> pipeline_voxelize_;
		std::shared_ptr<madX::Engine::Pipeline> pipeline_inject_;

		struct Constants
		{
			XMFLOAT4X4 world_;
			XMFLOAT4 bb_min_;
			XMFLOAT4 bb_max_;
 		};
		
		Constants constants_;

		struct GI_param
		{
			float gi_scale_;
			float glossiness_;
			UINT num_vpls_;
			BOOL debug_;
		};

		GI_param gi_param_;

		DirectX::XMFLOAT4X4 world_;
		DirectX::XMFLOAT3 svo_min_;
		DirectX::XMFLOAT3 svo_max_;

		const UINT volume_size_ = 256;

		UINT last_bound_;

		INT inject_rsm_rho_start_slot_ = -1;

	public:
		SparseVoxelOctree() : Pass(false) {}

		void Create(const std::shared_ptr<madX::Engine::Renderer>&, std::shared_ptr<Root::Component::Project>&) override;

		void Destroy() override
		{
			Pass::Destroy();

			v_normal_.reset();
			v_rho_.reset();
		}
				
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