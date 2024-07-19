// Three layer neural network
typedef struct
{
    s32 Inputs; // Number of units in the input layer
    s32 Hidden; // Number of units in the hidden layer
    s32 Outputs; // Number of units in the output layer
    matrix HiddenWeights; // Weights from the input to the hidden layer
    matrix OutputWeights; // Weights from the hidden to the output layer
    f32 LearningRate;
} network;

f32
Sigmoid (s32 RowIndex, s32 ColumnIndex, f32 Z)
{
    (void)RowIndex;
    (void)ColumnIndex;
    return 1.0f / (1.0f + expf(-1.0f * Z));
}

f32
Prime(s32 Row, s32 Column, f32 V)
{
    (void)Row;
    (void)Column;
    return V * (1.0f - V);
}

network
InitNetwork(s32 Inputs, s32 Hidden, s32 Outputs, f32 LearningRate)
{
    // Initialize weights with random data
    matrix HiddenWeights = InitMatrix(Hidden, Inputs, sizeof(f32), RandomFloatArray(Hidden * Inputs, (f32)Inputs));
    matrix OutputWeights = InitMatrix(Outputs, Hidden, sizeof(f32), RandomFloatArray(Outputs * Hidden, (f32)Hidden));
    network Network;
    Network.Inputs = Inputs;
    Network.Hidden = Hidden;
    Network.Outputs = Outputs;
    Network.LearningRate = LearningRate;
    Network.HiddenWeights = HiddenWeights;
    Network.OutputWeights = OutputWeights;
    return Network;
}

matrix
NetworkPredict(network Network, f32 *InputData, s32 InputDataSize)
{
    matrix Inputs = InitMatrix(InputDataSize, 1, sizeof(f32), InputData);
    
    // Layer 1
    matrix HiddenInputs = MatrixDotFloat(Network.HiddenWeights, Inputs);
    matrix HiddenOutputs = MatrixApplyFunctionFloat(HiddenInputs, Sigmoid);
    
    // Layer 2
    matrix FinalInputs = MatrixDotFloat(Network.OutputWeights, HiddenOutputs);
    matrix FinalOutputs = MatrixApplyFunctionFloat(FinalInputs, Sigmoid);
    return FinalOutputs;
}

void
NetworkTrain(network *Network, f32 *InputData, s32 InputDataSize, f32 *TargetData, s32 TargetDataSize)
{
    matrix Inputs = InitMatrix(InputDataSize, 1, sizeof(f32), InputData);
    
    // Forward propagation Layer 1
    matrix HiddenInputs = MatrixDotFloat(Network->HiddenWeights, Inputs);
    matrix HiddenOutputs = MatrixApplyFunctionFloat(HiddenInputs, Sigmoid);
    
    // Forward propagation Layer 2
    matrix FinalInputs = MatrixDotFloat(Network->OutputWeights, HiddenOutputs);
    matrix FinalOutputs = MatrixApplyFunctionFloat(FinalInputs, Sigmoid);
    
    // Errors
    matrix Targets = InitMatrix(TargetDataSize, 1, sizeof(f32), TargetData);
    matrix OutputErrors = MatrixSubtractFloat(Targets, FinalOutputs);
    matrix HiddenErrors = MatrixDotFloat(MatrixTransposeFloat(Network->OutputWeights), OutputErrors);
    
    // Backpropagation
    matrix NewOutputWeights, NewHiddenWeights;
    {
        matrix Interm1 = MatrixMultiplyFloat(OutputErrors, MatrixApplyFunctionFloat(FinalOutputs, Prime));
        matrix Interm2 = MatrixDotFloat(Interm1, MatrixTransposeFloat(HiddenOutputs));
        matrix Interm3 = MatrixScaleFloat(Interm2, Network->LearningRate);
        NewOutputWeights = MatrixAddFloat(Network->OutputWeights, Interm3);
    }
    {
        matrix Interm1 = MatrixMultiplyFloat(HiddenErrors, MatrixApplyFunctionFloat(HiddenOutputs, Prime));
        matrix Interm2 = MatrixDotFloat(Interm1, MatrixTransposeFloat(Inputs));
        matrix Interm3 = MatrixScaleFloat(Interm2, Network->LearningRate);
        NewHiddenWeights = MatrixAddFloat(Network->HiddenWeights, Interm3);
    }
    // Free the previous weight matrices
    DeinitMatrix(Network->OutputWeights);
    DeinitMatrix(Network->HiddenWeights);
    // The final weights are not allocated in the temporary arena!
    SetAllocator(DEFAULT);
    Network->OutputWeights = MatrixCopyData(NewOutputWeights);
    Network->HiddenWeights = MatrixCopyData(NewHiddenWeights);
    SetAllocator(TEMPORARY_ARENA);
}

void
NetworkSaveWeights(network Network)
{
    SerializeMatrixToFile("hidden.weights", Network.HiddenWeights);
    SerializeMatrixToFile("output.weights", Network.OutputWeights);
}

void
NetworkLoadWeights(network *Network)
{
    DeserializeMatrixFromFile("hidden.weights", &Network->HiddenWeights);
    DeserializeMatrixFromFile("output.weights", &Network->OutputWeights);
}