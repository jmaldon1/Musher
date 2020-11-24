#!/bin/bash

# Use an older numpy version of mac to avoid `Buggy Accelerate Backend` error.
if [[ '$RUNNER_OS' == 'macOS' ]]; then 
    pip install numpy==1.18.0;
fi

pip install -r dev_requirements.txt
