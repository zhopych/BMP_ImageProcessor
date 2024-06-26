#include "matrix_filter.h"
#include <cmath>

const double INT_8_MAX = 255.0;

MatrixFilter::MatrixFilter(int** input) {
    matrix = new int*[3];
    for (size_t i = 0; i < 3; ++i) {
        matrix[i] = new int[3];
    }
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            matrix[i][j] = input[i][j];
        }
    }
}

MatrixFilter::~MatrixFilter() {
    for (size_t i = 0; i < 3; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

void MatrixFilter::Process(Image& image) {
    std::vector<std::vector<FloatPixel>> pixel_floats(image.Height(), std::vector<FloatPixel>(image.Width()));
    for (uint32_t x = 0; x < image.dib_header.height; ++x) {
        for (uint32_t y = 0; y < image.dib_header.width; ++y) {
            pixel_floats[x][y].red = static_cast<double>(image.pixels[x][y].red) / INT_8_MAX;
            pixel_floats[x][y].green = static_cast<double>(image.pixels[x][y].green) / INT_8_MAX;
            pixel_floats[x][y].blue = static_cast<double>(image.pixels[x][y].blue) / INT_8_MAX;
        }
    }
    for (int32_t x = 0; x < static_cast<int32_t>(image.dib_header.height); ++x) {
        for (int32_t y = 0; y < static_cast<int32_t>(image.dib_header.width); ++y) {
            double sum_r = 0;
            double sum_g = 0;
            double sum_b = 0;
            for (int32_t add_x = -1; add_x <= 1; ++add_x) {
                for (int32_t add_y = -1; add_y <= 1; ++add_y) {
                    int32_t corrected_x = std::min(static_cast<int32_t>(image.dib_header.height - 1),
                                                   std::max(x + add_x, static_cast<int32_t>(0)));
                    int32_t corrected_y = std::min(static_cast<int32_t>(image.dib_header.width - 1),
                                                   std::max(y + add_y, static_cast<int32_t>(0)));
                    double red = pixel_floats[corrected_x][corrected_y].red;
                    double green = pixel_floats[corrected_x][corrected_y].green;
                    double blue = pixel_floats[corrected_x][corrected_y].blue;
                    sum_r += red * matrix[add_y + 1][add_x + 1];
                    sum_g += green * matrix[add_y + 1][add_x + 1];
                    sum_b += blue * matrix[add_y + 1][add_x + 1];
                }
            }
            double corrected_red = std::min(1.0, std::max(0.0, sum_r));
            double corrected_green = std::min(1.0, std::max(0.0, sum_g));
            double corrected_blue = std::min(1.0, std::max(0.0, sum_b));
            image.pixels[x][y].red = static_cast<uint8_t>(round(corrected_red * INT_8_MAX));
            image.pixels[x][y].green = static_cast<uint8_t>(round(corrected_green * INT_8_MAX));
            image.pixels[x][y].blue = static_cast<uint8_t>(round(corrected_blue * INT_8_MAX));
        }
    }
}