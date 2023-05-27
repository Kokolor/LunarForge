all: setup

setup:
	gcc src/setup.c -o setup

clean:
	rm setup