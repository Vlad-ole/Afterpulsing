#include "stdafx.h"
#include "CppUnitTest.h"
#include "RootFit.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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
		}

	};
}
