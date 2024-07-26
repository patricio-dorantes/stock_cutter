#!/bin/bash



podman run -it -v`pwd`/csv:/app/csv/ \
  --cpus 4 --memory 8G \
  localhost/stock_cutter csv/example
