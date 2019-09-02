/*! @file main.c
 * Square equation solver
 */

#include "stdio.h"
#include "math.h"
#include "assert.h"

#define EPS (1e-10) ///< Значение для учёта ошибки точности чисел с плавающей точкой

/// @details Перечисление возможного количества корней уравнения
enum EQ_SOLVE_NUM_ROOTS {
	NONE, ///< Корней нет
	ONE, ///< Корень один
	TWO, ///< Корня два
	INF ///< Корней бесконечно много
};

/*! Функция проверки равенства двух действительных чисел
 * @param [in] a Первое число
 * @param [in] b Второе число
 * @return 1 если a == b иначе 0
 */
int isEqual(double a, double b) {
	assert(isfinite(a));
	assert(isfinite(b));
	return fabs(a - b) < EPS;
}

/*! Функция сравнения двух действительных чисел
 * @param [in] a Первое число
 * @param [in] b Второе число
 * @return 1 если a > b иначе 0
 */
int isGreater(double a, double b) {
	assert(isfinite(a));
	assert(isfinite(b));
	return a - b > EPS;
}

/*! Функция сравнения двух действительных чисел
 * @param [in] a Первое число
 * @param [in] b Второе число
 * @return 1 если a < b иначе 0
 */
int isLess(double a, double b) {
	assert(isfinite(a));
	assert(isfinite(b));
	return isGreater(b, a);
}

/*! Функция решения линейного уравнения ax + b = 0
 * @ref EQ_SOLVE_NUM_ROOTS
 * @param [in] a Коэффициент а
 * @param [in] b Коэффициент b
 * @param [out] x Указатель на корень
 * @return NONE, ONE или INF
 */
int SolveLnEq(double a, double b, double* x) {
	assert(isfinite(a));
	assert(isfinite(b));
	assert(x != NULL);
	
	if(isEqual(a, 0)) {
		*x = NAN;
		return isEqual(b, 0) ? INF : NONE;
	}
	*x = b / a;
	return ONE;
}


/*! Функция решения квадратного уравнения ax^2 + bx + c = 0
 * @ref EQ_SOLVE_NUM_ROOTS
 * @param [in] a Коэффициент a
 * @param [in] b Коэффициент b
 * @param [in] c Коэффициент c
 * @param [out] x1 Указатель на первый корень
 * @param [out] x2 Указатель на второй корень
 * @return NONE, ONE, TWO или INF
 */
int SolveSqEq(double a, double b, double c, double* x1, double* x2) {
	assert(isfinite(a));
	assert(isfinite(b));
	assert(isfinite(c));
	assert(x1 != NULL);
	assert(x2 != NULL);
	assert(x1 != x2);
	
	if(isEqual(a, 0)) {
		*x2 = NAN;
		return SolveLnEq(b, c, x1);
	}
	double d2 = b * b - 4 * a * c;
	if(isLess(d2, 0)) {
		*x1 = NAN;
		*x2 = NAN;
		return NONE;
	}
	else if(isEqual(d2, 0)) {
		*x1 = -b / (2 * a);
		*x2 = NAN;
		return ONE;
	}
	d2 = sqrt(d2);
	*x1 = (-b + d2) / (2 * a);
	*x2 = (-b - d2) / (2 * a);
	return TWO;
}

#ifndef NDEBUG
	#define PRINTF_DEBUG(...) printf(__VA_ARGS__)
	#define PRINTF_MACHINE(...) 
#else
	#define PRINTF_DEBUG(...)
	#define PRINTF_MACHINE(...) printf(__VA_ARGS__)
#endif

int main(int argc, char *argv[]) {
	#ifndef TEST
	PRINTF_DEBUG("# Square equation solver by InversionSpaces \n");
	PRINTF_DEBUG("# Enter a, b and c separated by space: \n");
	
	double a = NAN, b = NAN, c = NAN;
	int scancount = scanf("%lg %lg %lg", &a, &b, &c);
	
	if(scancount < 3) {
		PRINTF_DEBUG("# Only %d of arguments read successfully. Exiting\n", scancount);
		return 2;
	}
	
	double x1 = NAN, x2 = NAN;
	int roots = SolveSqEq(a, b, c, &x1, &x2);
	
	switch(roots) {
		case NONE:
			PRINTF_DEBUG("# No solutions can be found\n");
			PRINTF_MACHINE("0\n");
			break;
		case ONE:
			PRINTF_DEBUG("# There is one solution: %lg\n", x1);
			PRINTF_MACHINE("1 %lg\n", x1);
			break;
		case TWO:
			PRINTF_DEBUG("# There are two solutions: %lg and %lg\n", x1, x2);
			PRINTF_MACHINE("2 %lg %lg\n", x1, x2);
			break;
		case INF:
			PRINTF_DEBUG("# Solution is any number\n");
			PRINTF_MACHINE("inf\n");
			break;
		default:
			PRINTF_DEBUG("# ERROR: Unexpected return value: roots = %d\n", roots);
			return 1;
	}
	#else
	double x1 = NAN, x2 = NAN;
	printf("Solving x^2 + 2x + 2 = 0\n");
	assert(SolveSqEq(1.0, 2.0, 2.0, &x1, &x2) == NONE);
	assert(isnan(x1));
	assert(isnan(x2));
	
	printf("Solving x^2 + 2x + 1 = 0\n");
	assert(SolveSqEq(1.0, 2.0, 1.0, &x1, &x2) == ONE);
	assert(isEqual(x1, -1.0));
	assert(isnan(x2));
	
	printf("Solving x^2 - 3x + 2 = 0\n");
	assert(SolveSqEq(1.0, -3.0, 2.0, &x1, &x2) == TWO);
	assert(isEqual(x1, 2.0));
	assert(isEqual(x2, 1.0));
	
	printf("Solving 0x^2 + 0x + 0 = 0\n");
	assert(SolveSqEq(0.0, 0.0, 0.0, &x1, &x2) == INF);
	assert(isnan(x1));
	assert(isnan(x2));
	
	PRINTF_DEBUG("# All test passed. Exiting...\n");
	#endif
}
