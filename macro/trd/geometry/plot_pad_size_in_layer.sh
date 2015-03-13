## plot only 1 pad size per module, no rows in sector
# root -l -b -q plot_pad_size_in_layer.C++\(\"trd.v13/trd_v13p_3e.digi.par\"\)
#
## plot only 1 pad size per module, no rows in sector, only 1st layer of each station
#root -l -b -q plot_pad_size_in_layer.C++\(\"trd.v13/trd_v13p_3e.digi.par\",1,0,0\)
root -l -b -q plot_pad_size_in_layer.C++\(\"trd.v14/trd_v14x_3e.digi.par\",1,0,0\)
#
## plot all 3 pad sizes per module, rows in sector, all layers
# root -l -b -q plot_pad_size_in_layer.C++\(\"trd.v13/trd_v13p_3e.digi.par\",3,1,1\)
#
## plot all 3 pad sizes per module, rows in sector, only 1st layer of each station
#root -l -b -q plot_pad_size_in_layer.C++\(\"trd.v13/trd_v13p_3e.digi.par\",3,1,0\)
