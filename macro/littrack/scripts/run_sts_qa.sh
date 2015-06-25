#!/bin/sh

xterm -hold -e ". ./sts_qa.sh sts_v13d_real sts/sts_v13d.geo.root $VMCWORKDIR/parameters/sts/sts_v13d_std.digi.par ${VMCWORKDIR}/parameters/sts/sts_matbudget_v13d.root real"&
xterm -hold -e ". ./sts_qa.sh sts_v13d_new sts/sts_v13d.geo.root $VMCWORKDIR/parameters/sts/sts_v13d_std.digi.par ${VMCWORKDIR}/parameters/sts/sts_matbudget_v13d.root new"&
