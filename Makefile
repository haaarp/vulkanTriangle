CFLAGS = -std=c++17 -O2 -DNDEBUG
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

DBGCFLAGS = -std=c++17 -O0 -g -DNDEBUG

VulkanTest: main.cpp
	g++ $(CFLAGS) -o VulkanTest main.cpp $(LDFLAGS)

.PHONY: test clean debug

test: VulkanTest
	./VulkanTest

clean:
	rm -f VulkanTest

debug:
	g++ $(DBGCFLAGS) -o VulkanTest main.cpp $(LDFLAGS)
