#!/bin/sh

#xterm -hold -e ". ./sts_qa.sh sts_v11a sts/sts_v11a.geo $VMCWORKDIR/parameters/sts/sts_v11a.digi.par"&
#xterm -hold -e ". ./sts_qa.sh sts_v11b sts/sts_v11b.geo $VMCWORKDIR/parameters/sts/sts_v11b.digi.par"&
#xterm -hold -e ". ./sts_qa.sh sts_v12a sts/sts_v12a.geo.root $VMCWORKDIR/parameters/sts/sts_v12a_std.digi.par"&
xterm -hold -e ". ./sts_qa.sh sts_v12b sts/sts_v12b.geo.root $VMCWORKDIR/parameters/sts/sts_v12b_std.digi.par ${VMCWORKDIR}/parameters/sts/sts_matbudget_v12b.root 0"&
xterm -hold -e ". ./sts_qa.sh sts_v12b_cons sts/sts_v12b.geo.root $VMCWORKDIR/parameters/sts/sts_v12b_std.digi.par ${VMCWORKDIR}/parameters/sts/sts_matbudget_v12b.root 1"&

