#!/usr/bin/python
# -*- coding: utf-8 -*-

from PIL import Image, ImageDraw, ImageFont
import sys
import struct
import os

if(len(sys.argv) != 6):
	print u'ERROR: invalid parameters.'
	sys.exit(-1)

mode = sys.argv[1]
font_file = sys.argv[2]
font_size = int(sys.argv[3])
font_width = int(sys.argv[4])
font_height = int(sys.argv[5])

font = ImageFont.truetype(font_file, font_size)




if mode == u'test':
	def create(char):
		image = Image.new(u'RGB', (font_width, font_height), (255, 255, 255))
		draw = ImageDraw.Draw(image)
		draw.text((0, 0), char, font=font, fill=(0, 0, 0))
		image.save(u'out/' + char + u'.png', u'png')
	create(u'A')
	create(u'a')
	create(u'0')
	create(u'~')
	create(u'_')
	create(u'|')
elif mode == u'build':
	def create(char):
		image = Image.new(u'RGB', (font_width, font_height), (255, 255, 255))
		draw = ImageDraw.Draw(image)
		draw.text((0, 1), char, font=font, fill=(0, 0, 0))
		return image
	fname = os.path.splitext(font_file)[0]
	f = open(u'out/' + fname + '.efx', u'wb')
	f.write(u'ENFNTX')
	for asc in range(256):
		char = chr(asc)
		image = create(char)
		# image.save(u'out/char' + unicode(asc) + u'.png', u'png')
		for y in range(image.height):
			for x in range(image.width):
				pixel = image.getpixel((x, y))
				value = (pixel[0] + pixel[1] + pixel[2]) / 3
				f.write(chr(value))
	f.close()

