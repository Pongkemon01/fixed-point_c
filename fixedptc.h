#ifndef _FIXEDPTC_H_
#define _FIXEDPTC_H_

#ifdef _FIXEDPT_STATIC

#define _FIXEDPT_FUNCTYPE	static inline
#define _FIXEDPT_PROTOTYPE	static inline
#define _FIXEDPT_IMPLEMENTATION

#else

#define _FIXEDPT_FUNCTYPE

#ifdef _FIXEDPT_IMPLEMENTATION
#define _FIXEDPT_PROTOTYPE
#else
#define _FIXEDPT_PROTOTYPE extern
#endif

#endif

/*
 * fixedptc.h is a 32-bit or 64-bit fixed point numeric library.
 *
 * The symbol FIXEDPT_BITS, if defined before this library header file
 * is included, determines the number of bits in the data type (its "width").
 * The default width is 32-bit (FIXEDPT_BITS=32) and it can be used
 * on any recent C99 compiler. The 64-bit precision (FIXEDPT_BITS=64) is
 * available on compilers which implement 128-bit "long long" types. This
 * precision has been tested on GCC 4.2+.
 *
 * The FIXEDPT_WBITS symbols governs how many bits are dedicated to the
 * "whole" part of the number (to the left of the decimal point). The larger
 * this width is, the larger the numbers which can be stored in the fixedpt
 * number. The rest of the bits (available in the FIXEDPT_FBITS symbol) are
 * dedicated to the fraction part of the number (to the right of the decimal
 * point).
 *
 * Since the number of bits in both cases is relatively low, many complex
 * functions (more complex than div & mul) take a large hit on the precision
 * of the end result because errors in precision accumulate.
 * This loss of precision can be lessened by increasing the number of
 * bits dedicated to the fraction part, but at the loss of range.
 *
 * Adventurous users might utilize this library to build two data types:
 * one which has the range, and one which has the precision, and carefully
 * convert between them (including adding two number of each type to produce
 * a simulated type with a larger range and precision).
 *
 * The ideas and algorithms have been cherry-picked from a large number
 * of previous implementations available on the Internet.
 * Tim Hartrick has contributed cleanup and 64-bit support patches.
 *
 * == Special notes for the 32-bit precision ==
 * Signed 32-bit fixed point numeric library for the 24.8 format.
 * The specific limits are -8388608.999... to 8388607.999... and the
 * most precise number is 0.00390625. In practice, you should not count
 * on working with numbers larger than a million or to the precision
 * of more than 2 decimal places. Make peace with the fact that PI
 * is 3.14 here. :)
 */

/*-
 * Copyright (c) 2010-2012 Ivan Voras <ivoras@freebsd.org>
 * Copyright (c) 2012 Tim Hartrick <tim@edgecast.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdint.h>

#ifndef FIXEDPT_BITS
#define FIXEDPT_BITS	32
#endif

#ifndef FIXEDPT_WBITS
#if FIXEDPT_BITS == 32
#define FIXEDPT_WBITS	14
#else
#define FIXEDPT_WBITS	32
#endif
#endif

#if FIXEDPT_WBITS >= FIXEDPT_BITS
#error "FIXEDPT_WBITS must be less than or equal to FIXEDPT_BITS"
#endif

#if FIXEDPT_BITS == 32
typedef int32_t fixedpt;
typedef	int64_t	fixedptd;
typedef	uint32_t fixedptu;
typedef	uint64_t fixedptud;
#elif FIXEDPT_BITS == 64
typedef int64_t fixedpt;
typedef	__int128_t fixedptd;
typedef	uint64_t fixedptu;
typedef	__uint128_t fixedptud;
#else
#error "FIXEDPT_BITS must be equal to 32 or 64"
#endif


#define FIXEDPT_VCSID "$Id$"

#define FIXEDPT_FBITS	(FIXEDPT_BITS - FIXEDPT_WBITS)
#define FIXEDPT_FMASK	(((fixedpt)1 << FIXEDPT_FBITS) - 1)

#define fixedpt_rconst(R) ((fixedpt)((R) * FIXEDPT_ONE + ((R) >= 0 ? 0.5 : -0.5)))
#define fixedpt_fromint(I) ((fixedptd)(I) << FIXEDPT_FBITS)
#define fixedpt_toint(F) ((F) >> FIXEDPT_FBITS)
#define fixedpt_add(A,B) ((A) + (B))
#define fixedpt_sub(A,B) ((A) - (B))
#define fixedpt_xmul(A,B)						\
	((fixedpt)(((fixedptd)(A) * (fixedptd)(B)) >> FIXEDPT_FBITS))
#define fixedpt_xdiv(A,B)						\
	((fixedpt)(((fixedptd)(A) << FIXEDPT_FBITS) / (fixedptd)(B)))
#define fixedpt_fracpart(A) ((fixedpt)(A) & FIXEDPT_FMASK)

#define FIXEDPT_ONE	((fixedpt)((fixedpt)1 << FIXEDPT_FBITS))
#define FIXEDPT_ONE_HALF (FIXEDPT_ONE >> 1)
#define FIXEDPT_TWO	(FIXEDPT_ONE + FIXEDPT_ONE)

/* Trigonometry constants */
#define FIXEDPT_PI	fixedpt_rconst(3.14159265358979323846)
#define FIXEDPT_TWO_PI	fixedpt_rconst(2 * 3.14159265358979323846)
#define FIXEDPT_HALF_PI	fixedpt_rconst(3.14159265358979323846 / 2)
#define FIXEDPT_QUATER_PI	fixedpt_rconst(3.14159265358979323846 / 4)
#define FIXEDPT_THREEQUATER_PI	fixedpt_rconst(3.14159265358979323846 * 3 / 4)
#define FIXEDPT_ONETHIRD_PI fixedpt_rconst(3.14159265358979323846 / 3)          // pi / 3
#define FIXEDPT_TWO_ONETHIRD_PI  fixedpt_rconst(2 * 3.14159265358979323846 / 3) // 2pi / 3

