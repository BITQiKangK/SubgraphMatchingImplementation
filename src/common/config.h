#pragma once

/**
 * Set the maximum size of a query graph. By default, we set the value as 64.
 */
#define MAXIMUM_QUERY_GRAPH_SIZE 64

/**
 * Setting the value as 1 is to enable the neighbor label frequency filter (i.e., NLF filter)
 * The cost is to build an unordered_map for each vertex to store the frequency of the labels of its neighbors
 * If the memory can hold the extra memory cost, then enable this feature to boost the performance. Otherwise, disable
 * it by setting this value as 0.
 */
#define OPTIMIZED_LABELED_GRAPH 1

/**
 * Set intersection method.
 * 0: Hybrid method; 1: Merge based set intersections.
 */
#define HYBRID 0

/**
 * Accelerate set intersection with SIMD instructions.
 * 0: AVX2; 1: AVX512; 2: Basic;
 */
#define SI 0

#define DEBUGMODE

#define PRINT_SEPARATOR "------------------------------"