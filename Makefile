CXX = g++ -O0 -Wall -std=c++17 -Iinclude -include include/pch.h
SOURCE_DIR = src/
PCH = include/pch.h
PCH_GCH = include/pch.h.gch

SOURCES = \
	Main.cpp \
	Input.cpp \
	Shader.cpp \
	Camera.cpp \
	Callbacks.cpp \
	Globals.cpp \
	UI.cpp \
	Memory.cpp \
	Window.cpp

ifeq ($(MAKECMDGOALS),static)
	EXEC = a
	BUILD_TYPE = static
	OBJECT_DIR = obj_s/
	LIBRARIES = -Llib -lglfw3 -lglew32s -lopengl32 -lpthread -lgdi32 -luser32 -lkernel32
	STATIC_FLAGS = -static -static-libgcc -static-libstdc++ -DGLEW_STATIC
else
	EXEC = bin/a
	BUILD_TYPE = dynamic
	OBJECT_DIR = obj_d/
	LIBRARIES = -Llib -lglfw3dll -lglew32 -lopengl32 -lpthread -lmingw32 -lgdi32 -luser32 -lkernel32
	STATIC_FLAGS =
endif

OBJECTS = $(SOURCES:%.cpp=$(OBJECT_DIR)%.o)
DEPS = $(OBJECTS:.o=.d)

##############################################################
# Targets

all: $(EXEC)

static: $(EXEC)

$(EXEC): $(PCH_GCH) $(OBJECTS)
	$(info Linking $(BUILD_TYPE)...)
	@$(CXX) $(STATIC_FLAGS) -o $@ $(OBJECTS) $(LIBRARIES)

$(PCH_GCH): $(PCH)
	$(info Generating precompiled header...)
	@$(CXX) -x c++-header -o $@ $<

$(OBJECT_DIR)%.o: $(SOURCE_DIR)%.cpp
	$(info Compiling $<...)
	@$(CXX) -c $(STATIC_FLAGS) $< -o $@ -MMD -MP

-include $(DEPS)
##############################################################
