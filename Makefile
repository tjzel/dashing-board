all:
	@$(MAKE) -C build

clean:
	@$(MAKE) -C build clean

test:
	@$(MAKE) -C build 
	(cd build && ctest)
