#!/bin/bash
# run_tests.sh — runs all .in tests against expected .out files
# Usage: cd tests && bash run_tests.sh
PASS=0; FAIL=0; SKIP=0
BINARY="../watopoly"

for IN in *.in; do
    OUT="${IN%.in}.out"
    SEED_FILE="${IN%.in}.seed"
    if [ ! -f "$OUT" ]; then
        echo "SKIP (no .out): ${IN%.in}"
        ((SKIP++))
        continue
    fi
    # Default seed 0 for reproducibility; override with a companion .seed file
    SEED_ARG="-seed 0"
    if [ -f "$SEED_FILE" ]; then
        SEED_ARG="-seed $(cat $SEED_FILE)"
    fi
    LOAD_ARG=""
    if [ -f "${IN%.in}.loadfile" ]; then
        LOAD_ARG="-load $(cat ${IN%.in}.loadfile)"
    fi
    ACTUAL=$($BINARY -testing $SEED_ARG $LOAD_ARG < "$IN" 2>&1)
    EXPECTED=$(cat "$OUT")
    if [ "$ACTUAL" = "$EXPECTED" ]; then
        echo "PASS: ${IN%.in}"
        ((PASS++))
    else
        echo "FAIL: ${IN%.in}"
        diff <(echo "$EXPECTED") <(echo "$ACTUAL") | head -30
        ((FAIL++))
    fi
done

echo ""
echo "--- $PASS passed | $FAIL failed | $SKIP skipped ---"
