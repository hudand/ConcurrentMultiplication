// ConcurrentMul.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <ctime>
#include <mutex>
#include <atomic>
#include <execution>
#include <chrono>
#include <fstream>

#pragma warning(push)
#pragma warning(disable:4996)
#include <boost/numeric/ublas/matrix.hpp>
#pragma warning(pop)

struct Element {
	double data;
	size_t row;
	size_t col;
};

using MatrixElement = Element;
using Matrix = boost::numeric::ublas::matrix<MatrixElement>;

using std::chrono::duration_cast;
using std::chrono::duration;
using std::milli;

std::atomic_uint progress = 0;
std::mutex mulMutex;

void Mul(const Matrix& A, const Matrix& B, Matrix& C) {
	
	auto oldTime = std::chrono::high_resolution_clock::now();
	
	size_t size = C.size1();				// matrix size

	std::for_each(std::execution::par, C.begin1().begin(), (C.end1() - 1).end(),
	[&](Element& value)
	{
		for (size_t i = 0; i < size; ++i)
			value.data += A(value.row, i).data * B(i, value.col).data;
		
		progress++;
		auto newTime = std::chrono::high_resolution_clock::now();

		if (duration_cast<duration<double, milli>>(newTime - oldTime).count() >= 20)
		{
			std::lock_guard<std::mutex> guard(mulMutex);
			if (duration_cast<duration<double, milli>>(newTime - oldTime).count() >= 20)
			{
				oldTime = newTime;
				std::cout << static_cast<int>(round(100.0 * progress / size / size)) << "% completed\r";
			}
		}
	});
	std::cout << "100% completed";
}

void RandomFillMatrix(Matrix& A) {
	const int a = -10;
	const int b = 10;
	int size = A.size1();

	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
		{
			A(i, j).data = a + rand() % (b - a + 1);
			A(i, j).row = i;
			A(i, j).col = j;
		}
}

void FillMatrix(Matrix& A, double number) {
	const int a = -10;
	const int b = 10;
	int size = A.size1();

	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
		{
			A(i, j).data = number;
			A(i, j).row = i;
			A(i, j).col = j;
		}
}

//void PrintMatrix(std::ostream& out, Matrix& A, const std::string& matrixCaption) {
//	int size = A.size1();
//	out << "Matrix " << matrixCaption << ":" << std::endl;
//	for (int i = 0; i < size; i++) {
//		for (int j = 0; j < size; j++)
//			out << A(i, j).data << " ";
//		out << std::endl;
//	}
//}


int main()
{
	size_t size = 500;
	Matrix A(size, size);
	Matrix B(size, size);
	Matrix C(size, size);
	
	srand(static_cast<unsigned int>(time(0)));

	std::cout << "Filling matrices A and B" << std::endl << std::endl;
	RandomFillMatrix(A);
	RandomFillMatrix(B);

	std::ofstream out{ "results.dat" };

	/*std::cout << "Output matrices A and B to the file" << std::endl << std::endl;
	PrintMatrix(out, A, "A");
	PrintMatrix(out, B, "B");*/

	std::cout << "Filling the matrix C with zero" << std::endl << std::endl;
	FillMatrix(C, 0);

	std::cout << "Matrix product calculation started" << std::endl << std::endl;
	Mul(A, B, C);

	/*PrintMatrix(out, C, "C");
	std::cout << "Output of matrix C to file is complete" << std::endl << std::endl;*/

}



