BUILD_DIR=build
OUTPUT_DIR=output
SRC_DIR=src

CC=g++ --std=c++11 -Werror -O3 -I$(SRC_DIR)

BROTMAP_FILES = brotmap.cc worker.cc evaluate.cc
MAKE_PPM_FILES = make_ppm.cc
INCLUDE_FILES = brotmap.h

BROTMAP_SRC = $(addprefix $(SRC_DIR)/,$(BROTMAP_FILES))
MAKE_PPM_SRC = $(addprefix $(SRC_DIR)/,$(MAKE_PPM_FILES))
INCLUDE_SRC = $(addprefix $(SRC_DIR)/,$(INCLUDE_FILES))

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


$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/make_ppm: $(MAKE_PPM_SRC) $(INCLUDE_SRC)
	$(CC) $(MAKE_PPM_SRC) -o $(BUILD_DIR)/make_ppm

$(BUILD_DIR)/brotmap: $(BROTMAP_SRC) $(INCLUDE_SRC)
	$(CC) $(BROTMAP_SRC) -lc -lpthread -o $(BUILD_DIR)/brotmap

clean:
	rm -f $(BUILD_DIR)/*

superclean: clean
	rm -f $(OUTPUT_DIR)/*
