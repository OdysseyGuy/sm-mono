#!/bin/bash

# Generate python code from proto files
echo "Generating protos..."
python -m grpc_tools.protoc -Igenerated=./protos    \
    --python_out=. --grpc_python_out=. --pyi_out=.  \
    ./protos/*.proto
