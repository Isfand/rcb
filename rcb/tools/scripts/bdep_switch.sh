#!/bin/sh

# Credit: 
# https://github.com/build2/build2/issues/131

# Usage: bdep_switch.sh config switch [@<config>]

if [ "$1" = "config" ] && [ "$2" = "switch" ]; then
	bdep config set --no-default --no-forward --all
	bdep config set --default --forward "$3"
	bdep sync -a
else
	bdep "$@"
fi