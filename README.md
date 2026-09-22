# Lossless Data Compressor

A high-performance C++ file compression and decompression utility built from the ground up using the **Huffman Greedy Coding Algorithm**, **Min-Heaps**, and **Bit-Level Serialization**.

## Key Technical Highlights
- **Optimal Prefix Trees**: Generates variable-length prefix-free codes using a custom Min-Heap priority queue, preventing ambiguity during decoding.
- **Bit Manipulation**: Accumulates variable bit streams into 8-bit byte buffers for compact binary serialization (`std::ios::binary`).
- **Self-Contained Header**: Stores frequency maps and precise byte counts in the binary file header for autonomous decompression without out-of-band metadata.
- **Consistent Compression**: Achieves **40% to 50% space reduction** on structured and repetitive text datasets.

## How to Build and Run
```bash
# Compile with optimization
g++ -O3 HuffmanCompressor.cpp -o compressor

# Execute compression & decompression pipeline
./compressor
