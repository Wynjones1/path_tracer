BUILD_DIR := ./build
all:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ..
	$(MAKE) -C $(BUILD_DIR)

run: all
	$(BUILD_DIR)/src/path_tracer

clean:
	rm -Rf $(BUILD_DIR)
