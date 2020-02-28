#include <moving_average.h>
#include <stdio.h>

#define SIGNAL_LEN 20
float signal[SIGNAL_LEN] = {5,5,5,3,5,5,6,5,4,5,5,7,5,5,4,5,5,5,8,5};
float filtered_signal[SIGNAL_LEN];

maf_t filter;

void show_signal(float *sig)
{
    for(int i = 0; i < SIGNAL_LEN; i++)
    {
        printf("%.2f|", sig[i]);
    }

    printf("\n");
}

void apply_filter()
{
    for(int i = 0; i < SIGNAL_LEN; i++)
    {
        filtered_signal[i] = maf_filter(&filter, signal[i]);
    }
}

int main()
{
    init_maf(&filter, 4);

    printf("INPUT SIGNAL:\n");
    show_signal(signal);
    
    apply_filter();

    printf("OUTPUT SIGNAL:\n");
    show_signal(filtered_signal);
    
    delete_maf(&filter);
    
    return 0;
}