// This code does not check for any overflows or unsafe truncations!

typedef struct
{
    s32 ElementSize;
    s32 Rows;
    s32 Columns;
    void *Data;
} matrix;

typedef struct
{
    s32 Count;
    void *Data;
} matrix_vector;

matrix
InitMatrix(s32 Rows, s32 Columns, s32 ElementSize, void *Data)
{
    matrix Matrix;
    Matrix.ElementSize = ElementSize;
    Matrix.Rows = Rows;
    Matrix.Columns = Columns;
    Matrix.Data = Data;
    return Matrix;
}

// Cannot be used on a matrix allocated in temporary storage!
inline void
DeinitMatrix(matrix Matrix)
{
    free(Matrix.Data);
}

matrix
MatrixCopyData(matrix Source)
{
    matrix Dest;
    Dest.ElementSize = Source.ElementSize;
    Dest.Rows = Source.Rows;
    Dest.Columns = Source.Columns;
    size_t DataSize = Dest.ElementSize * Dest.Rows * Dest.Columns;
    void *Data = Alloc(DataSize);
    memcpy(Data, Source.Data, DataSize);
    Dest.Data = Data;
    return Dest;
}

inline int
MatrixSameShape(matrix A, matrix B)
{
    return ((A.Rows == B.Rows) && (A.Columns == B.Columns) && (A.ElementSize == B.ElementSize));
}

// Invalid if the backing matrix is destroyed!
matrix_vector
MatrixGetRow(matrix Matrix, s32 RowIndex)
{
    matrix_vector Row;
    Row.Count = Matrix.Columns;
    Row.Data = (u8*)(Matrix.Data) + (RowIndex * Matrix.Columns * Matrix.ElementSize);
    return Row;
}

inline u8
MatrixGetValueU8(matrix Matrix, s32 RowIndex, s32 ColumnIndex)
{
    u8* Location = (u8*)(Matrix.Data) + ((RowIndex * Matrix.Columns * Matrix.ElementSize) + (ColumnIndex * Matrix.ElementSize));
    u8 Value = *(Location);
    return Value;
}

inline f32
MatrixGetValueF32(matrix Matrix, s32 RowIndex, s32 ColumnIndex)
{
    u8* Location = (u8*)(Matrix.Data) + ((RowIndex * Matrix.Columns * Matrix.ElementSize) + (ColumnIndex * Matrix.ElementSize));
    f32 Value = *((f32*)Location);
    return Value;
}

matrix
MatrixTransposeFloat(matrix Matrix)
{
    if (Matrix.ElementSize != sizeof(f32))
    {
        Panic("Cannot use MatrixTransposeFloat on matrix with element size %d\n", Matrix.ElementSize);
    }
    f32 *ResultData = (f32*)Alloc(Matrix.Rows * Matrix.Columns * Matrix.ElementSize);
    for (s32 RowIndex = 0; RowIndex < Matrix.Rows; RowIndex++)
    {
        for (s32 ColumnIndex = 0; ColumnIndex < Matrix.Columns; ColumnIndex++)
        {
            f32* Location = ResultData + ((ColumnIndex * Matrix.Rows) + RowIndex);
            f32 Value = MatrixGetValueF32(Matrix, RowIndex, ColumnIndex);
            *Location = Value;
        }
    }
    return InitMatrix(Matrix.Columns, Matrix.Rows, Matrix.ElementSize, ResultData);
}

matrix
MatrixScaleFloat(matrix Matrix, f32 Scalar)
{
    f32 *ResultData = (f32*)Alloc(Matrix.Rows * Matrix.Columns * Matrix.ElementSize);
    s32 DataIndex = 0;
    for (s32 RowIndex = 0; RowIndex < Matrix.Rows; RowIndex++)
    {
        for (s32 ColumnIndex = 0; ColumnIndex < Matrix.Columns; ColumnIndex++)
        {
            f32 Value = MatrixGetValueF32(Matrix, RowIndex, ColumnIndex);
            ResultData[DataIndex++] = Value * Scalar;
        }
    }
    return InitMatrix(Matrix.Rows, Matrix.Columns, Matrix.ElementSize, ResultData);
}

