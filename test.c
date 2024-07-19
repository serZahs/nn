#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "utils.h"
#include "matrix.h"

float Square(int Row, int Column, float Value)
{
    (void)Row;
    (void)Column;
    return Value * Value;
}

void TestApply()
{
    float AA[4] = {
        2.0f, 3.0f, 
        5.0f, 1.0f, 
    };
    matrix A;
    A.ElementSize = sizeof(float);
    A.Rows = 2;
    A.Columns = 2;
    A.Data = AA;
    printf("====== TestApply() ======\n");
    matrix Result = MatrixApplyFunctionFloat(A, Square);
    PrintMatrixFloat(Result);
}

void TestScale()
{
    float AA[4] = {
        2.0f, 3.0f, 
        5.0f, 1.0f, 
    };
    matrix A;
    A.ElementSize = sizeof(float);
    A.Rows = 2;
    A.Columns = 2;
    A.Data = AA;
    printf("====== TestScale() ======\n");
    matrix Result = MatrixScaleFloat(A, 4.0f);
    PrintMatrixFloat(Result);
}

void TestAdd()
{
    float AA[4] = {
        2.0f, 3.0f, 
        5.0f, 1.0f, 
    };
    matrix A;
    A.ElementSize = sizeof(float);
    A.Rows = 2;
    A.Columns = 2;
    A.Data = AA;
    
    float BB[4] = {
        1.0f, 7.0f,
        2.0f, 1.0f,
    };
    matrix B;
    B.ElementSize = sizeof(float);
    B.Rows = 2;
    B.Columns = 2;
    B.Data = BB;
    
    printf("====== TestAdd() ======\n");
    matrix Result = MatrixAddFloat(A, B);
    PrintMatrixFloat(Result);
}

void TestSubtract()
{
    float AA[4] = {
        2.0f, 3.0f, 
        5.0f, 1.0f, 
    };
    matrix A;
    A.ElementSize = sizeof(float);
    A.Rows = 2;
    A.Columns = 2;
    A.Data = AA;
    
    float BB[4] = {
        1.0f, 7.0f,
        2.0f, 1.0f,
    };
    matrix B;
    B.ElementSize = sizeof(float);
    B.Rows = 2;
    B.Columns = 2;
    B.Data = BB;
    
    printf("====== TestSubtract() ======\n");
    matrix Result = MatrixSubtractFloat(A, B);
    PrintMatrixFloat(Result);
}


void TestMultiply()
{
    float AA[4] = {
        2.0f, 3.0f, 
        5.0f, 1.0f, 
    };
    matrix A;
    A.ElementSize = sizeof(float);
    A.Rows = 2;
    A.Columns = 2;
    A.Data = AA;
    
    float BB[4] = {
        1.0f, 7.0f,
        2.0f, 1.0f,
    };
    matrix B;
    B.ElementSize = sizeof(float);
    B.Rows = 2;
    B.Columns = 2;
    B.Data = BB;
    
    printf("====== TestMultiply() ======\n");
    matrix Result = MatrixMultiplyFloat(A, B);
    PrintMatrixFloat(Result);
}

void TestTranspose()
{
    float AA[8] = {
        2.0f, 3.0f, 4.0f, 10.0f,
        5.0f, 1.0f, 2.0f, 20.0f
    };
    matrix A;
    A.ElementSize = sizeof(float);
    A.Rows = 2;
    A.Columns = 4;
    A.Data = AA;
    
    printf("====== TestTranspose() ======\n");
    matrix Transpose = MatrixTransposeFloat(A);
    PrintMatrixFloat(Transpose);
}

int main()
{
    TestScale();
    TestApply();
    TestAdd();
    TestSubtract();
    TestMultiply();
    TestTranspose();
}