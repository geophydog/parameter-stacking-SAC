/*---------------------------------------------------------
----    2017-6-2 Intially coded by Xuping Feng @NJU     ---
----SAC_file: name of inputing SAC format file name     ---
----weighting_factor: weighting factor of SAC files     ---
----shift_time: shift time of SAC files                 ---
----Nth_power: power of inputing SAC files              ---
----------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sacio.h"

float sign ( float x ) {
    float sig;
    if ( x < 0. ) sig = -1.;
    else if ( x > 0. ) sig = 1.;
    else sig = 0.;
    return sig;
}

int main ( int argc, char *argv[] ) {
    int i, j, size = 1024, shift_index, count = 0, npts;
    float *data, *sum,  weight, shift_time, delta, power;
    char *ss = (char*) malloc(size), file[256];
    FILE *fp;
    SACHEAD hd;

    if ( argc != 3 ) {
        fprintf(stderr,"Usage: para_stack <parameter_file> <stacking_out_SAC_name>\n");
        fprintf(stderr,"          return stacked out SAC file\n");
        fprintf(stderr,"          <parameter_file> Containing parameters of 4 columns :\n");
        fprintf(stderr,"          1:SAC_file 2:weighting_factor 3:shift_time 4:Nth_power\n");
        exit(1);
    }

    fp = fopen(argv[1],"r");

    while ( fgets(ss, size, fp) ) {
        fscanf(fp, "%s %f %f %f", file, &weight, &shift_time, &power);
        data = read_sac(file, &hd);
        delta = hd.delta; npts = hd.npts;
        if ( count == 0 ) {
            sum = (float*) malloc(sizeof(npts) * npts);
            for ( i = 0; i < npts; i ++ )
                sum[i] = 0.;
        }
        shift_index = (int)(shift_time/delta);
        for ( i = 0; i < npts; i ++ ) {
            if ( shift_index >= 0 ) {
                if ( i < shift_index ) sum[i] += 0.;
                else sum[i] += sign(data[i-shift_index])*weight*pow(fabs(data[i-shift_index]),power);
            }
            else if ( shift_index < 0 && (i - shift_index) < npts )
                sum[i] += sign(data[i-shift_index])*weight*pow(fabs(data[i-shift_index]),power);
            else if ( shift_index < 0 && (i - shift_index) >= npts )
                sum[i] += 0.;
        count += 1;
        }
    }
    write_sac(argv[2], hd, sum);
    free(ss); free(sum); free(data);
    return 0;
}
