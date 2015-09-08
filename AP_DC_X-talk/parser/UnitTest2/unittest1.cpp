#include "stdafx.h"
#include "CppUnitTest.h"
#include "RootFit.h"
#include "Math/MinimizerOptions.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
//using namespace std;

namespace UnitTest2
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			// TODO: Your test code here
			Assert::AreEqual(3.0, 3.0);
		}

		// 2/3 (quadratic/cubic)
		TEST_METHOD(C_i_s)
		{
			// TODO: Your test code here
			RootFit::CalculateFilterCoeff(5);
			Assert::AreEqual(5, int(RootFit::C_i_s.size()));

			Assert::AreEqual(-3.0 / 35, RootFit::C_i_s[0]);
			Assert::AreEqual(12.0 / 35, RootFit::C_i_s[1]);
			Assert::AreEqual(17.0 / 35, RootFit::C_i_s[2]);
			Assert::AreEqual(12.0 / 35, RootFit::C_i_s[3]);
			Assert::AreEqual(-3.0 / 35, RootFit::C_i_s[4]);

			RootFit::CalculateFilterCoeff(7);
			Assert::AreEqual(7, int(RootFit::C_i_s.size()));

			Assert::AreEqual(-2.0 / 21, RootFit::C_i_s[0]);
			Assert::AreEqual(3.0 / 21, RootFit::C_i_s[1]);
			Assert::AreEqual(6.0 / 21, RootFit::C_i_s[2]);
			Assert::AreEqual(7.0 / 21, RootFit::C_i_s[3]);
			Assert::AreEqual(6.0 / 21, RootFit::C_i_s[4]);
			Assert::AreEqual(3.0 / 21, RootFit::C_i_s[5]);
			Assert::AreEqual(-2.0 / 21, RootFit::C_i_s[6]);

			RootFit::CalculateFilterCoeff(9);
			Assert::AreEqual(9, int(RootFit::C_i_s.size()));

			Assert::AreEqual(-21.0 / 231, RootFit::C_i_s[0]);
			Assert::AreEqual(14.0 / 231, RootFit::C_i_s[1]);
			Assert::AreEqual(39.0 / 231, RootFit::C_i_s[2]);
			Assert::AreEqual(54.0 / 231, RootFit::C_i_s[3]);
			Assert::AreEqual(59.0 / 231, RootFit::C_i_s[4]);
			Assert::AreEqual(54.0 / 231, RootFit::C_i_s[5]);
			Assert::AreEqual(39.0 / 231, RootFit::C_i_s[6]);
			Assert::AreEqual(14.0 / 231, RootFit::C_i_s[7]);
			Assert::AreEqual(-21.0 / 231, RootFit::C_i_s[8]);
		}
		
		
		// 3 / 4 (cubic / quartic)
		TEST_METHOD(C_i_der)
		{
			// TODO: Your test code here
			RootFit::CalculateFilterCoeff(5);
			Assert::AreEqual(5, int(RootFit::C_i_der.size()));

			Assert::AreEqual(1.0 / 12 , RootFit::C_i_der[0]);
			Assert::AreEqual(-8.0 / 12, RootFit::C_i_der[1]);
			Assert::AreEqual(0.0, RootFit::C_i_der[2]);
			Assert::AreEqual(8.0 / 12, RootFit::C_i_der[3]);
			Assert::AreEqual(-1.0 / 12, RootFit::C_i_der[4]);

			

			RootFit::CalculateFilterCoeff(7);
			Assert::AreEqual(7, int(RootFit::C_i_der.size()));
			Assert::AreEqual(22.0 / 252, RootFit::C_i_der[0]);
			Assert::AreEqual(-67.0 / 252, RootFit::C_i_der[1]);
			Assert::AreEqual(-58.0 / 252, RootFit::C_i_der[2]);
			Assert::AreEqual(0.0 / 252, RootFit::C_i_der[3]);
			Assert::AreEqual(58.0 / 252, RootFit::C_i_der[4]);
			Assert::AreEqual(67.0 / 252, RootFit::C_i_der[5]);
			Assert::AreEqual(-22.0 / 252, RootFit::C_i_der[6]);

			RootFit::CalculateFilterCoeff(9);
			Assert::AreEqual(9, int(RootFit::C_i_der.size()));
			Assert::AreEqual(86.0 / 1188, RootFit::C_i_der[0]);
			Assert::AreEqual(-142.0 / 1188, RootFit::C_i_der[1]);
			Assert::AreEqual(-193.0 / 1188, RootFit::C_i_der[2]);
			Assert::AreEqual(-126.0 / 1188, RootFit::C_i_der[3]);
			Assert::AreEqual(0.0 / 1188, RootFit::C_i_der[4]);
			Assert::AreEqual(126.0 / 1188, RootFit::C_i_der[5]);
			Assert::AreEqual(193.0 / 1188, RootFit::C_i_der[6]);
			Assert::AreEqual(142.0 / 1188, RootFit::C_i_der[7]);
			Assert::AreEqual(-86.0 / 1188, RootFit::C_i_der[8]);
		}

		// 2/3 (quadratic/cubic)
		TEST_METHOD(C_i_der2)
		{
			// TODO: Your test code here
			RootFit::CalculateFilterCoeff(5);
			Assert::AreEqual(5, int(RootFit::C_i_der2.size()));

			Assert::AreEqual(2.0 / 7, RootFit::C_i_der2[0]);
			Assert::AreEqual(-1.0 / 7, RootFit::C_i_der2[1]);
			Assert::AreEqual(-2.0 / 7, RootFit::C_i_der2[2]);
			Assert::AreEqual(-1.0 / 7, RootFit::C_i_der2[3]);
			Assert::AreEqual(2.0 / 7, RootFit::C_i_der2[4]);

			RootFit::CalculateFilterCoeff(7);
			Assert::AreEqual(7, int(RootFit::C_i_der2.size()));

			Assert::AreEqual(5.0 / 42, RootFit::C_i_der2[0]);
			Assert::AreEqual(0.0 / 42, RootFit::C_i_der2[1]);
			Assert::AreEqual(-3.0 / 42, RootFit::C_i_der2[2]);
			Assert::AreEqual(-4.0 / 42, RootFit::C_i_der2[3]);
			Assert::AreEqual(-3.0 / 42, RootFit::C_i_der2[4]);
			Assert::AreEqual(0.0 / 42, RootFit::C_i_der2[5]);
			Assert::AreEqual(5.0 / 42, RootFit::C_i_der2[6]);

			RootFit::CalculateFilterCoeff(9);
			Assert::AreEqual(9, int(RootFit::C_i_der2.size()));

			Assert::AreEqual(28.0 / 462, RootFit::C_i_der2[0]);
			Assert::AreEqual(7.0 / 462, RootFit::C_i_der2[1]);
			Assert::AreEqual(-8.0 / 462, RootFit::C_i_der2[2]);
			Assert::AreEqual(-17.0 / 462, RootFit::C_i_der2[3]);
			Assert::AreEqual(-20.0 / 462, RootFit::C_i_der2[4]);
			Assert::AreEqual(-17.0 / 462, RootFit::C_i_der2[5]);
			Assert::AreEqual(-8.0 / 462, RootFit::C_i_der2[6]);
			Assert::AreEqual(7.0 / 462, RootFit::C_i_der2[7]);
			Assert::AreEqual(28.0 / 462, RootFit::C_i_der2[8]);
		}

		TEST_METHOD(CreateFrontGraph)
		{
			// TODO: Your test code here
			RootFit::time_start_index = 0;
			RootFit::time_finish_index = 5;
			
			RootFit::time_front.push_back(2);
			RootFit::time_front.push_back(4);
			RootFit::time_front.push_back(5);
			
			/*RootFit::yv.push_back(0); RootFit::yv.push_back(0); RootFit::yv.push_back(0); RootFit::yv.push_back(0); RootFit::yv.push_back(0);
			RootFit::yv.push_back(1); RootFit::yv.push_back(1); RootFit::yv.push_back(1); RootFit::yv.push_back(1); RootFit::yv.push_back(1);*/

			for (int i = 0; i <= RootFit::time_finish_index; i++)
			{
				RootFit::xv.push_back(i);
				RootFit::yv.push_back(i*i);
			}

			Assert::AreEqual(RootFit::time_finish_index + 1, int(RootFit::xv.size()));
			Assert::AreEqual(RootFit::time_finish_index + 1, int(RootFit::yv.size()));

			RootFit::CreateFrontGraph();

			Assert::AreEqual(RootFit::time_finish_index + 1, int(RootFit::xv_front.size()));
			Assert::AreEqual(RootFit::time_finish_index + 1, int(RootFit::yv_front.size()));

			Assert::AreEqual(0.0, RootFit::yv_front[0]);
			Assert::AreEqual(0.0, RootFit::yv_front[1]);
			Assert::AreEqual(4.0, RootFit::yv_front[2]);
			Assert::AreEqual(0.0, RootFit::yv_front[3]);
			Assert::AreEqual(16.0, RootFit::yv_front[4]);
			Assert::AreEqual(25.0, RootFit::yv_front[5]);
				

		}

		TEST_METHOD(FindStartStop)
		{
			// TODO: Your test code here
			Assert::AreEqual(3.0, 3.0);
		}

	};
}
