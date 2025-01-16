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
_FIXEDPT_PROTOTYPE fixedpt fixedpt_sin(fixedpt angle);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_cos(fixedpt angle);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_tan(fixedpt angle);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_asin(fixedpt x);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_acos(fixedpt x);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_atan(fixedpt x);
_FIXEDPT_PROTOTYPE fixedpt fixedpt_atan2(fixedpt y, fixedpt x);


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
	int invert = 0;
	int iter = FIXEDPT_FBITS;
	fixedpt l;
    int i;

	if (A < 0)
		return (-1);
	if (A == 0 || A == FIXEDPT_ONE)
		return (A);
	if (A < FIXEDPT_ONE && A > 6) {
		invert = 1;
		A = fixedpt_div(FIXEDPT_ONE, A);
	}
	if (A > FIXEDPT_ONE) {
		fixedpt s = A;

		iter = 0;
		while (s > 0) {
			s >>= 2;
			iter++;
		}
	}

	/* Newton's iterations */
	l = (A >> 1) + 1;
	for (i = 0; i < iter; i++)
		l = fixedpt_add(l, fixedpt_div(A, l)) >> 1;
	if (invert)
		return (fixedpt_div(FIXEDPT_ONE, l));
	return (l);
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

/* Returns the sine of the given fixedpt number. 
 * Note: the loss of precision is extraordinary! */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_sin(fixedpt angle)
{
	/* Minimax coefficients */
	/* These coefficients are precomputed and provide a high-accuracy approximation. */
	static const fixedpt c[7] ={ 
		fixedpt_rconst(1.0),
		fixedpt_rconst(-0.16666666641626524), // -1/3!
		fixedpt_rconst(0.008333333171954214), // 1/5!
		fixedpt_rconst(-0.0001984126963009452), // -1/7!
		fixedpt_rconst(2.755731884462877e-6), // 1/9!
		fixedpt_rconst(-2.505210838544172e-8), // -1/11!
		fixedpt_rconst(1.605904383682161e-10) // 1/13!
	};

    /* Perform angle normalization */
    /* Step 1 : Normalize to [-2pi, 2pi] */
	angle %= FIXEDPT_TWO_PI;

    /* Step 2 : Normalize to [-pi, pi] */
    if (angle < -FIXEDPT_PI) { 
        angle = fixedpt_add(angle, FIXEDPT_TWO_PI);
	} else if (angle > FIXEDPT_PI) {
        angle = fixedpt_sub(angle, FIXEDPT_TWO_PI);
	}

    /* Step 3 :  Normalize to [-pi/2, pi/2] */
    if (angle > FIXEDPT_HALF_PI) {
        angle = fixedpt_sub(FIXEDPT_PI, angle);
        //flip_cos_sign = 1;
    } else if (angle < -FIXEDPT_HALF_PI) {
        angle = fixedpt_sub(-FIXEDPT_PI, angle);
       // flip_cos_sign = 1;        
    }

    // Minimax polynomial approximation for sin(x)
    fixedpt x2 = fixedpt_mul(angle, angle); // x^2
    return (fixedpt_mul(angle, fixedpt_add(c[0], 
			fixedpt_mul(x2, fixedpt_add(c[1],  
			fixedpt_mul(x2, fixedpt_add(c[2], 
			fixedpt_mul(x2, fixedpt_add(c[3],
			fixedpt_mul(x2, fixedpt_add(c[4],
			fixedpt_mul(x2, fixedpt_add(c[5],
			fixedpt_mul(x2, c[6])
			)))))))))))));
}