/* Misc constant */
#define FIXEDPT_E	fixedpt_rconst(2.7182818284590452354)
#define FIXEDPT_SQRT_TWO	fixedpt_rconst(1.4142135623730950488)			// sqrt(2)
#define FIXEDPT_ONE_BY_SQRT_TWO	fixedpt_rconst(0.7071067811865474385)		// 1/sqrt(2)
#define FIXEDPT_SQRT_THREE fixedpt_rconst(1.73205080757)        	// sqrt(3)
#define FIXEDPT_ONE_BY_SQRT_THREE   fixedpt_rconst(0.57735026919)   // 1/sqrt(3)

#define fixedpt_abs(A) ((A) < 0 ? -(A) : (A))

/* fixedpt is meant to be usable in environments without floating point support
 * (e.g. microcontrollers, kernels), so we can't use floating point types directly.
 * Putting them only in macros will effectively make them optional. */
#define fixedpt_tofloat(T) ((float) ((T)*((float)(1)/(float)(1L << FIXEDPT_FBITS))))
#define fixedpt_todouble(T) ((double) ((T)*((double)(1)/(double)(1LL << FIXEDPT_FBITS))))

/* Function prototypes */

#ifdef __cplusplus
extern "C" {
#endif

_FIXEDPT_PROTOTYPE fixedpt fixedpt_mul(fixedpt A, fixedpt B);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_div(fixedpt A, fixedpt B);
_FIXEDPT_PROTOTYPE void fixedpt_str(fixedpt A, char *str, int max_dec);
_FIXEDPT_PROTOTYPE char* fixedpt_cstr(const fixedpt A, const int max_dec);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_sqrt(fixedpt A);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_exp(fixedpt x);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_ln(fixedpt x);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_log(fixedpt x, fixedpt base);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_pow(fixedpt x, fixedpt exp);
_FIXEDPT_PROTOTYPE void fixedpt_sincos(fixedpt angle, fixedpt *sin_val, fixedpt *cos_val) ;
_FIXEDPT_PROTOTYPE fixedpt fixedpt_atan2(fixedpt y, fixedpt x);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_sin(fixedpt A);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_cos(fixedpt A);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_tan(fixedpt A);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_asin(fixedpt x);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_acos(fixedpt x);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_atan(fixedpt x);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_bitwise_sqrt(fixedpt A);

#ifdef __cplusplus
}
#endif

#ifdef _FIXEDPT_IMPLEMENTATION

/* Implementation of the functions */

