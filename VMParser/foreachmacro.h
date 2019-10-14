#pragma once

#define FOR_EACH_1(what, x) what(x)
#define FOR_EACH_2(what, x, ...) what(x) FOR_EACH_1(what, __VA_ARGS__)
#define FOR_EACH_3(what, x, ...) what(x) FOR_EACH_2(what, __VA_ARGS__)
#define FOR_EACH_4(what, x, ...) what(x) FOR_EACH_3(what, __VA_ARGS__)
#define FOR_EACH_5(what, x, ...) what(x) FOR_EACH_4(what, __VA_ARGS__)
#define FOR_EACH_6(what, x, ...) what(x) FOR_EACH_5(what, __VA_ARGS__)
#define FOR_EACH_7(what, x, ...) what(x) FOR_EACH_6(what, __VA_ARGS__)
#define FOR_EACH_8(what, x, ...) what(x) FOR_EACH_7(what, __VA_ARGS__)

#define GET_CYCLE(_1, _2, _3, _4, _5, _6, _7, _8, _N, ...) _N

#define FOR_EACH(what, ...) GET_CYCLE(__VA_ARGS__, FOR_EACH_8, FOR_EACH_7, FOR_EACH_6, FOR_EACH_5, FOR_EACH_4, FOR_EACH_3,FOR_EACH_2, FOR_EACH_1)(what, __VA_ARGS__)

