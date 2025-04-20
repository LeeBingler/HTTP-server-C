#!/bin/bash

for t in tests/*.sh; do $t || exit 1; done