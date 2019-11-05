#include <gtest/gtest.h>

typedef int stack_el_t;

#include "ProtectedStack.hpp"

TEST(PStackTests, PushTest) 
{
	PStack_t stk = {};
	
	ASSERT_EQ(PStackInitMACRO(&stk, 20), NO_ERROR);
	
	for (int i = 0; i < 20; ++i)
		ASSERT_EQ(PStackPush(&stk, i), NO_ERROR);
		
	ASSERT_EQ(PStackDeInit(&stk), NO_ERROR);
}

TEST(PStackTests, PushPopTest) 
{
	PStack_t stk = {};
	
	ASSERT_EQ(PStackInitMACRO(&stk, 20), NO_ERROR);
	
	for (int i = 0; i < 20; ++i)
		ASSERT_EQ(PStackPush(&stk, i), NO_ERROR);
	
	int tmp = -1;
	for (int i = 0; i < 20; ++i) {
		ASSERT_EQ(PStackPop(&stk, &tmp), NO_ERROR);
		ASSERT_EQ(tmp, 19 - i);
	}
		
	ASSERT_EQ(PStackDeInit(&stk), NO_ERROR);
}

TEST(PStackTests, EmptyTest) 
{
	PStack_t stk = {};
	
	ASSERT_EQ(PStackInitMACRO(&stk, 20), NO_ERROR);
	
	int empty = -1;
	
	ASSERT_EQ(PStackIsEmpty(&stk, &empty), NO_ERROR);
	ASSERT_EQ(empty, 1);
	
	for (int i = 0; i < 20; ++i)
		ASSERT_EQ(PStackPush(&stk, i), NO_ERROR);
		
	ASSERT_EQ(PStackIsEmpty(&stk, &empty), NO_ERROR);
	ASSERT_EQ(empty, 0);
	
	int tmp = -1;
	for (int i = 0; i < 20; ++i) {
		ASSERT_EQ(PStackPop(&stk, &tmp), NO_ERROR);
		ASSERT_EQ(tmp, 19 - i);
	}
	
	ASSERT_EQ(PStackIsEmpty(&stk, &empty), NO_ERROR);
	ASSERT_EQ(empty, 1);
		
	ASSERT_EQ(PStackDeInit(&stk), NO_ERROR);
}

TEST(PStackTests, PopFromEmptyTest) 
{
	PStack_t stk = {};
	
	ASSERT_EQ(PStackInitMACRO(&stk, 20), NO_ERROR);
	
	int tmp = -1;
	ASSERT_EQ(PStackPop(&stk, &tmp), TOO_SMALL_SIZE | NO_ERROR);
		
	ASSERT_EQ(PStackDeInit(&stk), NO_ERROR);
}


TEST(PStackTests, PushMoreThenCapacityTest) 
{
	PStack_t stk = {};
	
	ASSERT_EQ(PStackInitMACRO(&stk, 20), NO_ERROR);
	
	for (int i = 0; i < 100; ++i)
		ASSERT_EQ(PStackPush(&stk, i), NO_ERROR);
		
	ASSERT_EQ(PStackDeInit(&stk), NO_ERROR);
}
