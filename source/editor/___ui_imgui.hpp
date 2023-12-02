import stl.core;
import microsoft.windows;
import microsoft.directx;
import microsoft.xmath;
import madX.Backend;
import madX.Frontend;

import gui.imgui;

import madX.Platform.Window;

namespace madX::Editor
{
	struct ImGuiBackendData
	{
		std::shared_ptr<madX::Backend::Processor> processor_;
		std::shared_ptr<madX::Backend::Pipeline> pipe_imgui_;
		std::shared_ptr<madX::Backend::Pipeline> pipe_raw_;
		std::unique_ptr<madX::Backend::Context> context_;
		std::shared_ptr<madX::Backend::SwapChain> swapchain_;

		std::shared_ptr<madX::Backend::Texture> fonts_;
		madX::Backend::TextureView fonts_srv_;
	};

	struct ImGuiBackendViewportData
	{
		std::unique_ptr<madX::Backend::Commands> commands_;
		std::unique_ptr<madX::Backend::Context> context_;
		std::shared_ptr<madX::Backend::SwapChain> swapchain_;

		madX::Backend::Frame frame_ = 0;
	};

	ImGuiBackendData* GetImGuiBackendData()
	{
		return ImGui::GetCurrentContext() ? (ImGuiBackendData*)ImGui::GetIO().BackendRendererUserData : nullptr;
	}

	////////////////////////////////////////////////////////////////////////////////

