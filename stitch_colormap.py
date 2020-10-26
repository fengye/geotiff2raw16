#!/usr/bin/env python3

import numpy as np
import PIL
from PIL import Image

list_list_im = [
	['Aerial_temp/BQ31_500_021079.jpg', 'Aerial_temp/BQ31_500_021080.jpg', 'Aerial_temp/BQ31_500_021081.jpg', 'Aerial_temp/BQ31_500_021082.jpg', 'Aerial_temp/BQ31_500_021083.jpg', 'Aerial_temp/BQ31_500_021084.jpg', 'Aerial_temp/BQ31_500_021085.jpg', 'Aerial_temp/BQ31_500_021086.jpg', 'Aerial_temp/BQ31_500_021087.jpg', 'Aerial_temp/BQ31_500_021088.jpg', 'Aerial_temp/BQ31_500_021089.jpg', 'Aerial_temp/BQ31_500_021090.jpg'],
	['Aerial_temp/BQ31_500_022079.jpg', 'Aerial_temp/BQ31_500_022080.jpg', 'Aerial_temp/BQ31_500_022081.jpg', 'Aerial_temp/BQ31_500_022082.jpg', 'Aerial_temp/BQ31_500_022083.jpg', 'Aerial_temp/BQ31_500_022084.jpg', 'Aerial_temp/BQ31_500_022085.jpg', 'Aerial_temp/BQ31_500_022086.jpg', 'Aerial_temp/BQ31_500_022087.jpg', 'Aerial_temp/BQ31_500_022088.jpg', 'Aerial_temp/BQ31_500_022089.jpg', 'Aerial_temp/BQ31_500_022090.jpg'], 
	['Aerial_temp/BQ31_500_023079.jpg', 'Aerial_temp/BQ31_500_023080.jpg', 'Aerial_temp/BQ31_500_023081.jpg', 'Aerial_temp/BQ31_500_023082.jpg', 'Aerial_temp/BQ31_500_023083.jpg', 'Aerial_temp/BQ31_500_023084.jpg', 'Aerial_temp/BQ31_500_023085.jpg', 'Aerial_temp/BQ31_500_023086.jpg', 'Aerial_temp/BQ31_500_023087.jpg', 'Aerial_temp/BQ31_500_023088.jpg', 'Aerial_temp/BQ31_500_023089.jpg', 'Aerial_temp/BQ31_500_023090.jpg'], 
	['Aerial_temp/BQ31_500_024079.jpg', 'Aerial_temp/BQ31_500_024080.jpg', 'Aerial_temp/BQ31_500_024081.jpg', 'Aerial_temp/BQ31_500_024082.jpg', 'Aerial_temp/BQ31_500_024083.jpg', 'Aerial_temp/BQ31_500_024084.jpg', 'Aerial_temp/BQ31_500_024085.jpg', 'Aerial_temp/BQ31_500_024086.jpg', 'Aerial_temp/BQ31_500_024087.jpg', 'Aerial_temp/BQ31_500_024088.jpg', 'Aerial_temp/BQ31_500_024089.jpg', 'Aerial_temp/BQ31_500_024090.jpg'], 
	['Aerial_temp/BQ31_500_025079.jpg', 'Aerial_temp/BQ31_500_025080.jpg', 'Aerial_temp/BQ31_500_025081.jpg', 'Aerial_temp/BQ31_500_025082.jpg', 'Aerial_temp/BQ31_500_025083.jpg', 'Aerial_temp/BQ31_500_025084.jpg', 'Aerial_temp/BQ31_500_025085.jpg', 'Aerial_temp/BQ31_500_025086.jpg', 'Aerial_temp/BQ31_500_025087.jpg', 'Aerial_temp/BQ31_500_025088.jpg', 'Aerial_temp/BQ31_500_025089.jpg', 'Aerial_temp/BQ31_500_025090.jpg'], 
	['Aerial_temp/BQ31_500_026079.jpg', 'Aerial_temp/BQ31_500_026080.jpg', 'Aerial_temp/BQ31_500_026081.jpg', 'Aerial_temp/BQ31_500_026082.jpg', 'Aerial_temp/BQ31_500_026083.jpg', 'Aerial_temp/BQ31_500_026084.jpg', 'Aerial_temp/BQ31_500_026085.jpg', 'Aerial_temp/BQ31_500_026086.jpg', 'Aerial_temp/BQ31_500_026087.jpg', 'Aerial_temp/BQ31_500_026088.jpg', 'Aerial_temp/BQ31_500_026089.jpg', 'Aerial_temp/BQ31_500_026090.jpg'], 
	['Aerial_temp/BQ31_500_027079.jpg', 'Aerial_temp/BQ31_500_027080.jpg', 'Aerial_temp/BQ31_500_027081.jpg', 'Aerial_temp/BQ31_500_027082.jpg', 'Aerial_temp/BQ31_500_027083.jpg', 'Aerial_temp/BQ31_500_027084.jpg', 'Aerial_temp/BQ31_500_027085.jpg', 'Aerial_temp/BQ31_500_027086.jpg', 'Aerial_temp/BQ31_500_027087.jpg', 'Aerial_temp/BQ31_500_027088.jpg', 'Aerial_temp/BQ31_500_027089.jpg', 'Aerial_temp/BQ31_500_027089.jpg'],
	['Aerial_temp/BQ31_500_028079.jpg', 'Aerial_temp/BQ31_500_028080.jpg', 'Aerial_temp/BQ31_500_028081.jpg', 'Aerial_temp/BQ31_500_028082.jpg', 'Aerial_temp/BQ31_500_028083.jpg', 'Aerial_temp/BQ31_500_028084.jpg', 'Aerial_temp/BQ31_500_028085.jpg', 'Aerial_temp/BQ31_500_028086.jpg', 'Aerial_temp/BQ31_500_028087.jpg', 'Aerial_temp/BQ31_500_028087.jpg', 'Aerial_temp/BQ31_500_028087.jpg', 'Aerial_temp/BQ31_500_028087.jpg'],
]

def stitch_img(output_filename, list_list_im):

	horizontal_imgs = []

	row = 1
	# First stitch each list_im horizontally to form a strip
	for list_im in list_list_im:
		imgs    = [ PIL.Image.open(i) for i in list_im ]
		# pick the image which is the smallest, and resize the others to match it (can be arbitrary image shape here)
		imgs_comb = np.hstack( (np.asarray(i) for i in imgs ) )

		imgs_comb = PIL.Image.fromarray( imgs_comb)
		horizontal_imgs.append(imgs_comb)
		row += 1

	# Next for a vertical stacking it is simple: use vstack
	imgs_final = np.vstack( np.asarray(i) for i in horizontal_imgs ) 
	imgs_final = PIL.Image.fromarray( imgs_final)
	imgs_final.save( output_filename )
	return imgs_final

full_img = stitch_img('final.jpg', list_list_im)

def subdivide_img(img, div_h, div_v):
	w, h = img.size
	sub_w = w / div_h
	sub_h = w / div_v
	output_imgs = []
	for i in range(div_v):
		row = []
		for j in range(div_v):
			box = (sub_w * j, sub_h * i, sub_w * j + sub_w, sub_h * i + sub_h)
			new_img = img.crop(box)
			row.append(new_img)
			
		output_imgs.append(row)

	return output_imgs

subdivided_imgs = subdivide_img(full_img, 4, 4)
for i in range(4):
	for j in range(4):
		subdivided_imgs[i][j].save('final_{}_{}.jpg'.format(i+1, j+1))