matrix
MatrixAddFloat(matrix A, matrix B)
{
    if (!MatrixSameShape(A, B))
    {
        Panic("Cannot perform addition on matrices (%dx%d, size=%d) and (%dx%d, size=%d)\n", A.Rows, A.Columns, A.ElementSize, B.Rows, B.Columns, B.ElementSize);
    }
    f32 *ResultData = (f32*)Alloc(A.Rows * A.Columns * A.ElementSize);
    s32 ResultIndex = 0;
    for (s32 RowIndex = 0; RowIndex < A.Rows; RowIndex++)
    {
        for (s32 ColumnIndex = 0; ColumnIndex < A.Columns; ColumnIndex++)
        {
            f32 Value = MatrixGetValueF32(A, RowIndex, ColumnIndex) + MatrixGetValueF32(B, RowIndex, ColumnIndex);
            ResultData[ResultIndex++] = Value;
        }
    }
    return InitMatrix(A.Rows, A.Columns, A.ElementSize, ResultData);
}

matrix
MatrixSubtractFloat(matrix A, matrix B)
{
    if (!MatrixSameShape(A, B))
    {
        Panic("Cannot perform subtraction on matrices (%dx%d, size=%d) and (%dx%d, size=%d)\n", A.Rows, A.Columns, A.ElementSize, B.Rows, B.Columns, B.ElementSize);
    }
    f32 *ResultData = (f32*)Alloc(A.Rows * A.Columns * A.ElementSize);
    s32 ResultIndex = 0;
    for (s32 RowIndex = 0; RowIndex < A.Rows; RowIndex++)
    {
        for (s32 ColumnIndex = 0; ColumnIndex < A.Columns; ColumnIndex++)
        {
            f32 Value = MatrixGetValueF32(A, RowIndex, ColumnIndex) - MatrixGetValueF32(B, RowIndex, ColumnIndex);
            ResultData[ResultIndex++] = Value;
        }
    }
    return InitMatrix(A.Rows, A.Columns, A.ElementSize, ResultData);
}

matrix
MatrixMultiplyFloat(matrix A, matrix B)
{
    if (!MatrixSameShape(A, B))
    {
        Panic("Cannot perform multiplication on matrices (%dx%d, size=%d) and (%dx%d, size=%d)\n", A.Rows, A.Columns, A.ElementSize, B.Rows, B.Columns, B.ElementSize);
    }
    f32 *ResultData = (f32*)Alloc(A.Rows * A.Columns * A.ElementSize);
    s32 ResultIndex = 0;
    for (s32 RowIndex = 0; RowIndex < A.Rows; RowIndex++)
    {
        for (s32 ColumnIndex = 0; ColumnIndex < A.Columns; ColumnIndex++)
        {
            f32 Value = MatrixGetValueF32(A, RowIndex, ColumnIndex) * MatrixGetValueF32(B, RowIndex, ColumnIndex);
            ResultData[ResultIndex++] = Value;
        }
    }
    return InitMatrix(A.Rows, A.Columns, A.ElementSize, ResultData);
}

matrix
MatrixDotFloat(matrix A, matrix B)
{
    if (A.Columns != B.Rows)
    {
        Panic("Cannot perform dot product on matrices of dimensions (%dx%d) and (%dx%d)\n", A.Rows, A.Columns, B.Rows, B.Columns);
    }
    if (A.ElementSize != B.ElementSize)
    {
        Panic("Cannot perform dot product on matrices of different element sizes\n");
    }
    f32 *ResultData = (f32*)Alloc(A.Rows * B.Columns * A.ElementSize);
    s32 ResultIndex = 0;
    for (s32 RowIndex = 0; RowIndex < A.Rows; RowIndex++)
    {
        for (s32 ColumnIndex = 0; ColumnIndex < B.Columns; ColumnIndex++)
        {
            f32 Accumulator = 0;
            for (s32 ElementIndex = 0; ElementIndex < A.Columns; ElementIndex++)
            {
                Accumulator += MatrixGetValueF32(A, RowIndex, ElementIndex) * MatrixGetValueF32(B, ElementIndex, ColumnIndex);
            }
            ResultData[ResultIndex++] = Accumulator;
        }
    }
    return InitMatrix(A.Rows, B.Columns, A.ElementSize, ResultData);
}

