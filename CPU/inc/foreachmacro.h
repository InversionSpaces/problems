#pragma once

#define FOR_EACH_1(what, x) what x
#define FOR_EACH_2(what, x, ...) what x FOR_EACH_1(what, __VA_ARGS__)
#define FOR_EACH_3(what, x, ...) what x FOR_EACH_2(what, __VA_ARGS__)
#define FOR_EACH_4(what, x, ...) what x FOR_EACH_3(what, __VA_ARGS__)
#define FOR_EACH_5(what, x, ...) what x FOR_EACH_4(what, __VA_ARGS__)
#define FOR_EACH_6(what, x, ...) what x FOR_EACH_5(what, __VA_ARGS__)
#define FOR_EACH_7(what, x, ...) what x FOR_EACH_6(what, __VA_ARGS__)
#define FOR_EACH_8(what, x, ...) what x FOR_EACH_7(what, __VA_ARGS__)
#define FOR_EACH_9(what, x, ...) what x FOR_EACH_8(what, __VA_ARGS__)
#define FOR_EACH_10(what, x, ...) what x FOR_EACH_9(what, __VA_ARGS__)
#define FOR_EACH_11(what, x, ...) what x FOR_EACH_10(what, __VA_ARGS__)
#define FOR_EACH_12(what, x, ...) what x FOR_EACH_11(what, __VA_ARGS__)
#define FOR_EACH_13(what, x, ...) what x FOR_EACH_12(what, __VA_ARGS__)

#define NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _N, ...) _N

#define FOR_EACH(what, ...) NTH_ARG(__VA_ARGS__, FOR_EACH_13, FOR_EACH_12, FOR_EACH_11, FOR_EACH_10, FOR_EACH_9, FOR_EACH_8, FOR_EACH_7, FOR_EACH_6, FOR_EACH_5, FOR_EACH_4, FOR_EACH_3,FOR_EACH_2, FOR_EACH_1)(what, __VA_ARGS__)

#define GET_1(x, ...) x
#define GET_2(x, ...) GET_1(__VA_ARGS__)
#define GET_3(x, ...) GET_2(__VA_ARGS__)
#define GET_4(x, ...) GET_3(__VA_ARGS__)
#define GET_5(x, ...) GET_4(__VA_ARGS__)
#define GET_6(x, ...) GET_5(__VA_ARGS__)

#define UNTUPLE(...) __VA_ARGS__
#define EVAL_UNTUPLE(...) UNTUPLE __VA_ARGS__

#define CONCAT(a, b) a ## b
#define EVAL_CONCAT(a, b) CONCAT(a, b)

#define STRING(x) #x
#define EVAL_STRING(x) STRING(x)

#define COUNT_ARGS(...) NTH_ARG(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
