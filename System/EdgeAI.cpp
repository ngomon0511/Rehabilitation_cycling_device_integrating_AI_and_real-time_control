#include "EdgeAI.h"

namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* model_input = nullptr;
TfLiteTensor* model_output = nullptr;
constexpr int kTensorArenaSize = 25 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
}

const float X_mean[7] = { 3.5166686, -8.323809, -0.0643552, -4.6484003, -8.0805235, 0.33645952, 2.8574674 };
const float X_std[7] = { 2.9490125, 1.366511, 17.316149, 2.4842374, 1.6089948, 17.117037, 1.7561004 };

EdgeAI::EdgeAI() {}

void EdgeAI::init() {
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  File modelFile = LittleFS.open("/model.tflite", "r");
  if (!modelFile || modelFile.isDirectory()) {
    error_reporter->Report("Failed to open model file");
    while (1)
      ;
  }

  size_t modelSize = modelFile.size();
  uint8_t* modelData = (uint8_t*)malloc(modelSize);
  if (!modelData) {
    error_reporter->Report("Failed to allocate memory for model");
    while (1)
      ;
  }
  modelFile.read(modelData, modelSize);
  modelFile.close();

  model = tflite::GetModel(modelData);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report("Model version mismatch");
    while (1)
      ;
  }

  static tflite::MicroMutableOpResolver<15> micro_op_resolver;
  micro_op_resolver.AddShape();
  micro_op_resolver.AddStridedSlice();
  micro_op_resolver.AddTranspose();
  micro_op_resolver.AddUnpack();
  micro_op_resolver.AddPack();
  micro_op_resolver.AddFill();
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddAdd();
  micro_op_resolver.AddMul();
  micro_op_resolver.AddSplit();
  micro_op_resolver.AddLogistic();
  micro_op_resolver.AddTanh();
  micro_op_resolver.AddSoftmax();
  micro_op_resolver.AddUnidirectionalSequenceLSTM();
  micro_op_resolver.AddRelu();

  static tflite::MicroInterpreter static_interpreter(
    model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    error_reporter->Report("Failed to allocate tensors");
    while (1)
      ;
  }

  model_input = interpreter->input(0);
  model_output = interpreter->output(0);
}

void EdgeAI::setInput(float input[5][7]) {
  for (int t = 0; t < 5; t++) {
    for (int f = 0; f < 7; f++) {
      float norm_val = (input[t][f] - X_mean[f]) / X_std[f];
      model_input->data.f[t * 7 + f] = norm_val;
    }
  }
}

void EdgeAI::compute() {
  if (interpreter->Invoke() != kTfLiteOk) {
    error_reporter->Report("Model invoke failed");
  }
}

int EdgeAI::getOutput() {
  float max_val = model_output->data.f[0];
  int max_index = 0;
  for (int i = 1; i < 4; i++) {
    if (model_output->data.f[i] > max_val) {
      max_val = model_output->data.f[i];
      max_index = i;
    }
  }
  return max_index + 1;
}