matrix
MatrixApplyFunctionFloat(matrix Matrix, f32 (*Function)(s32 Row, s32 Column, f32 Value))
{
    f32 *Data = (f32*)Alloc(Matrix.Rows * Matrix.Columns * Matrix.ElementSize);
    s32 DataIndex = 0;
    for (s32 RowIndex = 0; RowIndex < Matrix.Rows; RowIndex++)
    {
        for (s32 ColumnIndex = 0; ColumnIndex < Matrix.Columns; ColumnIndex++)
        {
            u8* Location = (u8*)(Matrix.Data) + ((RowIndex * Matrix.Columns * Matrix.ElementSize) + (ColumnIndex * Matrix.ElementSize));
            f32 Value = *((f32*)Location);
            f32 NewValue = Function(RowIndex, ColumnIndex, Value);
            Data[DataIndex++] = NewValue;
        }
    }
    return InitMatrix(Matrix.Rows, Matrix.Columns, Matrix.ElementSize, Data);
}

void
PrintMatrixVectorU8(matrix_vector Vector)
{
    printf("[ ");
    for (s32 ElemIndex = 0; ElemIndex < Vector.Count; ElemIndex++)
    {
        printf("%u ", *((u8*)Vector.Data + ElemIndex));
    }
    printf("]\n");
}

void
PrintMatrixVectorFloat(matrix_vector Vector)
{
    printf("[ ");
    for (s32 ElemIndex = 0; ElemIndex < Vector.Count; ElemIndex++)
    {
        printf("%f ", *((f32*)Vector.Data + ElemIndex));
    }
    printf("]\n");
}

void
PrintMatrixFloat(matrix Matrix)
{
    printf("[\n");
    for (s32 RowIndex = 0; RowIndex < Matrix.Rows; RowIndex++)
    {
        PrintMatrixVectorFloat(MatrixGetRow(Matrix, RowIndex));
    }
    printf("]\n");
}

int
SerializeMatrixToFile(char *Filename, matrix Matrix)
{
    FILE *File = fopen(Filename, "wb");
    if (!File)
    {
        printf("Error: Could not open file %s\n", Filename);
        return(0);
    }
    fwrite(&Matrix.ElementSize, sizeof(s32), 1, File);
    fwrite(&Matrix.Rows, sizeof(s32), 1, File);
    fwrite(&Matrix.Columns, sizeof(s32), 1, File);
    size_t DataSize = Matrix.ElementSize * Matrix.Rows * Matrix.Columns;
    fwrite(Matrix.Data, DataSize, 1, File);
    fclose(File);
    return(1);
}

int
DeserializeMatrixFromFile(char *Filename, matrix *Matrix)
{
    FILE *File = fopen(Filename, "rb");
    if (!File)
    {
        printf("Error: Could not open file %s\n", Filename);
        return(0);
    }
    fread(&Matrix->ElementSize, sizeof(s32), 1, File);
    fread(&Matrix->Rows, sizeof(s32), 1, File);
    fread(&Matrix->Columns, sizeof(s32), 1, File);
    size_t DataSize = Matrix->ElementSize * Matrix->Rows * Matrix->Columns;
    void *Data = Alloc(DataSize);
    fread(Data, DataSize, 1, File);
    Matrix->Data = Data;
    fclose(File);
    return(1);
}