/*
 *  OpenMP lecture exercises
 *  Copyright (C) 2011 by Christian Terboven <terboven@rz.rwth-aachen.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>                  

double CalcPi(int n);
double GetTime() {                 
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(int argc, char **argv)
{
    int n = 2000000000;
    const double fPi25DT = 3.141592653589793238462643;
    double fPi;
    double fTimeStart, fTimeEnd;  

#ifdef READ_INPUT
    printf("Enter the number of intervals: ");
    scanf("%d",&n);
#endif

    
    if (argc == 2) {
        n = atoi(argv[1]);
    }

    if (n <= 0 || n > 2147483647 )
    {
        printf("\ngiven value has to be between 0 and 2147483647\n");
        return 1;
    }
   
    fTimeStart = GetTime();      

    fPi = CalcPi(n);

    fTimeEnd = GetTime();        
   
    printf("\npi is approximately = %.20f\n", fPi);
    printf("Error = %.20f\n", fabs(fPi - fPi25DT));
   
    // imprimir tiempo con formato exacto
    printf("Elapsed time (CalcPi): %.6f seconds\n", fTimeEnd - fTimeStart);

    return 0;
}

double f(double a)
{
    return (4.0 / (1.0 + a*a));
}

double CalcPi(int n)
{
    const double fH = 1.0 / (double) n;
    double fSum = 0.0;
    double fX;
    int i;
    for (i = 0; i < n; i += 1)
    {
        fX = fH * ((double)i + 0.5);
        fSum += f(fX);
    }
    return fH * fSum;
}





















