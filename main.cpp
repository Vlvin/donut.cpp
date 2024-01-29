#include <iostream>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
struct Matrix3x3;
typedef struct Vec3 {
    double x,y,z;
    Vec3 operator+(Vec3 other) {
        Vec3 res{
            x + other.x,
            y + other.y,
            z + other.z
        };
        return res;
    }
    Vec3 operator+(double a) {
        Vec3 res{
            this->x + a,
            this->y + a,
            this->z + a
        };
        return res;
    }
    void operator+=(Vec3 other) {
        this->x = (*this + other).x;
        this->y = (*this + other).y;
        this->z = (*this + other).z;
    }
    Vec3 operator*(Matrix3x3 other);
    void print() {
        std::cout << x << " " << y << " " << z << std::endl;
    }
    void operator*=(Matrix3x3 other);
} Vec3;

typedef struct Matrix3x3 {
    double aa, ab, ac;
    double ba, bb, bc;
    double ca, cb, cc;

    Vec3 operator*(Vec3 other) {
        Vec3 res{
            aa*other.x + ab*other.y + ac*other.z,
            ba*other.x + bb*other.y + bc*other.z,
            ca*other.x + cb*other.y + cc*other.z
        };
        return res;
    }

    Matrix3x3 operator*(Matrix3x3 other) {
        Matrix3x3 res{
            aa*other.aa + ab*other.ba + ac*other.ca, aa*other.ab + ab*other.bb + ac*other.cb, aa*other.ac + ab*other.bc + ac*other.cc,
            ba*other.aa + bb*other.ba + bc*other.ca, ba*other.ab + bb*other.bb + bc*other.cb, ba*other.ac + bb*other.bc + bc*other.cc,
            ca*other.aa + cb*other.ba + cc*other.ca, ca*other.ab + cb*other.bb + cc*other.cb, ca*other.ac + cb*other.ba + cc*other.cc
        };
        return res;
    }

    Matrix3x3 operator+(double a) {
        Matrix3x3 res{
            aa+a, ab+a, ac+a,
            ba+a, bb+a, bc+a,
            ca+a, cb+a, cc+a
        };
        return res;
    }

    Matrix3x3 operator-(double a) {
        Matrix3x3 res{
            aa-a, ab-a, ac-a,
            ba-a, bb-a, bc-a,
            ca-a, cb-a, cc-a
        };
        return res;
    }
    void print() {
        std::cout << aa << " " << ab << " " << ac << 
        std::endl << ba << " " << bb << " " << bc << 
        std::endl << ca << " " << cb << " " << cc <<
        std::endl;
    }
} Matrix3x3;

Vec3 Vec3::operator*(Matrix3x3 other)  {
    Vec3 res{
        other.aa*x + other.ab*y + other.ac*z,
        other.ba*x + other.bb*y + other.bc*z,
        other.ca*x + other.cb*y + other.cc*z
    };
    return res;
}

void Vec3::operator*=(Matrix3x3 other) {
    this->x = (*this * other).x;
    this->y = (*this * other).y;
    this->z = (*this * other).z;
}

int main() {
    double i, j; // iterators
    int screen_width = 80; // screen width
    int screen_height = 44; // screen height
    int buffer_size = screen_width*screen_height; // compile time calculated buffer_size

    float z[buffer_size]; // z-buffer
    char screen[buffer_size]; // screen-buffer
    float ooz; // "one over zero": cause it's better to multiply than divide 

    char R2 = 2; // big radius
    char R1 = 1;// small radius
    char scale = 7; // scale
    char K2 = 5; // predefined distance screen:objectOrigin
    double K1 = screen_width*K2/(scale*(R1+R2));  // distance user:screen
    double sini, cosi, // i sin/cos value
           sinj, cosj, // j sin/cos value
           sinA, cosA, // A sin/cos value
           sinB, cosB; // B sin/cos value
    Vec3 xyz;  // vector of "Real" coordinates
    Vec3 orthxyz; // vector of transformed coordinates
    Matrix3x3 rotI; // Rotation matrix around small radius
    Matrix3x3 rotA; // Rotation matrix around A axile
    Matrix3x3 rotB; // Rotation matrix around B axile
    double A = 0, B = 0; // degree value for donut rotating around A/B
    printf("\x1b[2J"); // old-scool magic clear screen
    for(;;) { 
        /** if you really want explanation of things below 
         *  then goto donut.c creator's site by link below
         *  https://www.a1k0n.net/2011/07/20/donut-math.html
        */
        memset(screen, 32, buffer_size);
        memset(z, 0, buffer_size*sizeof(float));
        sinA = sin(A), cosA = cos(A);
        sinB = sin(B), cosB = cos(B);
        rotA = Matrix3x3 {
            1,     0,    0,
            0,  cosA, sinA,
            0, -sinA, cosA
        };
        rotB = Matrix3x3 {
             cosB, sinB, 0,
            -sinB, cosB, 0,
                0,    0, 1
        };
        for (j = 0; j < 6.28; j += 0.01){
            sinj = sin(j), cosj = cos(j); 
            for (i = 0; i < 6.28; i += 0.03) {
                sini = sin(i), cosi = cos(i);
                rotI = Matrix3x3{
                     cosi, 0, sini,
                     0,    1,    0,
                    -sini, 0, cosi,
                };
                
                xyz = Vec3{
                    R2*1.0, 0, 0
                };

                xyz += Vec3{
                    R1 * cosj, R1 * sinj, 0
                };
                xyz = rotI * xyz; 
                xyz = rotA * xyz; 
                xyz = rotB * xyz;
                ooz = 1/(K2+xyz.z); 
                orthxyz = Vec3{
                    screen_width/2 +((K1 * ooz * (xyz.x))),
                    screen_height/2 - ((K1 * ooz) * (xyz.y)),
                    ooz
                };

                Vec3 Nor{
                    cosj, sinj, 0
                };
                Nor = rotI * Nor;
                Nor = rotA * Nor;
                Nor = rotB * Nor;

                int L = 8*((Nor.x*0.5f) + ((Nor.y) + ((-Nor.z*0.5f)))); // Light index
                int o = (int)orthxyz.x + screen_width* (int)orthxyz.y; // screen buffer position
                if ((screen_height > orthxyz.y) && (orthxyz.y > 0) && (screen_width > orthxyz.x) && (orthxyz.x > 0) && (z[o] < ooz)) {
                    z[o] = ooz;
                    screen[o] = ".:!/r(l1Z4H9Q8$@"[(L>0)?(L<17?L:16):0];
                }
            }}
            printf("\x1b[H"); // another old-scool magic clear screen
                for(int k = 0; k < buffer_size; k++) {
                    putchar(k % screen_width ? screen[k] : 10); // print screen
                }
        A += 0.04;
        B += 0.02;
    }
    return 0;
}