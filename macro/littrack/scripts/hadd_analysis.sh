#!/bin/sh

DIR=/lustre/cbm/user/andrey/events/nu/25gev/

OUTPUTFILE=$DIR/analysis.all.root
INPUTFILES=$DIR/analysis.0*.root

hadd  -T -f $OUTPUTFILE $INPUTFILES