#ifdef __cplusplus
extern "C" {
#endif


/* Multiplies two fixedpt numbers, returns the result. */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_mul(fixedpt A, fixedpt B)
{
	fixedptd product = (fixedptd)A * (fixedptd)B;
	fixedpt result = (fixedpt)(product >> FIXEDPT_FBITS);
	fixedpt rounding = (fixedpt)((product  >> (FIXEDPT_FBITS - 1)) & 1);

	return (result + rounding);
}


/* Divides two fixedpt numbers, returns the result. */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_div(fixedpt A, fixedpt B)
{
	return (((fixedptd)A << FIXEDPT_FBITS) / (fixedptd)B);
}

/*
 * Note: adding and substracting fixedpt numbers can be done by using
 * the regular integer operators + and -.
 */

/**
 * Convert the given fixedpt number to a decimal string.
 * The max_dec argument specifies how many decimal digits to the right
 * of the decimal point to generate. If set to -1, the "default" number
 * of decimal digits will be used (2 for 32-bit fixedpt width, 10 for
 * 64-bit fixedpt width); If set to -2, "all" of the digits will
 * be returned, meaning there will be invalid, bogus digits outside the
 * specified precisions.
 */
_FIXEDPT_FUNCTYPE void fixedpt_str(fixedpt A, char *str, int max_dec)
{
	int ndec = 0, slen = 0;
	char tmp[12] = {0};
	fixedptud fr, ip;
	const fixedptud one = (fixedptud)1 << FIXEDPT_BITS;
	const fixedptud mask = one - 1;

	if (max_dec == -1)
#if FIXEDPT_BITS == 32
#if FIXEDPT_WBITS > 16
		max_dec = 2;
#else
		max_dec = 4;
#endif
#elif FIXEDPT_BITS == 64
		max_dec = 10;
#else
#error Invalid width
#endif
	else if (max_dec == -2)
		max_dec = 15;

	if (A < 0) {
		str[slen++] = '-';
		A *= -1;
	}

	ip = fixedpt_toint(A);
	do {
		tmp[ndec++] = '0' + ip % 10;
		ip /= 10;
	} while (ip != 0);

	while (ndec > 0)
		str[slen++] = tmp[--ndec];
	str[slen++] = '.';

	fr = (fixedpt_fracpart(A) << FIXEDPT_WBITS) & mask;
	do {
		fr = (fr & mask) * 10;

		str[slen++] = '0' + (fr >> FIXEDPT_BITS) % 10;
		ndec++;
	} while (fr != 0 && ndec < max_dec);

	if (ndec > 1 && str[slen-1] == '0')
		str[slen-1] = '\0'; /* cut off trailing 0 */
	else
		str[slen] = '\0';
}


/* Converts the given fixedpt number into a string, using a static
 * (non-threadsafe) string buffer */
_FIXEDPT_FUNCTYPE char* fixedpt_cstr(const fixedpt A, const int max_dec)
{
	static char str[25];

	fixedpt_str(A, str, max_dec);
	return (str);
}


/* Returns the square root of the given number, or -1 in case of error */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_sqrt(fixedpt A)
{
    fixedpt x = A; // Initial guess
	fixedpt res, diff;

	if (A < 0)
		return (-1);
	if (A == 0 || A == FIXEDPT_ONE)
		return (A);
	
	/* Using Heron’s method until result is changed with less than 0.00001 */
    do {
        res = fixedpt_add(x , fixedpt_div(A, x)) >> 1;
		diff = fixedpt_sub(res, x);
		if( diff < 0 )
			diff = -diff;
		x = res;
    }while( diff > fixedpt_rconst(0.00001));

    return x;
}

/* Returns the value exp(x), i.e. e^x of the given fixedpt number. */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_exp(fixedpt x)
{
	fixedpt xabs, k, z, R, xp;
	static const fixedpt LN2 = fixedpt_rconst(0.69314718055994530942);
	static const fixedpt LN2_INV = fixedpt_rconst(1.4426950408889634074);
	static const fixedpt EXP_P[5] = {
		fixedpt_rconst(1.66666666666666019037e-01),
		fixedpt_rconst(-2.77777777770155933842e-03),
		fixedpt_rconst(6.61375632143793436117e-05),
		fixedpt_rconst(-1.65339022054652515390e-06),
		fixedpt_rconst(4.13813679705723846039e-08),
	};

	if (x == 0)
		return (FIXEDPT_ONE);
	xabs = fixedpt_abs(x);
	k = fixedpt_mul(xabs, LN2_INV);
	k += FIXEDPT_ONE_HALF;
	k &= ~FIXEDPT_FMASK;
	if (x < 0)
		k = -k;
	x -= fixedpt_mul(k, LN2);
	z = fixedpt_mul(x, x);
	/* Taylor */
	R = FIXEDPT_TWO +
	    fixedpt_mul(z, EXP_P[0] + fixedpt_mul(z, EXP_P[1] +
	    fixedpt_mul(z, EXP_P[2] + fixedpt_mul(z, EXP_P[3] +
	    fixedpt_mul(z, EXP_P[4])))));
	xp = FIXEDPT_ONE + fixedpt_div(fixedpt_mul(x, FIXEDPT_TWO), R - x);
	if (k < 0)
		k = FIXEDPT_ONE >> (-k >> FIXEDPT_FBITS);
	else
		k = FIXEDPT_ONE << (k >> FIXEDPT_FBITS);
	return (fixedpt_mul(k, xp));
}


/* Returns the natural logarithm of the given fixedpt number. */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_ln(fixedpt x)
{
	fixedpt log2, xi;
	fixedpt f, s, z, w, R;
	static const fixedpt LN2 = fixedpt_rconst(0.69314718055994530942);
	static const fixedpt LG[7] = {
		fixedpt_rconst(6.666666666666735130e-01),
		fixedpt_rconst(3.999999999940941908e-01),
		fixedpt_rconst(2.857142874366239149e-01),
		fixedpt_rconst(2.222219843214978396e-01),
		fixedpt_rconst(1.818357216161805012e-01),
		fixedpt_rconst(1.531383769920937332e-01),
		fixedpt_rconst(1.479819860511658591e-01)
	};

	if (x < 0)
		return (0);
	if (x == 0)
		return 0xffffffff;

	log2 = 0;
	xi = x;
	while (xi > FIXEDPT_TWO) {
		xi >>= 1;
		log2++;
	}
	f = xi - FIXEDPT_ONE;
	s = fixedpt_div(f, FIXEDPT_TWO + f);
	z = fixedpt_mul(s, s);
	w = fixedpt_mul(z, z);
	R = fixedpt_mul(w, LG[1] + fixedpt_mul(w, LG[3]
	    + fixedpt_mul(w, LG[5]))) + fixedpt_mul(z, LG[0]
	    + fixedpt_mul(w, LG[2] + fixedpt_mul(w, LG[4]
	    + fixedpt_mul(w, LG[6]))));
	return (fixedpt_mul(LN2, (log2 << FIXEDPT_FBITS)) + f
	    - fixedpt_mul(s, f - R));
}
	

/* Returns the logarithm of the given base of the given fixedpt number */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_log(fixedpt x, fixedpt base)
{
	return (fixedpt_div(fixedpt_ln(x), fixedpt_ln(base)));
}


/* Return the power value (n^exp) of the given fixedpt numbers */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_pow(fixedpt x, fixedpt exp)
{
	if (exp == 0)
		return (FIXEDPT_ONE);
	if (x < 0)
		return 0;
	return (fixedpt_exp(fixedpt_mul(fixedpt_ln(x), exp)));
}

/* Function to perform the CORDIC algorithm
 * Define the number of iterations */
#define MAX_ITERATIONS 16

#ifndef ITERATIONS
	#define ITERATIONS 16
#endif

#if ITERATIONS > MAX_ITERATIONS
	#undef ITEERATIONS
	#define ITERATIONS MAX_TERATIONS
#endif

/* Constant */

/*
// Cordic table in 8087
1.570796×2-1	0.7853982	atan(20)
1.854590×2-2	0.4636476	atan(2-1)
1.959829×2-3	0.2449787	atan(2-2)
1.989680×2-4	0.1243550	atan(2-3)
1.997402×2-5	0.0624188	atan(2-4)
1.999349×2-6	0.0312398	atan(2-5)
1.999837×2-7	0.0156237	atan(2-6)
1.999959×2-8	0.0078123	atan(2-7)
1.999990×2-9	0.0039062	atan(2-8)
1.999997×2-10	0.0019531	atan(2-9)
1.999999×2-11	0.0009766	atan(2-10)
2.000000×2-12	0.0004883	atan(2-11)
2.000000×2-13	0.0002441	atan(2-12)
2.000000×2-14	0.0001221	atan(2-13)
2.000000×2-15	0.0000610	atan(2-14)
2.000000×2-16	0.0000305	atan(2-15)
*/


// static const fixedpt FIXEDPT_ATAN_TABLE[MAX_ITERATIONS] = {
//     fixedpt_rconst(0.7853982),  fixedpt_rconst(0.4636476),     
//     fixedpt_rconst(0.2449787),  fixedpt_rconst(0.1243550),
//     fixedpt_rconst(0.0624188),  fixedpt_rconst(0.0312398),   
//     fixedpt_rconst(0.0156237),  fixedpt_rconst(0.0078123),
//     fixedpt_rconst(0.0039062),  fixedpt_rconst(0.0019531),  
//     fixedpt_rconst(0.0009766),  fixedpt_rconst(0.0004883),
//     fixedpt_rconst(0.0002441),  fixedpt_rconst(0.0001221), 
//     fixedpt_rconst(0.0000610),  fixedpt_rconst(0.0000305)
// };

/* Precomputed arctangent values for the circular CORDIC algorithm */
static const fixedpt FIXEDPT_ATAN_TABLE[MAX_ITERATIONS] = {
    fixedpt_rconst(0.7853981633974483),             fixedpt_rconst(0.4636476090008061),     
    fixedpt_rconst(0.24497866312686414),            fixedpt_rconst(0.12435499454676144),
    fixedpt_rconst(0.06241880999595735),            fixedpt_rconst(0.031239833430268277),   
    fixedpt_rconst(0.015623728620476831),           fixedpt_rconst(0.007812341060101111),
    fixedpt_rconst(0.0039062301319669718),          fixedpt_rconst(0.0019531225164788188),  
    fixedpt_rconst(0.0009765621895593195),          fixedpt_rconst(0.0004882812111948983),
    fixedpt_rconst(0.00024414062014936177),         fixedpt_rconst(0.00012207031189367021), 
    fixedpt_rconst(0.00006103515617420877),         fixedpt_rconst(0.000030517578115526096),
//     fixedpt_rconst(0.000015258789061315762),        fixedpt_rconst(0.00000762939453110197),
//     fixedpt_rconst(0.000003814697265606496),        fixedpt_rconst(0.000001907348632810187),
//     fixedpt_rconst(0.0000009536743164059606),       fixedpt_rconst(0.0000004768371582030885), 
//     fixedpt_rconst(0.00000023841857910155712),      fixedpt_rconst(0.00000011920928955078059),
//     fixedpt_rconst(0.000000059604644775390625),     fixedpt_rconst(0.000000029802322387695312), 
//     fixedpt_rconst(0.000000014901161193847656),     fixedpt_rconst(0.000000007450580596923828),
//     fixedpt_rconst(0.000000003725290298461914),     fixedpt_rconst(0.000000001862645149230957), 
//     fixedpt_rconst(0.0000000009313225746154785),    fixedpt_rconst(0.0000000004656612873077393)
};
//static const fixedpt FIXEDPT_CIRCULAR_CORDIC_K = fixedpt_rconst(0.6072529350088812561694) ; /* K value for 32 iterations */
static const fixedpt FIXEDPT_CIRCULAR_CORDIC_K = fixedpt_rconst(0.60725303) ; /* K value for 16 iterations */

/* Function to compute sine and cosine using CORDIC algorithm */
_FIXEDPT_FUNCTYPE void fixedpt_sincos(fixedpt angle, fixedpt *sin_val, fixedpt *cos_val) 
{
    /* Initialize variables */
    fixedpt x = FIXEDPT_CIRCULAR_CORDIC_K;
    fixedpt y = 0;
    fixedpt xt, yt;
    int flip_cos_sign = 0;

    /* Perform angle normalization */
    /* Step 1 : Normalize to [-2pi, 2pi] */
	while (angle >= FIXEDPT_TWO_PI)
		angle = fixedpt_sub(angle, FIXEDPT_TWO_PI);
	while (angle <= -FIXEDPT_TWO_PI)
		angle = fixedpt_add(angle, FIXEDPT_TWO_PI);

    /* Step 2 : Normalize to [-pi, pi] */
    if (angle < -FIXEDPT_PI) 
        angle = fixedpt_add(angle, FIXEDPT_TWO_PI);
    if (angle > FIXEDPT_PI) 
        angle = fixedpt_sub(angle, FIXEDPT_TWO_PI);

    /* Step 3 :  Normalize to [-pi/2, pi/2] */
    if (angle > FIXEDPT_HALF_PI) {
        angle = fixedpt_sub(FIXEDPT_PI, angle);
        flip_cos_sign = 1;
    } else if (angle < -FIXEDPT_HALF_PI) {
        angle = fixedpt_sub(-FIXEDPT_PI, angle);
        flip_cos_sign = 1;        
    }

    /* Perform Rotation-mode CORDIC iterations */
    for (int i = 0; i < ITERATIONS; i++) {
        if (angle < 0) {
            xt = fixedpt_add(x, (y >> i));
            yt = fixedpt_sub(y, (x >> i));
            angle = fixedpt_add(angle, FIXEDPT_ATAN_TABLE[i]);
        } else {
            xt = fixedpt_sub(x, (y >> i));
            yt = fixedpt_add(y, (x >> i));
            angle = fixedpt_sub(angle, FIXEDPT_ATAN_TABLE[i]);
        }
        x = xt;
        y = yt;
    }

    /* Store the results */
    *cos_val = (flip_cos_sign != 0)? -x : x ;
    *sin_val = y;
}

/* Returns the arctan2 of the given x, y coordinate in fixedpt number using CORDIC algorithm */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_atan2(fixedpt y, fixedpt x) 
{
    fixedpt angle = 0;
    fixedpt xt, yt;

    /* Initialize angle */
    if (x < 0) {
        if (y >= 0) {
            angle = FIXEDPT_HALF_PI;
        } else {
            angle = -FIXEDPT_HALF_PI;
        }
        fixedpt temp = x;
        x = y;
        y = -temp;
    }

    /* Perform Vectoring-mode CORDIC iterations */
    for (int i = 0; i < ITERATIONS; i++) {
        if (y >= 0) {
            xt = fixedpt_add(x, (y >> i));
            yt = fixedpt_sub(y, (x >> i));
            angle = fixedpt_add(angle, FIXEDPT_ATAN_TABLE[i]);
        } else {
            xt = fixedpt_sub(x, (y >> i));
            yt = fixedpt_add(y, (x >> i));
            angle = fixedpt_sub(angle, FIXEDPT_ATAN_TABLE[i]);
        }
        x = xt;
        y = yt;
    }

    return angle;
}

/* Returns the sine of the given fixedpt number. 
 * Note: the loss of precision is extraordinary! */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_sin(fixedpt A)
{
	fixedpt sin_t, cos_t;

	fixedpt_sincos(A, &sin_t, &cos_t);
	return sin_t;
}

/* Returns the cosine of the given fixedpt number */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_cos(fixedpt A)
{
	fixedpt sin_t, cos_t;
	
	fixedpt_sincos(A, &sin_t, &cos_t);
	return cos_t;
}

/* Returns the tangens of the given fixedpt number */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_tan(fixedpt A)
{
	fixedpt sin_t, cos_t;

	fixedpt_sincos(A, &sin_t, &cos_t);
	return fixedpt_div(sin_t, cos_t);
}

/* Returns the arcsin of the given fixedpt number */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_asin(fixedpt x)
{
	/* Ensure input is within valid range (-1 to 1) */
	if (x > FIXEDPT_ONE || x < -FIXEDPT_ONE) 
	{
		return x;
	}

	/* Handle special cases (x = ±1) */
	if (x == FIXEDPT_ONE) 
	{
		return FIXEDPT_HALF_PI;
	} 
	else if (x == -FIXEDPT_ONE) 
	{
		return -FIXEDPT_HALF_PI;
	}

	return fixedpt_atan2(x, fixedpt_sqrt(fixedpt_sub(FIXEDPT_ONE, fixedpt_mul(x, x))));
}

/* Returns the arccos of the given fixedpt number */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_acos(fixedpt x)
{
	/* Ensure input is within valid range (-1 to 1) */
	if (x > FIXEDPT_ONE || x < -FIXEDPT_ONE) 
	{
		return x;
	}

	/* Handle special cases (x = ±1) */
	if (x == FIXEDPT_ONE) 
	{
		return 0;
	} 
	else if (x == -FIXEDPT_ONE) 
	{
		return FIXEDPT_PI;
	}

	return fixedpt_atan2(fixedpt_sqrt(fixedpt_sub(FIXEDPT_ONE, fixedpt_mul(x, x))), x);
}

/* Returns the arctan of the given fixedpt number */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_atan(fixedpt x)
{
	return (fixedpt_atan2(x, 1));
}

// /* Implementation of LOG and EXP function using CORDIC.
//    The constants ITERATIONS and MAX_ITERATIONS are defined for circular CORDIC above.
// */
// /* Constant */
// /* Precomputed arctangent values for the circular CORDIC algorithm */
// static const fixedpt FIXEDPT_ATANH_TABLE[MAX_ITERATIONS] = {
//     fixedpt_rconst(0.5493061443340549),     fixedpt_rconst(0.2554128118829954),
//     fixedpt_rconst(0.1256572141404531),     fixedpt_rconst(0.0625815714770030),
//     fixedpt_rconst(0.0312601784906670),     fixedpt_rconst(0.0156262717520522),
//     fixedpt_rconst(0.0078126589515404),     fixedpt_rconst(0.0039062698683968),
//     fixedpt_rconst(0.0019531274835326),     fixedpt_rconst(0.0009765628104410),
//     fixedpt_rconst(0.0004882812888051),     fixedpt_rconst(0.0002441406298506),
//     fixedpt_rconst(0.0001220703131063),     fixedpt_rconst(0.0000610351563258),
//     fixedpt_rconst(0.0000305175781345),     fixedpt_rconst(0.0000152587890637),
//     fixedpt_rconst(0.0000076293945314),     fixedpt_rconst(0.0000038146972656),
//     fixedpt_rconst(0.0000019073486328),     fixedpt_rconst(0.0000009536743164),
//     fixedpt_rconst(0.0000004768371582),     fixedpt_rconst(0.0000002384185791),
//     fixedpt_rconst(0.0000001192092896),     fixedpt_rconst(0.0000000596046448),
//     fixedpt_rconst(0.0000000298023224),     fixedpt_rconst(0.0000000149011612),
//     fixedpt_rconst(0.0000000074505806),     fixedpt_rconst(0.0000000037252903),
//     fixedpt_rconst(0.0000000018626451),     fixedpt_rconst(0.0000000009313226),
//     fixedpt_rconst(0.0000000004656613),     fixedpt_rconst(0.0000000002328306)
// };
// static const fixedpt FIXEDPT_HYPERBOLIC_CORDIC_K = fixedpt_rconst(1.207497067763) ; /* K value for 32 iterations */


// /* Returns the natural logarithm of the given fixedpt number. */
// _FIXEDPT_FUNCTYPE fixedpt fixedpt_cordic_ln(fixedpt x)
// {
//     fixedpt xt = fixedpt_add(x, FIXEDPT_ONE);
//     fixedpt yt = fixedpt_sub(x, FIXEDPT_ONE);
//     fixedpt zt = 0;

//     fixedpt x_new, y_new;

//     // Perform iterations (vector mode)
//     for (int i = 0; i < ITERATIONS; i++) 
//     {
//         if( yt < 0 )
//         {
//             x_new = fixedpt_add(xt, (yt >> (i + 1)));
//             y_new = fixedpt_add(yt, (xt >> (i + 1)));
//             zt = fixedpt_sub(zt, FIXEDPT_ATANH_TABLE[i]);
//         }
//         else
//         {
//             x_new = fixedpt_sub(xt, (yt >> (i + 1)));
//             y_new = fixedpt_sub(yt, (xt >> (i + 1)));
//             zt = fixedpt_add(zt, FIXEDPT_ATANH_TABLE[i]);
//         }
//         xt = x_new;
//         yt = y_new;
//     }

//     return zt << 1;  // log(value)
// }

/* Returns the square root of the given number, or -1 in case of error */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_bitwise_sqrt(fixedpt A)
{
    fixedpt res = 0;
    fixedpt bit = (fixedpt)1 << (FIXEDPT_BITS - 2); // The second highest bit set
	fixedpt tmp;
	
	if (A < 0)
		return (-1);
	if (A == 0 || A == FIXEDPT_ONE)
		return (A);

    // "bit" starts at the highest power of four <= the input value.
    while(bit > A) {
        bit >>= 2;
    }

    while(bit != 0) {
		tmp = fixedpt_add(res, bit);
        if(A >= tmp) {
            A = fixedpt_sub(A, tmp);
            res = fixedpt_add((res >> 1), bit);
        } else {
            res >>= 1;
        }
        bit >>= 2;
    }

    return res << (FIXEDPT_FBITS >> 1);;
}

#ifdef __cplusplus
}
#endif

#endif

#endif
