
							Claudia Höhne
							GSI, 28 Oct 2005


====================================================
How to create new RICH geometry files for studies on 
the RICH layout optimization:
====================================================

--> See presentation at the Simulation Workgroup meeting, October 28, 2005 

	"Geometry of the RICH detector in cbmroot2"
	http://www.gsi.de/documents/DOC-2005-Nov-4-2.pdf

The macro and an example RICH geometry file (input and output) can be found 
in the "geometry" folder of cbmroot2. The input file (*.dat) contains basic 
geometry parameters of the RICH detector from which the macro calculates the
*.geo file to be used in CBM detector simulations.

macro: cbmroot2/geometry/create_RICH_geo_file.C
inputfile: cbmroot2/geometry/rich-L2900-N2-angleM0-angleD0.dat
outputfile: cbmroot2/geometry/rich-L2900-N2-angleM0-angleD0.geo

Use in root like:
root> .L create_RICH_geo_file.C
root> create_RICH_geo_file("rich-L2900-N2-angleM0-angleD0")

attention:
A change in the RICH detector position and length requires a change in the pipe.geo 
file because in order to avoid overlaps the pipe is implemented in the RICH geo-file 
and cut out in the pipe geo-file. A warning and the required lines for the pipe.geo 
file is given out by the macro.

In case of problems please feel free to ask.

-- 
Dr. Claudia Höhne
Gesellschaft fuer Schwerionenforschung (GSI)
mail  c.hoehne@gsi.de


