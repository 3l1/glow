/**
 * Copyright (c) 2017-present, Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef GLOW_IMPORTER_ONNX_H
#define GLOW_IMPORTER_ONNX_H

#include "glow/Graph/Graph.h"

#include "llvm/ADT/ArrayRef.h"

#include <string>
#include <unordered_map>

namespace onnx {
class NodeProto;
class GraphProto;
} // namespace onnx

namespace glow {

class Tensor;
class Value;

/// Loads ONNX models.
class ONNXModelLoader {
  /// The graph that we are constructing.
  Function &G_;
  /// Saves network nodes by name.
  std::unordered_map<std::string, Node *> nodeByName_;
  /// A list of weight tensors indexed by name.
  std::unordered_map<std::string, Tensor *> tensors_;
  /// The external output of the network.
  SaveNode *root_{nullptr};

  /// Load the network operators from the GraphProto.
  void loadNetwork(onnx::GraphProto &net);

  /// Load the network initializers from the GraphProto.
  void loadInitializers(onnx::GraphProto &net);

  /// \returns the tensor that was registered under the name \p name.
  Tensor *getTensorByName(const std::string &name);

  /// Load the operator \p op into the network. This creates one or more nodes
  /// in the network.
  void loadOperator(const onnx::NodeProto &op);

  /// Reads a network (weights or structure) from the serialized protocol buffer
  /// file.
  bool loadProtoFile(onnx::GraphProto &net, const std::string &filename);

public:
  /// \returns the node that was registered with the name \p name.
  Node *getNodeByName(const std::string &name);

  /// \returns the node that was registered with the name \p name or create a
  /// new Variable node for a tensor with this name.
  Node *getOrCreateNodeByName(const std::string &name);

  /// \returns True if the node that's registered using \p name exists.
  bool hasNodeByName(const std::string &name) const;

  /// Loads the ONNX model that's represented by a model description file,
  /// serialized in \p modelDescFilename and populates the network into \p G.
  /// The tensors in \p tensors are stored with the names in the list of names
  /// \p names and used as inputs to the network.
  ONNXModelLoader(const std::string &modelDescFilename,
                  llvm::ArrayRef<const char *> names,
                  llvm::ArrayRef<Tensor *> tensors, Function &G);

  ~ONNXModelLoader();

  /// \returns the output of the network. This is usually the result of the last
  /// softmax or regression layer.
  SaveNode *getRoot() { return root_; }
};

} // namespace glow

#endif // GLOW_IMPORTER_ONNX_H
