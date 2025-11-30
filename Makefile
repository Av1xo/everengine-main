run:
	g++ src/*.cpp src/Engine/*.cpp src/Engine/Camera/*.cpp lib/glad.c -I include -lglfw -lGL -lX11 -lpthread -lXrandr -lassimp -ldl -o build/engine
	./build/engine