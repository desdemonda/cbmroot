# Remarks:
# 1. Absorber and station positions are defined relative to MuchCave Zin
#
# 2. When defining distances between absorbers and stations keep in mind 
#    the following technological restrictions:
#    a) distance from layer center to absorber >=   5 cm 
#    b) distance between layer centers         >=  10 cm 
#
# 3. Absorber materials currently implemented:
#    I - MUCHiron
#    L - MUCHlead
#    W - MUCHwolfram
#    C - MUCHcarbon
#
# 4. There is a possibility to switch off detailed module design 
#    (Use module design = 0). One large module per layer is created 
#    in this case (at the front side of each layer).
# 
# 5. If detector type = 2 (straws), support will not be created. 

# General information
MuchCave Zin position [cm]  :     120
Acceptance tangent min      :     0.1
Acceptance tangent max      :     0.5
Number of absorbers         :       6
Number of stations          :       6

# Absorber specification
Absorber Zin position [cm]  :       0      50      100    150     210     275
Absorber thickness [cm]     :      20      20      20      30      35     100
Absorber material           :       I       I       I       I       I       I

# Station specification
Station Zceneter [cm]       :      35      85     135     195     260     390
Number of layers            :       3       3       3       3       3       3
Detector type               :       3       3       3       3       3       3
Distance between layers [cm]:      10      10      10      10      10      10
Support thickness [cm]      :     1.5     1.5     1.5     1.5     1.5     1.5
Use module design (0/1)     :       1       1       1       1       1       1

# GEM module specification (type 1)
Active volume lx [cm]       :    25.6
Active volume ly [cm]       :    25.6
Active volume lz [cm]       :     0.4
Spacer lx [cm]              :     0.5
Spacer ly [cm]              :       5
Overlap along y axis [cm]   :       2

# Straw module specification (type 2)
Straw thickness [cm]        :     0.4

# Sector-type GEM module specification (type 3)
Number of sectors/layer     :      14      18      22      26      32      42
Active volume lz [cm]       :     0.3
Spacer in r [cm]            :       2
Spacer in phi [cm]          :       2
Overlap in r [cm]           :       2
