#!/usr/bin/python
# -*- coding: utf-8 -*-

import codecs

def load_part(name):
	f = codecs.open('./html/part/' + name + ".html", 'r', 'utf8')
	html = f.read()
	f.close()
	return html
