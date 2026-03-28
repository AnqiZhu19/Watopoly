#!/bin/bash
# find_seed.sh — find the smallest seed that produces a specific output string
# Usage: bash find_seed.sh "target string" InputFile.in
# Example: bash find_seed.sh "moves forward" SLC_Forward.in
TARGET="$1"
INPUT="$2"
BINARY="../watopoly"

if [ -z "$TARGET" ] || [ -z "$INPUT" ]; then
    echo "Usage: bash find_seed.sh <target_string> <input_file>"
    exit 1
fi

for SEED in $(seq 0 20000); do
    OUTPUT=$($BINARY -testing -seed $SEED < "$INPUT" 2>&1)
    if echo "$OUTPUT" | grep -q "$TARGET"; then
        echo "$SEED"
        echo "Found seed $SEED produces: $TARGET"
        exit 0
    fi
done
echo "No seed found in range 0..20000 for: $TARGET"
exit 1
