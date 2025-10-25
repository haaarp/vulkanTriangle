#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>
#include <set>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkSurfaceKHR surface;

// As Vulkan uses queues, we need to check whether our GPU supports
// specific queues
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class HelloTriangleApplication {
public:
    void run() {
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
    }

private:
    GLFWwindow* window;
    VkInstance instance;
    VkDevice device;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    // Queues get destroyed with devices => no cleanup 
    VkQueue graphicsQueue;

    VkQueue presentQueue;

    void initWindow() { 
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow( WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }
    void initVulkan() {
	createInstance();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
    }

    void mainLoop() {
	while (!glfwWindowShouldClose(window)) {
	    glfwPollEvents();
	}
    }

    void cleanup() {
	// Make sure instance is destroyed before instance!
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	vkDestroyDevice(device, nullptr);

	glfwTerminate();
    }

    void createInstance() {
	if (enableValidationLayers && !checkValidationLayerSupport()) {
	    throw std::runtime_error("validation layers requested, but not available!");
	}


	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Include Validation Layer names in createInfo struct 
	if (enableValidationLayers) {
	    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	    createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
	    createInfo.enabledLayerCount = 0;
	}

	uint32_t glfwExtensionCount = 0;
	uint32_t extensionCount = 0;
	const char** glfwExtensions;

	// Check number of extensions
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Save extensions in vector
	std::vector<VkExtensionProperties> extensions(extensionCount);
	//access extensions from vector
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	std::cout << "available extensions:\n";

	for (const auto& extension : extensions) {
	    std::cout << '\t' << extension.extensionName << '\n';
	}

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
	    throw std::runtime_error("failed to create instance!");
	}
    }

    void createSurface() { 
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
	    throw std::runtime_error("failed to create window surface");
	}
    }

    void pickPhysicalDevice() {
	// Beginning of listing graphic devices  
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
	    throw std::runtime_error("failed to find GPUs with Balkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);	
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	for (const auto& device : devices) {
	    if (isDeviceSuitable(device)) {
		physicalDevice = device;
		break;
	    }
	}

	if (physicalDevice == VK_NULL_HANDLE) {
	    throw std::runtime_error("failed to find a suitable GPU!");
	}
    }
 void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value()
        };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = 0;

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    }

//    void createLogicalDevice() {
//	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
//
//	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
//	std::set<uint32_t> uniqueQueueFamilies = {
//	    indices.graphicsFamily.value(),
//	    indices.presentFamily.value()
//	};
//
//	// Queues ALWAYS NEED priorities between 0f and 1f
//	float queuePriority = 1.0f;
//	for (uint32_t queueFamily : uniqueQueueFamilies) {
//	    VkDeviceQueueCreateInfo queueCreateInfo{};
//	    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//	    queueCreateInfo.queueFamilyIndex = queueFamily;
//	    queueCreateInfo.queueCount = 1;
//	    queueCreateInfo.pQueuePriorities = &queuePriority;
//	    queueCreateInfos.push_back(queueCreateInfo);
//	}
//
//	VkPhysicalDeviceFeatures deviceFeatures{};
//
//	VkDeviceCreateInfo createInfo{};
//
//	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//
//	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
//	createInfo.pQueueCreateInfos = queueCreateInfos.data();
//
//	createInfo.pEnabledFeatures = &deviceFeatures;
//
//	createInfo.enabledExtensionCount = 0;
//
//	// This is not needed anymore by newer implementations. Older
//	// implementations might need this as a distinction between instance and device
//	// specific validation layers 
//	if (enableValidationLayers) {
//	    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
//	    createInfo.ppEnabledLayerNames = validationLayers.data();
//	} else {
//	    createInfo.enabledLayerCount = 0;
//	}
//
//	// Finally create a logical device
//	// DON'T FORGET TO CLEANUP 
//	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
//	    throw std::runtime_error("failed to create logical device!");
//	}
//
//	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
//	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
//    }


    bool isDeviceSuitable(VkPhysicalDevice device) {
	// Here you would specify GPU specifics (see
	// https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Physical_devices_and_queue_families)
	QueueFamilyIndices indices = findQueueFamilies(device);

	return indices.graphicsFamily.has_value();
    }


    bool checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
	    bool layerFound = false;

	    for (const auto& layerProperties : availableLayers) {
		if (strcmp(layerName, layerProperties.layerName) == 0) {
		    layerFound = true;
		    break;
		}
	    }

	    if (!layerFound) {
		return false;
	    }
	}

	return true;
    }

    QueueFamilyIndices findQueueFamilies (VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
	// Logic to find queue indices
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	// Note auto tells the compiler to automatically induce the type of
	// the element
	for (const auto& queueFamily : queueFamilies) {
	    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
		indices.graphicsFamily = i;
		if (indices.isComplete()) {
		    break;
		}
	    }

	    i++;
	}

	VkBool32 presentSupport = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
	 
	if (presentSupport) { 
	    indices.presentFamily = i;
	}

	return indices;
    }
};

int main() {
    HelloTriangleApplication app;

    try {
	app.run();
    } catch (const std::exception& e) {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

