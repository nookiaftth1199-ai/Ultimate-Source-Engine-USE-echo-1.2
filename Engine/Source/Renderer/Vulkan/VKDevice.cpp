// ============================================================
// Ultimate Source Engine - Vulkan Device Implementation
// ============================================================

#include "stdafx.h"
#include "VKDevice.h"
#include "Core/Window.h"
#include "Core/Logger.h"
#include <set>
#include <algorithm>

namespace USE {

    VKDevice::VKDevice()
        : m_instance(VK_NULL_HANDLE)
        , m_physicalDevice(VK_NULL_HANDLE)
        , m_device(VK_NULL_HANDLE)
        , m_graphicsQueue(VK_NULL_HANDLE)
        , m_presentQueue(VK_NULL_HANDLE)
        , m_surface(VK_NULL_HANDLE)
        , m_swapchain(VK_NULL_HANDLE)
        , m_renderPass(VK_NULL_HANDLE)
        , m_pipelineLayout(VK_NULL_HANDLE)
        , m_pipeline(VK_NULL_HANDLE)
        , m_commandPool(VK_NULL_HANDLE)
        , m_imageAvailableSemaphore(VK_NULL_HANDLE)
        , m_renderFinishedSemaphore(VK_NULL_HANDLE)
        , m_inFlightFence(VK_NULL_HANDLE)
        , m_window(nullptr)
        , m_vsync(true)
    {
    }

    VKDevice::~VKDevice()
    {
        Shutdown();
    }

    // -----------------------------------------------------------------
    // Initialization
    // -----------------------------------------------------------------
    bool VKDevice::Initialize(Window* window, bool vsync)
    {
        USE_LOG_INFO("Initializing Vulkan device...");

        m_vsync = vsync;

        // Get SDL window handle (Window is assumed to be SDLWindow with GetSDLWindow())
        m_window = (SDL_Window*)window->GetNativeHandle();
        if (!m_window) {
            USE_LOG_ERROR("VKDevice: Unable to get SDL window");
            return false;
        }

        if (!CreateInstance()) return false;
        if (!CreateSurface(m_window)) return false;
        if (!SelectPhysicalDevice()) return false;
        if (!CreateLogicalDevice()) return false;
        if (!CreateSwapchain()) return false;
        if (!CreateRenderPass()) return false;
        if (!CreatePipeline()) return false;
        if (!CreateFramebuffers()) return false;
        if (!CreateCommandPool()) return false;
        if (!CreateCommandBuffers()) return false;
        if (!CreateSyncObjects()) return false;

        USE_LOG_INFO("Vulkan device initialized successfully.");
        return true;
    }

    void VKDevice::Shutdown()
    {
        if (m_device) {
            vkDeviceWaitIdle(m_device);
        }
        Cleanup();
    }

