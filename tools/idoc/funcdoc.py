#!/usr/bin/python
# -*- coding: utf-8 -*-

import htmlldr

class FuncDoc(object):
	IN = 'IN'
	OUT = 'OUT'
	IN_OUT = 'IN OUT'

	def __init__(self, name, access, desc):
		self.name = name
		self.access = access
		self.desc = desc
		self.params = []
		self.ret = None

	def add_param(self, name, type, inout, desc):
		param = {'name':name, 'type':type, 'inout':inout, 'desc':desc}
		self.params.append(param)

	def set_ret(self, type, desc):
		self.ret = {'type':type, 'desc':desc}

	def get_params(self):
		return self.params

	def get_ret(self):
		return self.ret

	def get_html(self):
		fhtml = htmlldr.load_part(u'function')
		fhtml = fhtml.replace(u'{{{{Name}}}}', self.name.strip())
		fhtml = fhtml.replace(u'{{{{Access}}}}', self.access)
		fhtml = fhtml.replace(u'{{{{Desc}}}}', self.desc.replace('\n', '<br />'))
		pshtml = u''
		for param in self.params:
			phtml = htmlldr.load_part(u'param')
			phtml = phtml.replace(u'{{{{Name}}}}', param['name'])
			phtml = phtml.replace(u'{{{{Type}}}}', param['type'])
			phtml = phtml.replace(u'{{{{InOut}}}}', param['inout'])
			phtml = phtml.replace(u'{{{{Desc}}}}', param['desc'])
			pshtml = pshtml + phtml
		fhtml = fhtml.replace(u'{{{{Params}}}}', pshtml)
		if self.ret != None:
			fhtml = fhtml.replace(u'{{{{RetType}}}}', self.ret['type'])
			fhtml = fhtml.replace(u'{{{{RetDesc}}}}', self.ret['desc'])
		else:
			fhtml = fhtml.replace(u'{{{{RetType}}}}', u'')
			fhtml = fhtml.replace(u'{{{{RetDesc}}}}', u'')
		return fhtml
