#!/bin/bash

# NW
# Row 1
./gtiff2r16 -i DSM_BQ31_2013_1000_1140.tif DSM_BQ31_2013_1000_1141.tif DSM_BQ31_2013_1000_1142.tif -o row1.r16 -m stitch_horizontal
# Row 2
./gtiff2r16 -i DSM_BQ31_2013_1000_1240.tif DSM_BQ31_2013_1000_1241.tif DSM_BQ31_2013_1000_1242.tif -o row2.r16 -m stitch_horizontal
# Stitch vertically
./stitchr16 -i row1.r16 row2.r16 -single_w 1440 -single_h 720 -o row1-2.r16 -m stitch_vertical
# Subdivide into 2x2
./cutr16 -i row1-2.r16 -o final -filename_base_row 1 -filename_base_col 1 -input_w 1440 -input_h 1440 -div_h 2 -div_v 2

# SW
# Row 3
./gtiff2r16 -i DSM_BQ31_2013_1000_1340.tif DSM_BQ31_2013_1000_1341.tif DSM_BQ31_2013_1000_1342.tif -o row1.r16 -m stitch_horizontal
# Row 4
./gtiff2r16 -i DSM_BQ31_2013_1000_1440.tif DSM_BQ31_2013_1000_1441.tif DSM_BQ31_2013_1000_1442.tif -o row2.r16 -m stitch_horizontal
# Stitch vertically
./stitchr16 -i row1.r16 row2.r16 -single_w 1440 -single_h 720 -o row1-2.r16 -m stitch_vertical
# Subdivide into 2x2
./cutr16 -i row1-2.r16 -o final -filename_base_row 3 -filename_base_col 1 -input_w 1440 -input_h 1440 -div_h 2 -div_v 2


# NE
# Row 1
./gtiff2r16 -i DSM_BQ31_2013_1000_1143.tif DSM_BQ31_2013_1000_1144.tif DSM_BQ31_2013_1000_1145.tif -o row1.r16 -m stitch_horizontal
# Row 2
./gtiff2r16 -i DSM_BQ31_2013_1000_1243.tif DSM_BQ31_2013_1000_1244.tif DSM_BQ31_2013_1000_1245.tif -o row2.r16 -m stitch_horizontal
# Stitch vertically
./stitchr16 -i row1.r16 row2.r16 -single_w 1440 -single_h 720 -o row1-2.r16 -m stitch_vertical
# Subdivide into 2x2
./cutr16 -i row1-2.r16 -o final -filename_base_row 1 -filename_base_col 3 -input_w 1440 -input_h 1440 -div_h 2 -div_v 2

# SE
# Row 3
./gtiff2r16 -i DSM_BQ31_2013_1000_1343.tif DSM_BQ31_2013_1000_1344.tif DSM_BQ31_2013_1000_1345.tif -o row1.r16 -m stitch_horizontal
# Row 4
./gtiff2r16 -i DSM_BQ31_2013_1000_1443.tif -o row2.r16 -m stitch_horizontal  -padding_w 1440 
# Stitch vertically
./stitchr16 -i row1.r16 row2.r16 -single_w 1440 -single_h 720 -o row1-2.r16 -m stitch_vertical
# Subdivide into 2x2
./cutr16 -i row1-2.r16 -o final -filename_base_row 3 -filename_base_col 3 -input_w 1440 -input_h 1440 -div_h 2 -div_v 2

rm row1.r16 row2.r16 row1-2.r16