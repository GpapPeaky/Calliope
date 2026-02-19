#!/bin/bash

# File to generate
FILENAME=".tests/huge_file.c"

# Number of lines to generate (start smaller for testing)
LINES=500000  # Adjust to 500000000 carefully if you really want 500M

echo "#include <stdio.h>" > "$FILENAME"
echo "" >> "$FILENAME"
echo "int main() {" >> "$FILENAME"

# Generate lines
for ((i=1; i<=LINES; i++)); do
    echo "    int var$i = $i;" >> "$FILENAME"

    # Print progress every 100k lines
    if (( i % 100000 == 0 )); then
        echo "Generated $i lines..."
    fi
done

echo "    return 0;" >> "$FILENAME"
echo "}" >> "$FILENAME"

echo "Finished generating $LINES lines in $FILENAME"
