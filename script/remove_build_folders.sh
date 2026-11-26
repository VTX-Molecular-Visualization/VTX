#!/bin/bash
cd "$(dirname "$0")/.."
find . -type d -name build -exec rm -rf {} +
