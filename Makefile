clean:
	-rm -rf build

build: main.c
	-mkdir -p build
	gcc main.c -o build/main -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Lraylib/lib -Iraylib/include

run:
	./build/main
