/*
 *  UCF COP3330 Fall 2021 Assignment 5 Solution
 *  Copyright 2021 Kameryn Cambridge
 */

#include "std_lib_facilities.h"
#include <iostream>
#include <math.h>

double quadratic(double b, double a, double c);

//prints the roots of the quadratic equation given a, b, and c.
int main(){
    double b, a, c;
    //get input for values 
    cout << "Enter values for b, a, and c: \n";
    cin >> b >> a >> c;

    quadratic(b, a, c);

    return 0;
}

double quadratic(double b, double a, double c){
    double x1, x2;

    if(sqrt(b*b - 4*a*c) < 0){
        error("This equation has no real roots.\n");
    }
    if(sqrt(b*b - 4*a*c) == 0){
        x1 = -b;
        x1 += sqrt(b*b - 4*a*c);
        x1 /= (2*a);
        cout << "The only real root is " << x1 << ".\n";
    }
    else if (sqrt(b*b - 4*a*c) > 0){
        x1 = -b;
        x1 += sqrt(b*b - 4*a*c);
        x1 /= (2*a);

        x2 = -b;
        x2 -= sqrt(b*b - 4*a*c);
        x2 /= (2*a);

        cout << "The roots of the quadratic are " << x1 << " and " << x2 << ".\n";
    }

    return 0;
}