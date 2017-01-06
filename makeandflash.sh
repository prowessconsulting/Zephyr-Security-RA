#!/bin/bash
## make -C $1 pristine 
make -C $1 
make -C $1 flash
