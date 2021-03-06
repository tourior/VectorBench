/*
* Tests for Simd Library (http://simd.sourceforge.net).
*
* Copyright (c) 2011-2016 Yermalayeu Ihar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy 
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
* copies of the Software, and to permit persons to whom the Software is 
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in 
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "Test/TestUtils.h"
#include "Test/TestPerformance.h"
#include "Test/TestData.h"

namespace Test
{
	namespace
	{
		struct Func1
		{
			typedef void (*FuncPtr)(const uint8_t *src, size_t srcWidth, size_t srcHeight, size_t srcStride, 
				 uint8_t *dst, size_t dstWidth, size_t dstHeight, size_t dstStride);

			FuncPtr func;
			String description;

			Func1(const FuncPtr & f, const String & d) : func(f), description(d) {}

			void Call(const View & src, View & dst) const
			{
				TEST_PERFORMANCE_TEST(description);
				func(src.data, src.width, src.height, src.stride, dst.data, dst.width, dst.height, dst.stride);
			}
		};
	}

#define FUNC1(function) Func1(function, #function)

	namespace
	{
		struct Func2
		{
			typedef void (*FuncPtr)(const uint8_t *src, size_t srcWidth, size_t srcHeight, size_t srcStride, 
				 uint8_t *dst, size_t dstWidth, size_t dstHeight, size_t dstStride, int correction);

			FuncPtr func;
			String description;
			bool correction;

			Func2(const FuncPtr & f, const String & d, bool c) : func(f), description(d + (c ? "[1]" : "[0]")), correction(c) {}

			void Call(const View & src, View & dst) const
			{
				TEST_PERFORMANCE_TEST(description);
				func(src.data, src.width, src.height, src.stride, dst.data, dst.width, dst.height, dst.stride, correction);
			}
		};
	}

#define FUNC2(function, correction) Func2(function, #function, correction)

	template <class Func>
	bool ReduceGrayAutoTest(int width, int height, const Func & f1, const Func & f2)
	{
		bool result = true;

		TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

		const int reducedWidth = (width + 1)/2;
		const int reducedHeight = (height + 1)/2;

		View s(width, height, View::Gray8, NULL, TEST_ALIGN(width));
		FillRandom(s);

		View d1(reducedWidth, reducedHeight, View::Gray8, NULL, TEST_ALIGN(reducedWidth));
		View d2(reducedWidth, reducedHeight, View::Gray8, NULL, TEST_ALIGN(reducedWidth));

		TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(s, d1));

		TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(s, d2));

		result = result && Compare(d1, d2, 0, true, 64);

		return result;
	}

    template <class Func>
    bool ReduceGrayAutoTest(const Func & f1, const Func & f2)
    {
        bool result = true;

        result = result && ReduceGrayAutoTest(W, H, f1, f2);
        result = result && ReduceGrayAutoTest(W + E, H - E, f1, f2);
        result = result && ReduceGrayAutoTest(W - O, H + O, f1, f2);

        return result;
    }



}