    // -----------------------------------------------------------------
    // Frame control
    // -----------------------------------------------------------------
    void VKDevice::BeginFrame()
    {
        // Wait for the previous frame to finish
        vkWaitForFences(m_device, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(m_device, 1, &m_inFlightFence);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX,
                               m_imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        // Record command buffer
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(m_commandBuffers[imageIndex], &beginInfo);

        // Begin render pass
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_swapchainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.width = 1280; // FIXME: use actual swapchain extent
        renderPassInfo.renderArea.extent.height = 720;
        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(m_commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VKDevice::EndFrame()
    {
        // End render pass
        vkCmdEndRenderPass(m_commandBuffers[0]); // FIXME: need current image index

        // End command buffer
        vkEndCommandBuffer(m_commandBuffers[0]);

        // Submit command buffer
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_commandBuffers[0];

        VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFence) != VK_SUCCESS) {
            USE_LOG_ERROR("VKDevice: Failed to submit draw command buffer");
        }
    }

    void VKDevice::Present()
    {
        // Present
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_renderFinishedSemaphore;

        VkSwapchainKHR swapChains[] = {m_swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &m_currentImageIndex; // we need to store this

        vkQueuePresentKHR(m_presentQueue, &presentInfo);
    }

    // -----------------------------------------------------------------
    // Clear, viewport, scissor (stubs)
    // -----------------------------------------------------------------
    void VKDevice::Clear(uint32_t flags, const Color& color, float depth, uint32_t stencil)
    {
        // Clear is handled by render pass clear values.
    }

    void VKDevice::SetViewport(int x, int y, int width, int height)
    {
        VkViewport viewport = {};
        viewport.x = (float)x;
        viewport.y = (float)y;
        viewport.width = (float)width;
        viewport.height = (float)height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(m_commandBuffers[0], 0, 1, &viewport);
    }

    void VKDevice::SetScissorRect(int x, int y, int width, int height)
    {
        VkRect2D scissor = {};
        scissor.offset.x = x;
        scissor.offset.y = y;
        scissor.extent.width = (uint32_t)width;
        scissor.extent.height = (uint32_t)height;
        vkCmdSetScissor(m_commandBuffers[0], 0, 1, &scissor);
    }

    void VKDevice::EnableScissor(bool enable)
    {
        // Not directly controllable; scissor is always enabled if set.
    }

    void VKDevice::DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation)
    {
        vkCmdDrawIndexed(m_commandBuffers[0], indexCount, 1, startIndexLocation, baseVertexLocation, 0);
    }

    // -----------------------------------------------------------------
    // Resource creation stubs
    // -----------------------------------------------------------------
    bool VKDevice::CreateVertexBuffer(const void* data, size_t size, uint32_t& bufferHandle)
    {
        USE_LOG_WARN("VKDevice::CreateVertexBuffer not implemented");
        bufferHandle = 0;
        return false;
    }

    bool VKDevice::CreateIndexBuffer(const void* data, size_t size, uint32_t& bufferHandle)
    {
        USE_LOG_WARN("VKDevice::CreateIndexBuffer not implemented");
        bufferHandle = 0;
        return false;
    }

    bool VKDevice::CreateTexture2D(int width, int height, TextureFormat format, const void* data, uint32_t& textureHandle)
    {
        USE_LOG_WARN("VKDevice::CreateTexture2D not implemented");
        textureHandle = 0;
        return false;
    }

    bool VKDevice::CreateShader(ShaderType type, const char* source, uint32_t& shaderHandle)
    {
        USE_LOG_WARN("VKDevice::CreateShader not implemented");
        shaderHandle = 0;
        return false;
    }

    void VKDevice::DestroyBuffer(uint32_t handle)
    {
        USE_LOG_WARN("VKDevice::DestroyBuffer not implemented");
    }

    void VKDevice::DestroyTexture(uint32_t handle)
    {
        USE_LOG_WARN("VKDevice::DestroyTexture not implemented");
    }

    void VKDevice::DestroyShader(uint32_t handle)
    {
        USE_LOG_WARN("VKDevice::DestroyShader not implemented");
    }

    // -----------------------------------------------------------------
    // Private helpers
    // -----------------------------------------------------------------
    bool VKDevice::CreateInstance()
    {
        // Check Vulkan loader
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        if (extensionCount == 0) {
            USE_LOG_ERROR("VKDevice: No Vulkan extensions found");
            return false;
        }

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Ultimate Source Engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Ultimate Source Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // Get required extensions from SDL
        unsigned int sdlExtensionCount = 0;
        SDL_Vulkan_GetInstanceExtensions(m_window, &sdlExtensionCount, nullptr);
        std::vector<const char*> extensions(sdlExtensionCount);
        SDL_Vulkan_GetInstanceExtensions(m_window, &sdlExtensionCount, extensions.data());

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = (uint32_t)extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        // Optional layers (debug)
        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };
        #ifdef _DEBUG
        createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
        #endif

        if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
            USE_LOG_ERROR("VKDevice: Failed to create Vulkan instance");
            return false;
        }