/* Returns the cosine of the given fixedpt number */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_cos(fixedpt angle)
{
	int flip_cos_sign = 0;

	/* Minimax coefficients */
	/* These coefficients are precomputed and provide a high-accuracy approximation. */
	static const fixedpt c[7] ={ 
		fixedpt_rconst(1.0),
		fixedpt_rconst(-0.5), // -1/2!
		fixedpt_rconst(0.04166666657946379), // 1/4!
		fixedpt_rconst(-0.001388888894063317), // -1/6!
		fixedpt_rconst(2.480158728947673e-5), // 1/8!
		fixedpt_rconst(-2.755731435139066e-7), // -1/10!
		fixedpt_rconst(2.087675440040727e-9) // 1/12!
	};

    /* Perform angle normalization */
    /* Step 1 : Normalize to [-2pi, 2pi] */
	angle %= FIXEDPT_TWO_PI;

    /* Step 2 : Normalize to [-pi, pi] */
    if (angle < -FIXEDPT_PI) { 
        angle = fixedpt_add(angle, FIXEDPT_TWO_PI);
	} else if (angle > FIXEDPT_PI) {
        angle = fixedpt_sub(angle, FIXEDPT_TWO_PI);
	}

    /* Step 3 :  Normalize to [-pi/2, pi/2] */
    if (angle > FIXEDPT_HALF_PI) {
        angle = fixedpt_sub(FIXEDPT_PI, angle);
        flip_cos_sign = 1;
    } else if (angle < -FIXEDPT_HALF_PI) {
        angle = fixedpt_sub(-FIXEDPT_PI, angle);
       flip_cos_sign = 1;        
    }
    /* Minimax polynomial approximation for cos(x) */
    fixedpt x2 = fixedpt_mul(angle, angle); // x^2

	fixedpt val = fixedpt_add(c[0], 
		fixedpt_mul(x2, fixedpt_add(c[1],
		fixedpt_mul(x2, fixedpt_add(c[2],
		fixedpt_mul(x2, fixedpt_add(c[3],
		fixedpt_mul(x2, fixedpt_add(c[4],
		fixedpt_mul(x2, fixedpt_add(c[5],
		fixedpt_mul(x2, c[6])
	)))))))))));

	return((flip_cos_sign == 0)?val:-val);
}

/* Returns the tangens of the given fixedpt number */
_FIXEDPT_FUNCTYPE fixedpt fixedpt_tan(fixedpt angle)
{
	return fixedpt_div(fixedpt_sin(angle), fixedpt_cos(angle));
}

