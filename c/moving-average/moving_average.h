
/*
 * MIT License
 * 
 * Copyright (c) 2020 Filipe Chagas
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file moving_average.h
 * @author Filipe Chagas
 * @date 28/02/2020 (last revision on 28/02/2020)
 * @brief Implementation of a moving average filter 
 *
 * @see https://github.com/FilipeChagasDev/signals
 */

#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>

/**
 * @brief Typedef of the Moving Average Filter (MAF) instance struct.
 * Each instance of the MAF filter must have its own maf_t struct.
 */
typedef struct maf
{
    /* -- Instance mode --*/
    uint8_t instance_mode;
    #define MAF_INSTANCE_NONE 0
    #define MAF_INSTANCE_STATIC 1
    #define MAF_INSTANCE_DYNAMIC 2

    /* -- Buffers -- */
    uint8_t current_buffer;
    #define MAF_BUFFER_0 0
    #define MAF_BUFFER_1 1

    float *buffer0;
    float *buffer1;
    size_t buffer_length;

    /* -- Kernel function callback -- */
    float (*kern_function)(struct maf *instance, size_t index);

    /* -- Kernel function params vector -- */
    float *kern_params;
    size_t kern_params_length;
}maf_t;

/**
 * @brief Typedef of the kernel-function callback
 * A kernel-function provides a convolution kernel to the filtering.
 * @param instance Pointer to the maf_t struct of the filter instance.
 * @param index Index of the sample in the filter buffer.
 * @return Weight to the sample in the filter buffer.
 */
typedef float (*maf_kern_func_t)(maf_t *instance, size_t index);


/**
 * @brief Initialize a MAF instance with a static maf_t.
 * The kernel-function will be defined as maf_simple_average_kern. Buffer will be filled with zero.
 * @param instance Pointer to the maf_t static struct of the filter instance.
 * @param length Buffer length
 * @return 1 for success, 0 for error.
 */
uint8_t init_maf(maf_t *instance, size_t length);

/**
 * @brief Dynamically creates a MAF instance.
 * The maf_t struct and its internal data will be dynamically allocated.
 * @param size_t length
 * @return Pointer to the maf_t struct. NULL for error.
 */
maf_t *create_maf(size_t length);

/**
 * @brief Fill the buffer with a value.
 * @param instance Pointer to the maf_t struct of the filter instance.
 * @param value Value to fill the buffer with.
 * @return 1 for success, 0 for error.
 */
uint8_t maf_fill(maf_t *instance, float value);

/**
 * @brief Apply the filtering to the input sample and return the filtered sample.
 * @param instance Pointer to the maf_t struct of the filter instance.
 * @param input Input sample.
 * @return Filtered sample. 0 in error cases.
 */
float maf_filter(maf_t *instance, float input);

/**
 * @brief Deletes the MAF instance (static or dynamic).
 * @param instance Pointer to the maf_t struct of the filter instance.
 * @return 1 for success, 0 for error.
 */
uint8_t delete_maf(maf_t *instance);

/**
 * @brief Kernel-function of the simple average.
 * Always return (1/buffer_length).
 * @param instance Pointer to the maf_t struct of the filter instance.
 * @param index Index of the sample in the filter buffer.
 * @return Weight to the sample in the filter buffer.
 */
float maf_simple_average_kern(maf_t *instance, size_t index);

/**
 * @brief Sets the MAF instance to work with simple average.
 * Define kernel-function as maf_simple_average_kern and kern_params as NULL.
 * @param instance Pointer to the maf_t struct of the filter instance.
 * @return 1 for success, 0 for error.
 */
uint8_t maf_set_simple_average(maf_t *instance);


#ifdef __cplusplus
}
#endif

#endif //MOVING_AVERAGE_H