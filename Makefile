CFLAGS = -std=c++17 -O2 -DNDEBUG
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

DBGCFLAGS = -std=c++17 -O0 -g -DNDEBUG

VulkanTest: main.cpp
	g++ $(CFLAGS) -o VulkanTest main.cpp $(LDFLAGS)

DevVulkanTest: development-setup-test.cpp
	g++ $(CFLAGS) -o DevVulkanTest development-setup-test.cpp $(LDFLAGS)

.PHONY: test clean debug testDev

testDev: DevVulkanTest
	./DevVulkanTest

test: VulkanTest
	./VulkanTest

clean:
	rm -f VulkanTest DevVulkanTest

debug:
	g++ $(DBGCFLAGS) -o VulkanTest main.cpp $(LDFLAGS)

