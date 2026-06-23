#ifndef Puzzle_HPP
#define Puzzle_HPP

struct CharacterQuad;
struct Puzzle {
    uint32_t* cells; // CPU Side
    
    uint32_t quads_count; // The total number of quads in all cells.
    
    GLuint cells_handle;
    CharacterQuad* cells_buffer;
    
    uint32_t number_of_digits_in_current_level;
    GLuint level_handle;
    CharacterQuad* level_buffer;
};

constexpr uint32_t MAX_LEVEL = 40;

constexpr uint32_t count_digits_up_to_square(uint32_t input_value) {
    const uint32_t upper_limit = input_value * input_value;
    uint32_t total_digit_count = 0;
    uint32_t digits_per_number = 1;
    uint32_t range_start = 1;
    while (range_start <= upper_limit) {
        const uint32_t range_end = (range_start * 10 - 1 < upper_limit)
            ? (range_start * 10 - 1)
            : upper_limit;
        const uint32_t numbers_in_range = range_end - range_start + 1;
        total_digit_count += numbers_in_range * digits_per_number;
        range_start *= 10;
        ++digits_per_number;
    }
    return total_digit_count;
}

constexpr uint32_t count_decimal_digits(uint32_t value) {
    if (value == 0) {
        return 1;
    }
    uint32_t digit_count = 0;
    while (value > 0) {
        value /= 10;
        ++digit_count;
    }
    return digit_count;
}

#endif
