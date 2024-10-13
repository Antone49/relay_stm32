#!/bin/bash

currentDirectory=$(realpath `dirname $0`)

$currentDirectory/make.sh $1
$currentDirectory/flash.sh