// Minimax polynomial approximation for arctan(z) with 7 terms
_FIXEDPT_FUNCTYPE fixedpt fixedpt_atan(fixedpt z) 
{
    // Example coefficients for a degree-7 polynomial approximation
    // static fixedpt E[7] = {
    //     fixedpt_rconst(1.0),
    //     fixedpt_rconst(-0.3333333333333333),  // -1/3
    //     fixedpt_rconst(0.2),                 // 1/5
    //     fixedpt_rconst(-0.1428571428571429), // -1/7
    //     fixedpt_rconst(0.1111111111111111), // 1/9
    //     fixedpt_rconst(-0.0909090909090909), // -1/11
    //     fixedpt_rconst(0.0769230769230769) // 1/13
    // };
	// Constatnt in Intel Pentium
    static fixedpt E[7] = {
        fixedpt_rconst(1.0),
        fixedpt_rconst(-0.3333333333),  // -1/3
        fixedpt_rconst(0.2),                 // 1/5
        fixedpt_rconst(-0.1428571429), // -1/7
        fixedpt_rconst(0.1111111089), // 1/9
        fixedpt_rconst(-0.0909075848), // -1/11
        fixedpt_rconst(0.0764169081) // 1/13
    };

	static fixedpt bias_atan[32] = {
		fixedpt_rconst(0.0000000000),  // atain(0/32)
		fixedpt_rconst(0.0312398334),  // atain(1/32)
		fixedpt_rconst(0.0624188100),  // atain(2/32)
		fixedpt_rconst(0.0934767812),  // atain(3/32)
		fixedpt_rconst(0.1243549945),  // atain(4/32)
		fixedpt_rconst(0.1549967419),  // atain(5/32)
		fixedpt_rconst(0.1853479500),  // atain(6/32)
		fixedpt_rconst(0.2153576997),  // atain(7/32)
		fixedpt_rconst(0.2449786631),  // atain(8/32)
		fixedpt_rconst(0.2741674511),  // atain(9/32)
		fixedpt_rconst(0.3028848684),  // atain(10/32)
		fixedpt_rconst(0.3310960767),  // atain(11/32)
		fixedpt_rconst(0.3587706703),  // atain(12/32)
		fixedpt_rconst(0.3858826694),  // atain(13/32)
		fixedpt_rconst(0.4124104416),  // atain(14/32)
		fixedpt_rconst(0.4383365599),  // atain(15/32)
		fixedpt_rconst(0.4636476090),  // atain(16/32)
		fixedpt_rconst(0.4883339511),  // atain(17/32)
		fixedpt_rconst(0.5123894603),  // atain(18/32)
		fixedpt_rconst(0.5358112380),  // atain(19/32)
		fixedpt_rconst(0.5585993153),  // atain(20/32)
		fixedpt_rconst(0.5807563536),  // atain(21/32)
		fixedpt_rconst(0.6022873461),  // atain(22/32)
		fixedpt_rconst(0.6231993299),  // atain(23/32)
		fixedpt_rconst(0.6435011088),  // atain(24/32)
		fixedpt_rconst(0.6632029927),  // atain(25/32)
		fixedpt_rconst(0.6823165549),  // atain(26/32)
		fixedpt_rconst(0.7008544079),  // atain(27/32)
		fixedpt_rconst(0.7188299996),  // atain(28/32)
		fixedpt_rconst(0.7362574290),  // atain(29/32)
		fixedpt_rconst(0.7531512810),  // atain(30/32)
		fixedpt_rconst(0.7695264804)  // atain(31/32)
	};

	int signflip_flag = 0, inverse_flag = 0;
    fixedpt theta, factor_c = 0;

    // Reduce z to [0, inf] if necessary
	if(z < 0) {
		z = fixedpt_abs(z);
		signflip_flag = 1;
	}
	
	// Reduce z to [0, 1]
    if (z > FIXEDPT_ONE) {
        // Use arctan(z) = π/2 - arctan(1/z) for z > 1
        z = fixedpt_div(FIXEDPT_ONE, z);
		inverse_flag = 1;
    }

	// Reduce z to [0, 1/32] using the identity atan(x) = atan{(x - c) / (1 + x * c)} + atan(c)
	fixedpt c = fixedpt_rconst(31.0);
	factor_c = c >> 5; // c / 32.0
	while( fixedpt_sub(z, factor_c) < 0) {
		c = fixedpt_sub(c, FIXEDPT_ONE);
		factor_c = c >> 5; // c / 32.0
	}

	z = fixedpt_div( fixedpt_sub(z, factor_c), fixedpt_add(FIXEDPT_ONE, fixedpt_mul(z, factor_c)) );

    // Polynomial computation
    fixedpt z2 = fixedpt_mul(z, z);
    theta = ( fixedpt_mul(z, fixedpt_add(E[0],
		fixedpt_mul(z2, fixedpt_add(E[1],
		fixedpt_mul(z2, fixedpt_add(E[2],
		fixedpt_mul(z2, fixedpt_add(E[3],
		fixedpt_mul(z2, fixedpt_add(E[4],
		fixedpt_mul(z2, fixedpt_add(E[5],
		fixedpt_mul(z2, E[6])
	)))))))))))) );

	theta = fixedpt_add(theta, bias_atan[fixedpt_toint(c)]);

	if(inverse_flag == 1)
		theta = fixedpt_sub(FIXEDPT_HALF_PI, theta);  // Use arctan(z) = π/2 - arctan(1/z) for z > 1

	if(signflip_flag == 1)
		theta = -theta;

	return theta;
}

// atan2 implementation with domain reduction
_FIXEDPT_FUNCTYPE fixedpt fixedpt_atan2(fixedpt y, fixedpt x) 
{
    if (x == 0) {
        if (y > 0) return FIXEDPT_HALF_PI;   // Positive y-axis
        if (y < 0) return -FIXEDPT_HALF_PI; // Negative y-axis
        return 0;                  // Origin (undefined, return 0)
    }
    if (y == 0) {
        return (x > 0 ? 0.0 : FIXEDPT_PI); // Positive or negative x-axis
    }

	fixedpt theta = fixedpt_atan( fixedpt_div(y, x) );

    // Adjust theta for the correct quadrant
    if (x < 0) {
        if (y >= 0) {
            theta = fixedpt_add(theta, FIXEDPT_PI); // Quadrant II
        } else {
            theta = fixedpt_sub(theta, FIXEDPT_PI); // Quadrant III
        }
    }

    return theta;
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

#ifdef __cplusplus
}
#endif

#endif

#endif