        USE_LOG_INFO("Vulkan instance created.");
        return true;
    }

    bool VKDevice::CreateSurface(SDL_Window* window)
    {
        if (!SDL_Vulkan_CreateSurface(window, m_instance, &m_surface)) {
            USE_LOG_ERROR("VKDevice: Failed to create Vulkan surface");
            return false;
        }
        USE_LOG_INFO("Vulkan surface created.");
        return true;
    }

    bool VKDevice::SelectPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            USE_LOG_ERROR("VKDevice: No Vulkan-capable physical devices found");
            return false;
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

        // Pick first discrete GPU if available, otherwise any
        for (const auto& device : devices) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(device, &props);
            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                m_physicalDevice = device;
                break;
            }
        }
        if (m_physicalDevice == VK_NULL_HANDLE) {
            m_physicalDevice = devices[0];
        }

        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(m_physicalDevice, &props);
        USE_LOG_INFO("Selected Vulkan device: %s", props.deviceName);
        return true;
    }

    bool VKDevice::CreateLogicalDevice()
    {
        uint32_t graphicsFamily = FindQueueFamily(VK_QUEUE_GRAPHICS_BIT, false);
        uint32_t presentFamily  = FindQueueFamily(0, true);

        std::set<uint32_t> uniqueQueueFamilies = {graphicsFamily, presentFamily};
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        float queuePriority = 1.0f;
        for (uint32_t family : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueInfo = {};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = family;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;

        const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        createInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
            USE_LOG_ERROR("VKDevice: Failed to create logical device");
            return false;
        }

        vkGetDeviceQueue(m_device, graphicsFamily, 0, &m_graphicsQueue);
        vkGetDeviceQueue(m_device, presentFamily,  0, &m_presentQueue);

        USE_LOG_INFO("Vulkan logical device created.");
        return true;
    }

    uint32_t VKDevice::FindQueueFamily(VkQueueFlags flags, bool present)
    {
        uint32_t familyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &familyCount, nullptr);
        std::vector<VkQueueFamilyProperties> families(familyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &familyCount, families.data());

        for (uint32_t i = 0; i < familyCount; ++i) {
            if (present) {
                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, i, m_surface, &presentSupport);
                if (presentSupport) return i;
            } else {
                if (families[i].queueFlags & flags) return i;
            }
        }
        return UINT32_MAX;
    }

    bool VKDevice::CreateSwapchain()
    {
        // For simplicity, we assume a basic swapchain with one image.
        // Full implementation would query surface capabilities and choose format/extent.

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_surface;

        // Pick a format
        createInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
        createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        // Extent (use window size)
        int width, height;
        SDL_GetWindowSize(m_window, &width, &height);
        createInfo.imageExtent.width = width;
        createInfo.imageExtent.height = height;

        createInfo.minImageCount = 2; // double buffering
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t graphicsFamily = FindQueueFamily(VK_QUEUE_GRAPHICS_BIT, false);
        uint32_t presentFamily  = FindQueueFamily(0, true);

        if (graphicsFamily != presentFamily) {
            uint32_t queueFamilyIndices[] = {graphicsFamily, presentFamily};
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = m_vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
        createInfo.clipped = VK_TRUE;

        if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS) {
            USE_LOG_ERROR("VKDevice: Failed to create swapchain");
            return false;
        }

        // Get swapchain images
        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
        std::vector<VkImage> images(imageCount);
        vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, images.data());

        // Create image views
        m_swapchainImageViews.resize(imageCount);
        for (size_t i = 0; i < images.size(); ++i) {
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = images[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_device, &viewInfo, nullptr, &m_swapchainImageViews[i]) != VK_SUCCESS) {
                USE_LOG_ERROR("VKDevice: Failed to create image view");
                return false;
            }
        }

        USE_LOG_INFO("Swapchain created with %u images.", imageCount);
        return true;
    }

    bool VKDevice::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
            USE_LOG_ERROR("VKDevice: Failed to create render pass");
            return false;
        }

        return true;
    }

    bool VKDevice::CreatePipeline()
    {
        // Stub: full pipeline creation requires shader modules, vertex input state, etc.
        // For now, we create a minimal pipeline layout.
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            USE_LOG_ERROR("VKDevice: Failed to create pipeline layout");
            return false;
        }

        // A real pipeline would be created here.
        USE_LOG_WARN("VKDevice::CreatePipeline: pipeline not fully implemented");
        return true;
    }

    bool VKDevice::CreateFramebuffers()
    {
        m_swapchainFramebuffers.resize(m_swapchainImageViews.size());

        for (size_t i = 0; i < m_swapchainImageViews.size(); ++i) {
            VkFramebufferCreateInfo fbInfo = {};
            fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbInfo.renderPass = m_renderPass;
            fbInfo.attachmentCount = 1;
            fbInfo.pAttachments = &m_swapchainImageViews[i];
            fbInfo.width = 1280; // FIXME: use swapchain extent
            fbInfo.height = 720;
            fbInfo.layers = 1;

            if (vkCreateFramebuffer(m_device, &fbInfo, nullptr, &m_swapchainFramebuffers[i]) != VK_SUCCESS) {
                USE_LOG_ERROR("VKDevice: Failed to create framebuffer");
                return false;
            }
        }
        return true;
    }

    bool VKDevice::CreateCommandPool()
    {
        uint32_t graphicsFamily = FindQueueFamily(VK_QUEUE_GRAPHICS_BIT, false);
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = graphicsFamily;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
            USE_LOG_ERROR("VKDevice: Failed to create command pool");
            return false;
        }
        return true;
    }

    bool VKDevice::CreateCommandBuffers()
    {
        m_commandBuffers.resize(m_swapchainFramebuffers.size());

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

        if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
            USE_LOG_ERROR("VKDevice: Failed to allocate command buffers");
            return false;
        }
        return true;
    }

    bool VKDevice::CreateSyncObjects()
    {
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFence) != VK_SUCCESS) {
            USE_LOG_ERROR("VKDevice: Failed to create synchronization objects");
            return false;
        }
        return true;
    }

    void VKDevice::DestroySwapchain()
    {
        for (auto fb : m_swapchainFramebuffers) vkDestroyFramebuffer(m_device, fb, nullptr);
        for (auto iv : m_swapchainImageViews) vkDestroyImageView(m_device, iv, nullptr);
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    }

    void VKDevice::Cleanup()
    {
        if (m_device) {
            vkDestroySemaphore(m_device, m_renderFinishedSemaphore, nullptr);
            vkDestroySemaphore(m_device, m_imageAvailableSemaphore, nullptr);
            vkDestroyFence(m_device, m_inFlightFence, nullptr);

            if (m_commandPool) {
                vkDestroyCommandPool(m_device, m_commandPool, nullptr);
            }

            DestroySwapchain();

            if (m_pipelineLayout) vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
            if (m_renderPass) vkDestroyRenderPass(m_device, m_renderPass, nullptr);

            vkDestroyDevice(m_device, nullptr);
        }

        if (m_surface) vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        if (m_instance) vkDestroyInstance(m_instance, nullptr);
    }

} // namespace USE