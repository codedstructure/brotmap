BUILD_DIR=build
OUTPUT_DIR=output
SRC_DIR=src

$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(OUTPUT_DIR))

CC=g++ --std=c++11 -Werror -O3 -I$(SRC_DIR)

BROTMAP_FILES = brotmap.cc worker.cc evaluate.cc
BROTMAP_HEADERS = brotmap.h
MAKE_PPM_FILES = make_ppm.cc colourmap.cc
MAKE_PPM_HEADERS = brotmap.h colourmap.h

BROTMAP_SRC = $(addprefix $(SRC_DIR)/,$(BROTMAP_FILES))
MAKE_PPM_SRC = $(addprefix $(SRC_DIR)/,$(MAKE_PPM_FILES))
BROTMAP_INCLUDE = $(addprefix $(SRC_DIR)/,$(BROTMAP_HEADERS))
MAKE_PPM_INCLUDE = $(addprefix $(SRC_DIR)/,$(MAKE_PPM_HEADERS))

all: $(BUILD_DIR)/make_ppm $(BUILD_DIR)/brotmap

run: $(OUTPUT_DIR)/out_image

$(OUTPUT_DIR):
	@mkdir -p $(OUTPUT_DIR)

$(OUTPUT_DIR)/out_image: $(OUTPUT_DIR)/out.ppm $(OUTPUT_DIR)
	@echo "Converting to a nicer image format..."
	@which pnmtopng > /dev/null && pnmtopng $(OUTPUT_DIR)/out.ppm > $(OUTPUT_DIR)/out_image || ppm2tiff $(OUTPUT_DIR)/out.ppm $(OUTPUT_DIR)/out_image
	@echo "  done."
	@echo "check $(OUTPUT_DIR)/out_image for a Mandelbrot image file"

$(OUTPUT_DIR)/out.ppm: $(BUILD_DIR)/make_ppm $(OUTPUT_DIR)/mandel.dat
	@echo "Creating PPM image from source data"
	@$(BUILD_DIR)/make_ppm $(OUTPUT_DIR)/mandel.dat $(OUTPUT_DIR)/out.ppm

$(OUTPUT_DIR)/mandel.dat: $(BUILD_DIR)/brotmap
	@echo "Running brotmap to create a 1024x1024 test image..."
	@time $(BUILD_DIR)/brotmap $(OUTPUT_DIR)/mandel.dat 10

test: $(OUTPUT_DIR)/mandel.dat
	@echo "Running brotmap to update test image..."
	@time $(BUILD_DIR)/brotmap $(OUTPUT_DIR)/mandel.dat 10

show: $(OUTPUT_DIR)/out_image
	@touch $(OUTPUT_DIR)/out_image
	@which xdg-open > /dev/null && xdg-open $(OUTPUT_DIR)/out_image || open $(OUTPUT_DIR)/out_image

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/make_ppm: $(MAKE_PPM_SRC) $(MAKE_PPM_INCLUDE)
	$(CC) $(MAKE_PPM_SRC) -o $(BUILD_DIR)/make_ppm

$(BUILD_DIR)/brotmap: $(BROTMAP_SRC) $(BROTMAP_INCLUDE)
	$(CC) $(BROTMAP_SRC) -lc -lpthread -o $(BUILD_DIR)/brotmap

clean:
	rm -f $(BUILD_DIR)/*

imageclean:
	rm -f $(OUTPUT_DIR)/{*.ppm,out_image}

superclean: clean
	rm -f $(OUTPUT_DIR)/*
