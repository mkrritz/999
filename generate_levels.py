import os
import random

MAX_RESOLUTION = 35
OUTPUT_DIR = "Levels"

def generate_level(resolution):
    cell_count = resolution * resolution
    # Generate list from 1 to cell_count
    numbers = list(range(1, cell_count + 1))
    
    # Shuffle the numbers
    random.shuffle(numbers)
    
    # Format into a string with spaces and newlines
    lines = []
    for row in range(resolution):
        start_idx = row * resolution
        end_idx = start_idx + resolution
        line_numbers = numbers[start_idx:end_idx]
        lines.append(" ".join(map(str, line_numbers)))
    
    return "\n".join(lines)

def main():
    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)
        
    for res in range(1, MAX_RESOLUTION + 1):
        level_str = generate_level(res)
        filename = os.path.join(OUTPUT_DIR, f"{res}.txt")
        with open(filename, "w", encoding="utf-8") as f:
            f.write(level_str)
        print(f"Generated {filename}")

if __name__ == "__main__":
    main()