	void ImGuiRenderData(ImDrawData* draw_data, ID3D12GraphicsCommandList* command_list)
	{
		assert(draw_data != nullptr);

		if (draw_data->DisplaySize.x < 1.0f || draw_data->DisplaySize.y < 1.0f)
		{
			return;
		}

		ImGuiBackendData* fd = GetImGuiBackendData();
		auto context = fd->context_.get();

		////////////////////////////////////////////////////////////////////////////////

		context->SetCommandList(command_list);
		auto renderer = fd->processor_.get();

		madX::Backend::BufferDesc vb_desc;
		vb_desc.name_ = "imgui_VB";
		vb_desc.count_ = draw_data->TotalVtxCount;
		vb_desc.stride_ = sizeof(ImDrawVert);
		vb_desc.usage_ = madX::Backend::ResourceUsage::Dynamic;
		vb_desc.bind_ = madX::Backend::ResourceBind::VertexBuffer;
		auto vb = renderer->CreateBuffer(vb_desc);

		madX::Backend::BufferDesc ib_desc;
		ib_desc.name_ = "imgui_IB";
		ib_desc.count_ = draw_data->TotalIdxCount;
		ib_desc.stride_ = sizeof(ImDrawIdx);
		ib_desc.usage_ = madX::Backend::ResourceUsage::Dynamic;
		ib_desc.bind_ = madX::Backend::ResourceBind::IndexBuffer;
		auto ib = renderer->CreateBuffer(ib_desc);

		////////////////////////////////////////////////////////////////////////////////

		{
			ImDrawVert* vtx_dst = (ImDrawVert*)vb->GetAddressCPU();
			ImDrawIdx* idx_dst = (ImDrawIdx*)ib->GetAddressCPU();

			for (int list_index = 0; list_index < draw_data->CmdListsCount; ++list_index)
			{
				const auto cmd_list = draw_data->CmdLists[list_index];

				assert(cmd_list->VtxBuffer.Data != nullptr);
				assert(cmd_list->VtxBuffer.Size > 0);

				std::memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));

				assert(cmd_list->IdxBuffer.Data != nullptr);
				assert(cmd_list->IdxBuffer.Size > 0);

				std::memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

				vtx_dst += cmd_list->VtxBuffer.Size;
				idx_dst += cmd_list->IdxBuffer.Size;
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		madX::Backend::BufferDesc buffer_desc;
		buffer_desc.name_ = "Constants";
		buffer_desc.count_ = 1;
		buffer_desc.stride_ = sizeof(DirectX::XMFLOAT4X4);
		buffer_desc.usage_ = madX::Backend::ResourceUsage::Dynamic;
		buffer_desc.bind_ = madX::Backend::ResourceBind::UniformBuffer;

		auto vs_constants = renderer->CreateBuffer(buffer_desc);

		{
			DirectX::XMFLOAT4X4* cb_constants = (DirectX::XMFLOAT4X4*)vs_constants->GetAddressCPU();

			float L = draw_data->DisplayPos.x;
			float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
			float T = draw_data->DisplayPos.y;
			float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

			*cb_constants = DirectX::XMFLOAT4X4
			(
				2.0f / (R - L), 0.0f, 0.0f, 0.0f,
				0.0f, 2.0f / (T - B), 0.0f, 0.0f,
				0.0f, 0.0f, 0.5f, 0.0f,
				(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f
			);
		}

		////////////////////////////////////////////////////////////////////////////////

		//	const float blend_factors[4] = { 0.f, 0.f, 0.f, 0.f };
		//	context->SetBlendFactors(blend_factors);

		std::vector<D3D12_VIEWPORT> viewports;
		viewports.emplace_back(CD3DX12_VIEWPORT(0.0f, 0.0f, (float)draw_data->DisplaySize.x, (float)draw_data->DisplaySize.y));
		context->SetViewports(viewports);

		context->SetPipeline(fd->pipe_imgui_);
		context->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto test = std::make_pair<std::shared_ptr<madX::Backend::Buffer>, std::size_t>(std::move(vb), (std::size_t)0);
		std::vector<madX::Backend::VertexStream> test2 = { test };
		context->SetVertexBuffer(0, test2);
		context->SetIndexBuffer(ib, 0u);

		context->CommitShaderResources();

		auto root_signature = fd->pipe_imgui_->GetRootSignature();
		command_list->SetGraphicsRootSignature(root_signature);

		context->CommitConstantBuffers(vs_constants);

		////////////////////////////////////////////////////////////////////////////////

		int vtx_offset = 0;
		int idx_offset = 0;

		ImVec2 clip = draw_data->DisplayPos;

		auto fonts = ImGui::GetIO().Fonts->TexID;

		for (int list_index = 0; list_index < draw_data->CmdListsCount; ++list_index)
		{
			const auto cmd_list = draw_data->CmdLists[list_index];

			for (int cmd_index = 0; cmd_index < cmd_list->CmdBuffer.Size; ++cmd_index)
			{
				const auto cmd = &cmd_list->CmdBuffer[cmd_index];

				if (cmd->UserCallback != nullptr)
				{
					//TODO: something useful...
					assert(false);
				}
				else
				{
					const ImVec2 clip_min(cmd->ClipRect.x - clip.x, cmd->ClipRect.y - clip.y);
					const ImVec2 clip_max(cmd->ClipRect.z - clip.x, cmd->ClipRect.w - clip.y);

					if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
					{
						continue;
					}

					if (auto texture = cmd->GetTexID(); texture)
					{
						D3D12_GPU_DESCRIPTOR_HANDLE texture_handle = { (UINT64)texture };
						command_list->SetGraphicsRootDescriptorTable(1, texture_handle);

						//TODO: context!
						if (fonts == texture)
						{
							command_list->SetPipelineState(fd->pipe_imgui_->GetPipelineState());
						}
						else
						{
							command_list->SetPipelineState(fd->pipe_raw_->GetPipelineState());
						}
					}

					const D3D12_RECT scisors = { (LONG)clip_min.x, (LONG)clip_min.y, (LONG)clip_max.x, (LONG)clip_max.y };
					context->SetScissorRects(1, &scisors);

					madX::Backend::DrawIndexedAttributes draw;
					draw.indices_ = cmd->ElemCount;
					draw.first_index_index_ = cmd->IdxOffset + idx_offset;
					draw.base_vertex_ = cmd->VtxOffset + vtx_offset;
					context->DrawIndexed(draw);
				}
			}

			vtx_offset += cmd_list->VtxBuffer.Size;
			idx_offset += cmd_list->IdxBuffer.Size;
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	void ImGuiCreateWindow(ImGuiViewport* viewport)
	{
		ImGuiBackendData* fd = GetImGuiBackendData();
		ImGuiBackendViewportData* vd = IM_NEW(ImGuiBackendViewportData)();
		viewport->RendererUserData = vd;

		assert(viewport->PlatformHandleRaw);
		//HWND hwnd = viewport->PlatformHandleRaw ? (HWND)viewport->PlatformHandleRaw : (HWND)viewport->PlatformHandle;
		HWND hwnd = (HWND)viewport->PlatformHandleRaw;

		////////////////////////////////////////////////////////////////////////////////

		auto renderer = fd->processor_.get();

		vd->commands_ = std::make_unique<madX::Backend::Commands>
			(
				renderer,
				madX::Backend::cEngine_BackBufferCount,
				madX::Backend::cEngine_CommandListPerFrame,
				renderer->GetGraphicsQueue()->GetDesc()
				);

		vd->context_ = std::make_unique<madX::Backend::Context>(renderer);
		//auto command_list = vd->commands_->GetNewCommandList();
		//vd->context_->SetCommandList(command_list);

		madX::Backend::SwapChainDesc swapchain_desc;
		swapchain_desc.name_ = "SwapChain::Viewport" + std::to_string(viewport->ID);
		swapchain_desc.width_ = (UINT)viewport->Size.x;
		swapchain_desc.height_ = (UINT)viewport->Size.y;
		//swapchain_desc.format_ = 
		swapchain_desc.window_ = hwnd;
		vd->swapchain_ = fd->processor_->CreateSwapChain(swapchain_desc);

		return;
	};

	void ImGuiDestroyWindow(ImGuiViewport* viewport)
	{
		if (ImGuiBackendViewportData* vd = (ImGuiBackendViewportData*)viewport->RendererUserData)
		{
			assert(vd != NULL);
			IM_DELETE(vd);
		}

		viewport->RendererUserData = NULL;
	}

	void ImGuiSetWindowSize(ImGuiViewport* viewport, ImVec2 size)
	{
		ImGuiBackendData* fd = GetImGuiBackendData();
		ImGuiBackendViewportData* vd = (ImGuiBackendViewportData*)viewport->RendererUserData;

		//TODO: separate command queue
		fd->processor_->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		fd->processor_->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);
		vd->swapchain_->Resize((UINT)size.x, (UINT)size.y);
	}

	void ImGuiRenderWindow(ImGuiViewport* viewport, void*)
	{
		ImGuiBackendData* fd = GetImGuiBackendData();
		ImGuiBackendViewportData* vd = (ImGuiBackendViewportData*)viewport->RendererUserData;

		auto commands = vd->commands_.get();
		commands->BeginFrame(vd->frame_);

		auto command_list = commands->GetNewCommandList();
		vd->context_->SetCommandList(command_list);

		auto renderer = fd->processor_.get();
		auto swapchain = vd->swapchain_.get();

		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				swapchain->GetBackBufferResource(),
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			);

			command_list->ResourceBarrier(1, &bb_barier);
		}

		////////////////////////////////////////////////////////////////////////////////

		{
			auto result = command_list->Close();
			madX::Backend::ThrowIfFailed(result, "[PlatformIO_RenderWindow at swapchain::wait] ID3D12GraphicsCommandList::Close failed");

			ID3D12CommandList* const lists[] = { command_list };
			renderer->GetGraphicsQueue()->ExecuteCommandLists(1, lists);
		}

		swapchain->Wait(vd->frame_);

		////////////////////////////////////////////////////////////////////////////////

		command_list = commands->GetNewCommandList();
		vd->context_->SetCommandList(command_list);

		command_list->OMSetRenderTargets(1, swapchain->GetBackBufferHandle(), TRUE, NULL);

		float clear_color[] = { 0.3f, 0.3f, 0.3f, 1.0f }; //TODO: not seen
		command_list->ClearRenderTargetView(*swapchain->GetBackBufferHandle(), clear_color, 0, NULL);

		ImGuiRenderData(viewport->DrawData, command_list);
	}

	void ImGuiSwapBuffers(ImGuiViewport* viewport, void*)
	{
		ImGuiBackendData* fd = GetImGuiBackendData();
		ImGuiBackendViewportData* vd = (ImGuiBackendViewportData*)viewport->RendererUserData;

		auto renderer = fd->processor_.get();
		auto swapchain = vd->swapchain_.get();

		auto command_list = vd->context_->GetCommandList();

		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				swapchain->GetBackBufferResource(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT
			);

			command_list->ResourceBarrier(1, &bb_barier);
		}

		{
			auto result = command_list->Close();
			madX::Backend::ThrowIfFailed(result, "[PlatformIO_RenderWindow at swapchain::present] ID3D12GraphicsCommandList::Close failed");

			ID3D12CommandList* const lists[] = { command_list };
			renderer->GetGraphicsQueue()->ExecuteCommandLists(1, lists);
		}

		////////////////////////////////////////////////////////////////////////////////

		swapchain->Present(vd->frame_++);
	}
}