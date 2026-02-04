#!/bin/bash
# generate_c_file.sh
# Usage: ./generate_c_file.sh <num_lines> <output_file>

NUM_LINES=$1
OUTPUT_FILE=$2

if [[ -z "$NUM_LINES" || -z "$OUTPUT_FILE" ]]; then
    echo "Usage: $0 <num_lines> <output_file>"
    exit 1
fi

echo "// Auto-generated C file with $NUM_LINES lines" > "$OUTPUT_FILE"

for i in $(seq 0 $((NUM_LINES - 1))); do
    echo "int var_$i = $i;" >> "$OUTPUT_FILE"
    echo "void func_$i() {" >> "$OUTPUT_FILE"
    echo "    int x = var_$i * 2;" >> "$OUTPUT_FILE"
    echo "    printf(\"Line $i: %d\\n\", x);" >> "$OUTPUT_FILE"
    echo "}" >> "$OUTPUT_FILE"
    echo "" >> "$OUTPUT_FILE"
done

echo "Generated $OUTPUT_FILE with $NUM_LINES lines."
