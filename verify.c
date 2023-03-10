#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <locale.h>

//#define FIXEDPT_WBITS 16
#include "fixedptc.h"

/* This test program verifies the fixedpt precision, comparing it to
 * float and double precision results. */

static const float pi_f = 3.14159265358979323846264338;
static const double pi_d = 3.14159265358979323846264338;
static const fixedpt pi_x = fixedpt_rconst(3.14159265358979323846264338);

static const float e_f = 2.71828182845904523536028747;
static const double e_d = 2.71828182845904523536028747;
static const fixedpt e_x = fixedpt_rconst(2.71828182845904523536028747);

static const float atan_1_f = 0.0;
static const float atan_2_f = 0.5;
static const float atan_3_f = 0.707106781;
static const float atan_4_f = 1.0;
static const float atan_5_f = 0.866025403;

static const double atan_1_d = 0.0;
static const double atan_2_d = 0.5;
static const double atan_3_d = 0.707106781;
static const double atan_4_d = 1.0;
static const double atan_5_d = 0.866025403;

static const fixedpt atan_1_x = fixedpt_rconst(0.0);
static const fixedpt atan_2_x = fixedpt_rconst(0.5);
static const fixedpt atan_3_x = fixedpt_rconst(0.707106781);
static const fixedpt atan_4_x = fixedpt_rconst(1.0);
static const fixedpt atan_5_x = fixedpt_rconst(0.866025403);


void
verify_numbers()
{
	printf("pi as string:\t3.14159265358979323846264338\n");
	printf("pi as float:\t%0.6f\n", pi_f);
	printf("pi as double:\t%0.15lf\n", pi_d);
	printf("pi as fixedpt:\t%s\n", fixedpt_cstr(pi_x, -2));
	printf("  delta fixedpt-double:\t%0.10lf\n", atof(fixedpt_cstr(pi_x, -2)) - pi_d);
	printf("pi as fixedpt converted to float: %0.6f\n", fixedpt_tofloat(pi_x));

	printf("e as string:\t2.71828182845904523536028747\n");
	printf("e as float:\t%0.6f\n", e_f);
	printf("e as double:\t%0.15lf\n", e_d);
	printf("e as fixedpt:\t%s\n", fixedpt_cstr(e_x, -2));
	printf("  delta fixedpt-double:\t%0.10lf\n", atof(fixedpt_cstr(e_x, -2)) - e_d);
}

void
verify_trig()
{
	printf("sin(pi) as float:\t%0.6f\n", sinf(pi_f));
	printf("sin(pi) as double:\t%0.15lf\n", sin(pi_d));
	printf("sin(pi) as fixedpt:\t%s\n", fixedpt_cstr(fixedpt_sin(pi_x), -2));
	printf("  delta fixedpt-double:\t%0.10lf\n", atof(fixedpt_cstr(fixedpt_sin(pi_x), -2)) - sin(pi_d));

	printf("sin(e) as float:\t%0.6f\n", sinf(e_f));
	printf("sin(e) as double:\t%0.15lf\n", sin(e_d));
	printf("sin(e) as fixedpt:\t%s\n", fixedpt_cstr(fixedpt_sin(e_x), -2));
	printf("  delta fixedpt-double:\t%0.10lf\n", atof(fixedpt_cstr(fixedpt_sin(e_x), -2)) - sin(e_d));

	printf("tan(e) as float:\t%0.6f\n", tanf(e_f));
	printf("tan(e) as double:\t%0.15lf\n", tan(e_d));
	printf("tan(e) as fixedpt:\t%s\n", fixedpt_cstr(fixedpt_tan(e_x), -2));
	printf("  delta fixedpt-double:\t%0.10lf\n", atof(fixedpt_cstr(fixedpt_tan(e_x), -2)) - tan(e_d));
}

void
verify_atan2()
{
	printf("atan2(x) as float:\t%0.6f\n", atan2f(atan_1_f, atan_4_f));
	printf("atan2(x) as double:\t%0.15lf\n", atan2(atan_1_d, atan_4_d));
	printf("atan2(x)) as fixedpt:\t%s\n", fixedpt_cstr(fixedpt_atan2(atan_1_x, atan_4_x), -2));
	printf("  delta fixedpt-double:\t%0.10lf\n", atof(fixedpt_cstr(fixedpt_atan2(atan_1_x, atan_4_x), -2)) - atan2(atan_1_d, atan_4_d));

	printf("atan2(x) as float:\t%0.6f\n", atan2f(atan_2_f, -atan_2_f));
	printf("atan2(x) as double:\t%0.15lf\n", atan2(atan_2_d, -atan_2_d));
	printf("atan2(x)) as fixedpt:\t%s\n", fixedpt_cstr(fixedpt_atan2(atan_2_x, -atan_2_x), -2));
	printf("  delta fixedpt-double:\t%0.10lf\n", atof(fixedpt_cstr(fixedpt_atan2(atan_2_x, -atan_2_x), -2)) - atan2(atan_2_d, -atan_2_d));

	printf("atan2(x) as float:\t%0.6f\n", atan2f(-atan_3_f, atan_5_f));
	printf("atan2(x) as double:\t%0.15lf\n", atan2(-atan_3_d, atan_5_d));
	printf("atan2(x)) as fixedpt:\t%s\n", fixedpt_cstr(fixedpt_atan2(-atan_3_x, atan_5_x), -2));
	printf("  delta fixedpt-double:\t%0.10lf\n", atof(fixedpt_cstr(fixedpt_atan2(-atan_3_x, atan_5_x), -2)) - atan2(-atan_3_d, atan_5_d));

}

void
verify_powers()
{
	printf("pow(pi,3) as float:\t%0.6f\n", powf(pi_f, 3));
	printf("pow(pi,3) as double:\t%0.15f\n", pow(pi_d, 3));
	printf("pow(pi,3) as fixedpt:\t%s\n", fixedpt_cstr(fixedpt_pow(pi_x, fixedpt_rconst(3)), -2));
	printf("  delta fixedpt-double:\t%0.10lf\n", atof(fixedpt_cstr(fixedpt_pow(pi_x, fixedpt_rconst(3)), -2)) - pow(pi_d, 3));

	printf("exp(3) as float:\t%0.6f\n", expf(3));
	printf("exp(3) as double:\t%0.15f\n", expf(3));
	printf("exp(3) as fixedpt:\t%s\n", fixedpt_cstr(fixedpt_exp(fixedpt_rconst(3)), -2));
	printf("  delta fixedpt-double:\t%0.10lf\n", atof(fixedpt_cstr(fixedpt_exp(fixedpt_rconst(3)), -2)) - exp(3));

	printf("ln(e) as float:\t%0.6f\n", logf(e_f));
	printf("ln(e) as double:\t%0.15f\n", log(e_d));
	printf("ln(e) as fixedpt:\t%s\n", fixedpt_cstr(fixedpt_ln(e_x), -2));
	printf("  delta fixedpt-double:\t%0.10lf\n", atof(fixedpt_cstr(fixedpt_ln(e_x), -2)) - log(e_d));
}

int
main() 
{
	setlocale(LC_NUMERIC, "C");
	printf("fixedptc library version: %s\n", FIXEDPT_VCSID);
	printf("Using %d-bit precision, %d.%d format\n\n", FIXEDPT_BITS, FIXEDPT_WBITS, FIXEDPT_FBITS);

	verify_numbers();
	printf("\n");
	verify_trig();
	printf("\n");
	verify_powers();
	printf("\n");
	verify_atan2();

	return (0);
}
