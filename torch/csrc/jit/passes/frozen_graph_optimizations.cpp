#include <torch/csrc/jit/jit_log.h>
#include <torch/csrc/jit/ir/alias_analysis.h>
#include <torch/csrc/jit/ir/ir_views.h>
#include <torch/csrc/jit/passes/frozen_conv_folding.h>
#include <torch/csrc/jit/passes/frozen_conv_relu_fusion.h>
#include <torch/csrc/jit/passes/frozen_graph_optimizations.h>
#include <torch/csrc/jit/passes/remove_dropout.h>
#include <torch/csrc/jit/passes/remove_mutation.h>
#include <torch/csrc/jit/runtime/graph_executor.h>
#include <torch/csrc/utils/memory.h>

namespace torch {
namespace jit {

void OptimizeFrozenGraph(
    std::shared_ptr<Graph>& graph,
    bool optimize_numerics) {
  removeDropout(graph);
  RemoveTensorMutation(graph);

  // run a couple times to capture Conv -> Mul -> Add etc
  if (optimize_numerics) {
    for (size_t i = 0; i < 2; i++) {
      FoldFrozenConvBatchnorm(graph);
      FoldFrozenConvAddOrSub(graph);
      FoldFrozenConvMulOrDiv(graph);
    }
  }

  FuseFrozenConvRelu(graph);
}

} // namespace jit
} // namespace torch
