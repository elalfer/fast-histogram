#!/usr/bin/env bash
set -euo pipefail

# Create and enter the `bench` directory
mkdir -p bench
cd bench

# Download the archive (only if it doesn't exist yet)
URL="https://sun.aei.polsl.pl/~sdeor/corpus/silesia.zip"
ZIP="silesia.zip"
if [[ ! -f "$ZIP" ]]; then
  echo "Downloading $URL ..."
  curl -L -o "$ZIP" "$URL"

  # Extract the archive
  echo "Extracting $ZIP ..."
  unzip -o "$ZIP"
fi


# Prepare the output CSV
OUTPUT="bench_results.csv"
echo "file_name,size,implementation,time" > "$OUTPUT"

# Path to the bench binary (assumes it lives in the project root under `bin/bench`)
BENCH_BIN="../bin/bench"

# Run the benchmark on every file in the extracted archive
for file in *; do
  [[ -f "$file" ]] || continue   # skip non‑regular files
  # Skip any file whose name ends with .csv
  [[ "$file" == *.csv ]] && continue
  
  size=$(stat -c%s "$file")

  echo "Benchmarking $file ..."
  # Run the benchmark, capture both stdout and stderr
  BENCH_OUT=$({ BENCHMARK_FILE="$file" $BENCH_BIN --benchmark_min_time=10s 2>&1; } || true)

  # Extract the benchmark lines (exclude header and other informational lines)
  benchmark_lines=$(echo "$BENCH_OUT" | awk '/^------/ {count++; next} count > 1')
  
  # Process each benchmark line
  while read -r line; do
    impl=$(echo "$line" | awk '{print $1}')
    time_val=$(echo "$line" | awk '{print $2}')
    time_unit=$(echo "$line" | awk '{print $3}')
    time="${time_val}${time_unit}"
  
    # Append to CSV
    echo "$file,$size,$impl,$time" >> "$OUTPUT"
  done <<< "$benchmark_lines"
  
done

echo "Benchmark results written to $OUTPUT"
