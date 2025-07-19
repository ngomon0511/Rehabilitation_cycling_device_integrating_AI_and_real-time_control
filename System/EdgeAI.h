#ifndef EDGE_AI
#define EDGE_AI

#include <TensorFlowLite_ESP32.h>
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include <LittleFS.h>

class EdgeAI {
public:
  EdgeAI();
  void init();
  void setInput(float input[5][7]);  
  void compute();
  int getOutput();             
};

#endif
