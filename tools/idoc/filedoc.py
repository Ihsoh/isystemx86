#!/usr/bin/python
# -*- coding: utf-8 -*-

import htmlldr

class FileDoc(object):
	def __init__(self, name, author, date, desc):
		self.name = name
		self.author = author
		self.date = date
		self.desc = desc
		self.funcs = []

	def add_func(self, func):
		self.funcs.append(func)

	def get_html(self):
		fhtml = htmlldr.load_part(u'file')
		fhtml = fhtml.replace(u'{{{{Name}}}}', self.name)
		fhtml = fhtml.replace(u'{{{{Author}}}}', self.author)
		fhtml = fhtml.replace(u'{{{{Date}}}}', self.date)
		fhtml = fhtml.replace(u'{{{{Desc}}}}', self.desc.replace('\n', '<br />'))
		funchtml = u''
		for func in self.funcs:
			funchtml = funchtml + func.get_html()
		fhtml = fhtml.replace(u'{{{{Functions}}}}', funchtml)
		return fhtml
