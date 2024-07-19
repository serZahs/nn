#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <float.h>
#include <math.h>
#include <time.h>

#include "utils.h"
#include "matrix.h"
#include "nn.h"

//#define TRAIN

int
GetBestOutput(matrix OutputPredictions)
{
    f32 HighestPrediction = -1.0f; // Prediction values range from 0.0 to 1.0
    int HighestPredictionOutput = 0;
    for (int OutputIndex = 0; OutputIndex < 10; OutputIndex++)
    {
        f32 PredictionValue = MatrixGetValueF32(OutputPredictions, OutputIndex, 0);
        if (PredictionValue > HighestPrediction)
        {
            HighestPrediction = PredictionValue;
            HighestPredictionOutput = OutputIndex;
        }
    }
    return HighestPredictionOutput;
}

int main()
{
    // Seed random generator
    srand((unsigned int)time(NULL));
    
    u32 ImageCount, RowCount, ColumnCount;
    f32 *ImageData;
    if (!ReadMNISTImageFile("mnist-images.training", &ImageCount, &RowCount, &ColumnCount, &ImageData))
    {
        return(1);
    }
    
    u32 LabelCount;
    u8 *LabelData;
    if (!ReadMNISTLabelFile("mnist-labels.training", &LabelCount, &LabelData))
    {
        return(1);
    }
    
    // 60000 x (28*28) Matrix (f32)
    matrix TrainingImages = InitMatrix(ImageCount, RowCount * ColumnCount, sizeof(f32), ImageData);
    // 60000 x 1 Matrix (u8)
    matrix TrainingLabels = InitMatrix(LabelCount, 1, sizeof(u8), LabelData);
    
    network Network = InitNetwork(RowCount * ColumnCount, // Input units: the number of pixels per image
                                  200, // Hidden units: arbitrary
                                  10, // Output units: output values range from 0 to 9
                                  0.1f // Learning rate
                                  );
    
#ifdef TRAIN
    InitTemporaryArena();
    SetAllocator(TEMPORARY_ARENA);
    
    printf("-- STARTING TRAINING --\n");
    s32 NumExamples = 4000;//ImageCount;
    for (s32 ExampleIndex = 0; ExampleIndex < NumExamples; ExampleIndex++)
    {
        printf("Training on example %d out of %d\n", ExampleIndex+1, NumExamples);
        matrix_vector Input = MatrixGetRow(TrainingImages, ExampleIndex);
        u8 Output = MatrixGetValueU8(TrainingLabels, ExampleIndex, 0);
        f32 Targets[10] = {0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f};
        Targets[Output] = 0.99f;
        
        ClearTemporaryArena();
        NetworkTrain(&Network, Input.Data, Input.Count, Targets, 10);
    }
    printf("-- FINISHED TRAINING --\n");
    
    DeinitTemporaryArena();
    SetAllocator(DEFAULT);
    NetworkSaveWeights(Network);
#else
    NetworkLoadWeights(&Network);
#endif
    
    // Make a prediction using the network
    int ExampleIndex = 2;
    matrix_vector ExampleInput = MatrixGetRow(TrainingImages, ExampleIndex);
    u8 ExampleOutput = MatrixGetValueU8(TrainingLabels, ExampleIndex, 0);
    matrix OutputPredictions = NetworkPredict(Network, ExampleInput.Data, ExampleInput.Count);
    PrintMatrixFloat(OutputPredictions);
    int BestOutput = GetBestOutput(OutputPredictions);
    f32 BestOutputPrediction = MatrixGetValueF32(OutputPredictions, BestOutput, 0);
    printf("Actual: %u, Prediction : %u (Probability: %f)\n", ExampleOutput, BestOutput, BestOutputPrediction); 
}