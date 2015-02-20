#!/usr/bin/python
# -*- coding: utf-8 -*-

import codecs

global __html_buffer
__html_buffer = {}

def load_part(name):
	if name in __html_buffer.keys():
		return __html_buffer[name]
	else:
		f = codecs.open('./html/part/' + name + ".html", 'r', 'utf8')
		html = f.read()
		f.close()
		__html_buffer[name] = html
		return html
