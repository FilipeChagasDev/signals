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
 * @file moving_average.c
 * @author Filipe Chagas
 * @date 28/02/2020 (last revision on 28/02/2020)
 * @brief Implementation of a moving average filter 
 *
 * @see https://github.com/FilipeChagasDev/signals
 */

#include "moving_average.h"

#define RETURN_ERROR 0
#define RETURN_SUCCESS 1

uint8_t init_maf(maf_t *instance, size_t length)
{
    if(instance == NULL) return RETURN_ERROR;

    /* -- Instance mode --*/
    instance->instance_mode = MAF_INSTANCE_STATIC;

    /* -- Buffers -- */
    instance->buffer_length = length;
    instance->buffer0 = (float*)malloc(sizeof(float)*length);
    
    if(instance->buffer0 == NULL)
    {
        instance->instance_mode = MAF_INSTANCE_NONE;
        return RETURN_ERROR;
    } 

    instance->buffer1 = (float*)malloc(sizeof(float)*length);

    if(instance->buffer1 == NULL)
    {
        instance->instance_mode = MAF_INSTANCE_NONE;
        free(instance->buffer0);
        return RETURN_ERROR;
    }

    instance->current_buffer = MAF_BUFFER_0;

    /* -- Kernel function callback -- */
    instance->kern_function = maf_simple_average_kern;

    /* -- Kernel function params vector -- */
    instance->kern_params = NULL;
    instance->kern_params_length = 0;

    maf_fill(instance, 0); //Fill buffer with 0

    return RETURN_SUCCESS;
}

maf_t *create_maf(size_t length)
{
    maf_t *instance = (maf_t*)malloc(sizeof(maf_t));
    
    if(instance == NULL) return NULL; //ERROR

    /* -- Instance mode --*/
    instance->instance_mode = MAF_INSTANCE_DYNAMIC;

    /* -- Buffers -- */
    instance->buffer_length = length;
    instance->buffer0 = (float*)malloc(sizeof(float)*length);
    
    if(instance->buffer0 == NULL) //ERROR
    {
        free(instance);
        return NULL;
    }

    instance->buffer1 = (float*)malloc(sizeof(float)*length);

    if(instance->buffer1 == NULL) //ERROR
    {
        free(instance->buffer0);
        free(instance);
        return NULL;
    }

    instance->current_buffer = MAF_BUFFER_0;

    /* -- Kernel function callback -- */
    instance->kern_function = maf_simple_average_kern;

    /* -- Kernel function params vector -- */
    instance->kern_params = NULL;
    instance->kern_params_length = 0;

    maf_fill(instance, 0); //Fill buffer with 0

    return instance;
}

uint8_t maf_fill(maf_t *instance, float value)
{
    if(instance == NULL) return RETURN_ERROR;
    if(instance->instance_mode == MAF_INSTANCE_NONE) return RETURN_ERROR;

    float *buffer = instance->current_buffer == MAF_BUFFER_0 ? instance->buffer0 : instance->buffer1;

    for(size_t i = 0; i < instance->buffer_length; i++)
    {
        buffer[i] = value;
    }

    return RETURN_SUCCESS;
}

float maf_filter(maf_t *instance, float input)
{
    if(instance == NULL) return 0;
    if(instance->instance_mode == MAF_INSTANCE_NONE) return 0;

    float *buffer;

    if(instance->current_buffer == MAF_BUFFER_0)
    {
        /* -- Toggle buffers -- */
        instance->current_buffer = MAF_BUFFER_1;
        buffer = instance->buffer1;

        /* -- Moves samples one index forward from buffer0 to buffer1 -- */    
        for(size_t i = 0; i < instance->buffer_length-1; i++)
        {
            instance->buffer1[i+1] = instance->buffer0[i]; 
        }
    }
    else //MAF_BUFFER_1
    {
        /* -- Toggle buffers -- */
        instance->current_buffer = MAF_BUFFER_0;
        buffer = instance->buffer0;

        /* -- Moves samples one index forward from buffer1 to buffer0 -- */    
        for(size_t i = 0; i < instance->buffer_length-1; i++)
        {
            instance->buffer0[i+1] = instance->buffer1[i]; 
        }
    }

    /* -- Average calc -- */
    float sum = 0;
    buffer[0] = input;
    for(size_t i = 0; i < instance->buffer_length; i++)
    {
        sum += buffer[i] * instance->kern_function(instance, i);
    }

    return sum;
}

uint8_t delete_maf(maf_t *instance)
{
    if(instance == NULL) return RETURN_ERROR;
    if(instance->instance_mode == MAF_INSTANCE_NONE) return RETURN_ERROR;

    instance->instance_mode = MAF_INSTANCE_NONE;
    free(instance->buffer0);
    free(instance->buffer1);
    if(instance->kern_params != NULL) free(instance->kern_params);

    if(instance->instance_mode == MAF_INSTANCE_DYNAMIC)
    {
        free(instance);
    }
}

float maf_simple_average_kern(maf_t *instance, size_t index)
{
    return ((float)1) / ((float)instance->buffer_length);
}

uint8_t maf_set_simple_average(maf_t *instance)
{
    if(instance == NULL) return RETURN_ERROR;
    if(instance->instance_mode == MAF_INSTANCE_NONE) return RETURN_ERROR;
    
    instance->kern_function = maf_simple_average_kern;
    
    if(instance->kern_params != NULL)
    { 
        free(instance->kern_params);
        instance->kern_params = NULL;
        instance->kern_params_length = 0;
    }

    return RETURN_SUCCESS;
}