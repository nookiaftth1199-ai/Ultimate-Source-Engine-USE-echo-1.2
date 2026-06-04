// ============================================================
// Ultimate Source Engine – Vulkan Render Device Implementation
// ============================================================

#include "stdafx.h"
#include "VKDevice.h"
#include "Core/Logger.h"
#include <SDL_vulkan.h>
#include <set>
#include <cstring>

namespace USE
{
	VKDevice::VKDevice() = default;
	VKDevice::~VKDevice() { Shutdown(); }

	bool VKDevice::Initialize(void* windowHandle, uint32_t width, uint32_t height, bool vsync)
	{
		m_width = width;
		m_height = height;
		SDL_Window* window = static_cast<SDL_Window*>(windowHandle);
		if (!window)
		{
			USE_LOG_ERROR("VKDevice: Invalid window handle.");
			return false;
		}

		if (!CreateInstance()) return false;
		if (!SDL_Vulkan_CreateSurface(window, m_instance, &m_surface))
		{
			USE_LOG_ERROR("VKDevice: Failed to create Vulkan surface: %s", SDL_GetError());
			return false;
		}
		if (!PickPhysicalDevice()) return false;
		if (!CreateLogicalDevice()) return false;
		if (!CreateSwapchain()) return false;

		m_initialized = true;
		USE_LOG_INFO("VKDevice initialized (%u x %u).", width, height);
		return true;
	}

	void VKDevice::Shutdown()
	{
		if (m_device)
		{
			vkDeviceWaitIdle(m_device);
			if (m_inFlightFence) vkDestroyFence(m_device, m_inFlightFence, nullptr);
			if (m_renderFinishedSemaphore) vkDestroySemaphore(m_device, m_renderFinishedSemaphore, nullptr);
			if (m_imageAvailableSemaphore) vkDestroySemaphore(m_device, m_imageAvailableSemaphore, nullptr);
			if (m_commandPool) vkDestroyCommandPool(m_device, m_commandPool, nullptr);
			if (m_swapchain) vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
			vkDestroyDevice(m_device, nullptr);
			vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
			vkDestroyInstance(m_instance, nullptr);
		}
		m_initialized = false;
		USE_LOG_INFO("VKDevice shut down.");
	}

	void VKDevice::ResizeBackBuffer(uint32_t width, uint32_t height)
	{
		m_width = width; m_height = height;
		CreateSwapchain(); // re‑create swapchain
	}

	void VKDevice::BeginFrame()
	{
		vkWaitForFences(m_device, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(m_device, 1, &m_inFlightFence);

		uint32_t imageIndex;
		vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		vkResetCommandBuffer(m_commandBuffer, 0);
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(m_commandBuffer, &beginInfo);

		// Begin render pass (simple clear)
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		// (render pass setup omitted for brevity – real implementation would create a render pass)
		// For now, we skip the actual rendering commands and just present a cleared image.

		vkCmdEndRenderPass(m_commandBuffer); // placeholder
		vkEndCommandBuffer(m_commandBuffer);
	}

	void VKDevice::EndFrame() {}
	void VKDevice::Present()
	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_imageAvailableSemaphore;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_renderFinishedSemaphore;
		vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFence);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_renderFinishedSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_swapchain;
		uint32_t imageIndex = 0;
		presentInfo.pImageIndices = &imageIndex;
		vkQueuePresentKHR(m_graphicsQueue, &presentInfo);
	}

	void VKDevice::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {}
	void VKDevice::SetScissorRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {}
	void VKDevice::SetDepthStencilState(bool depthTest, bool depthWrite) {}
	void VKDevice::SetRasterizerState(bool cullBackFaces, bool wireframe) {}
	void VKDevice::SetBlendState(bool enable) {}
	void VKDevice::Clear(bool color, bool depth, bool stencil, const Vector4& clearColor, float clearDepth, uint8_t clearStencil) {}
	void VKDevice::Draw(PrimitiveType type, uint32_t vertexCount, uint32_t startVertex) {}
	void VKDevice::DrawIndexed(PrimitiveType type, uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) {}

	// ----------------------------------------------------------------------
	// Private Vulkan setup
	// ----------------------------------------------------------------------
	bool VKDevice::CreateInstance()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Ultimate Source Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "USE";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 2, 0);
		appInfo.apiVersion = VK_API_VERSION_1_1;

		uint32_t extensionCount = 0;
		SDL_Vulkan_GetInstanceExtensions(nullptr, &extensionCount, nullptr);
		std::vector<const char*> extensions(extensionCount);
		SDL_Vulkan_GetInstanceExtensions(nullptr, &extensionCount, extensions.data());

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledLayerCount = 0;

		if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
		{
			USE_LOG_ERROR("VKDevice: Failed to create Vulkan instance.");
			return false;
		}
		return true;
	}

	bool VKDevice::PickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
		if (deviceCount == 0)
		{
			USE_LOG_ERROR("VKDevice: No Vulkan‑capable GPU found.");
			return false;
		}
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());
		m_physicalDevice = devices[0]; // take first
		return true;
	}

	bool VKDevice::CreateLogicalDevice()
	{
		float queuePriority = 1.0f;
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = 0; // simplified – should find graphics queue
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures{};
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.pEnabledFeatures = &deviceFeatures;

		if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
		{
			USE_LOG_ERROR("VKDevice: Failed to create logical device.");
			return false;
		}
		vkGetDeviceQueue(m_device, 0, 0, &m_graphicsQueue);
		return true;
	}

	bool VKDevice::CreateSwapchain()
	{
		// Simplified swapchain creation – real implementation would query surface capabilities.
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_surface;
		createInfo.minImageCount = 2;
		createInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
		createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		createInfo.imageExtent = { m_width, m_height };
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
		{
			USE_LOG_ERROR("VKDevice: Failed to create swapchain.");
			return false;
		}
		return true;
	}
}