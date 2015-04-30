#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os
import codecs

import funcdoc
import filedoc
import htmlldr

def is_comment_begin(text):
	return text.strip() == u'/**'

def is_comment_end(text):
	return text.strip() == u'*/'

def is_block1(text):
	return 	(len(text) == 1 		\
			and text[0] == u'\t')	\
			or (len(text) >= 2		\
			and text[0] == u'\t'	\
			and text[1] != u'\t')

def is_block2(text):
	return 	(len(text) == 2 		\
			and text[0] == u'\t'	\
			and text[1] == u'\t')	\
			or (len(text) >= 3		\
			and text[0] == u'\t'	\
			and text[1] == u'\t'	\
			and text[2] != u'\t')

def is_block3(text):
	return 	(len(text) == 3 		\
			and text[0] == u'\t' 	\
			and text[1] == u'\t'	\
			and text[2] == u'\t')	\
			or (len(text) >= 4		\
			and text[0] == u'\t'	\
			and text[1] == u'\t'	\
			and text[2] == u'\t' 	\
			and text[3] != u'\t')

def get_node_name(text):
	name = u''
	is_name_char = False
	for char in text:
		if char == u':':
			break
		if is_name_char:
			name = name + char
		if char == u'@':
			is_name_char = True
	if is_name_char:
		return name
	else:
		return None

def get_node_value(text):
	value = u''
	is_value_char = False
	for char in text:
		if is_value_char:
			value = value + char
		if char == u':':
			is_value_char = True
	return value

def process_die(path, ln, message):
	raise Exception(u'[Error]: %s: %d: %s' 
						% (path, ln + 1, message))

global total_line
total_line = 0

def process(path, document_path):
	STATE_NONE = 0
	STATE_FILE = 1
	STATE_FUNC = 2
	source_file = os.path.split(path)[1]
	target_file = source_file + '.html'
	funchtml = htmlldr.load_part('func');
	funclinkshtml = u'';
	f = codecs.open(path, 'r', 'utf8')
	lines = []
	try:
		for line in f:
			lines.append(line.rstrip())
	except Exception, e:
		process_die(path, len(lines) + 1, "Invalid line")
		f.close();
	f.close()
	ln = 0
	state = STATE_NONE
	comment = False
	# 文件注释的各个域。
	file_name = u''
	file_author = u''
	file_date = u''
	file_desc = u''
	# 函数注释的各个域。
	func_name = u''
	func_access = u''
	func_desc = u''
	func_params = None
	func_ret_type = u''
	func_ret_desc = u''
	# 文件注释对象。
	file_doc = None
	func_doc = None
	
	global total_line
	total_line = total_line + len(lines)

	while ln < len(lines):
		line = lines[ln]
		# 处理 /*。
		if is_comment_begin(line):
			if comment:
				process_die(path, ln, 'Invalid "/*"')
			comment = True
		# 处理 */。
		elif is_comment_end(line):
			if comment:
				if state == STATE_FILE:
					file_doc = filedoc.FileDoc(file_name, file_author, file_date, file_desc)
					file_name = u''
					file_author = u''
					file_date = u''
					file_desc = u''
				elif state == STATE_FUNC:
					if file_doc == None:
						process_die(path, ln, 'Expect file comment')
					else:
						funclinkhtml = htmlldr.load_part('funclink')
						funclinkhtml = funclinkhtml.replace('{{{{File}}}}', os.path.split(target_file)[1])
						funclinkhtml = funclinkhtml.replace('{{{{Name}}}}', func_name.strip())
						funclinkshtml = funclinkshtml + funclinkhtml
						func_doc = funcdoc.FuncDoc(func_name, func_access, func_desc)
						for func_param in func_params:
							func_doc.add_param(func_param['arr'][0], func_param['arr'][1], func_param['arr'][2], func_param['desc'])
						if func_ret_type != u'':
							func_doc.set_ret(func_ret_type, func_ret_desc)
						file_doc.add_func(func_doc)
						func_name = u''
						func_access = u''
						func_desc = u''
						func_params = None
						func_ret_type = u''
						func_ret_desc = u''
				state = STATE_NONE
				comment = False
		# 处理文档注释的第一行，也就是文档类型。
		elif comment and state == STATE_NONE:
			if is_block1(line):
				name = get_node_name(line)
				if name == 'File':
					state = STATE_FILE
					file_name = get_node_value(line)
				elif name == 'Function':
					state = STATE_FUNC
					func_name = get_node_value(line)
				else:
					process_die(path, ln, 'Invalid block')
			else:
				process_die(path, ln, 'Invalid block')
		# 处理文件文档注释的参数。
		elif comment and state == STATE_FILE:
			if is_block1(line):
				name = get_node_name(line)
				value = get_node_value(line)
				if name == u'Author':
					file_author = value
				elif name == u'Date':
					file_date = value
				elif name == u'Description':
					ln = ln + 1
					while ln < len(lines):
						line = lines[ln]
						if is_block2(line):
							file_desc = file_desc + line + '\n'
						else:
							break
						ln = ln + 1
					ln = ln - 1
				else:
					process_die(path, ln, 'Invalid file comment parameter')
			else:
				process_die(path, ln, 'Invalid file comment parameter')
		# 处理函数文档注释的参数。
		elif comment and state == STATE_FUNC:
			if is_block1(line):
				name = get_node_name(line)
				value = get_node_value(line)
				if name == u'Access':
					func_access = value
				elif name == u'Description':
					ln = ln + 1
					while ln < len(lines):
						line = lines[ln]
						if is_block2(line):
							func_desc = func_desc + line + '\n'
						else:
							break
						ln = ln + 1
					ln = ln - 1
				elif name == u'Parameters':
					func_params = []
					param_desc = u''
					ln = ln + 1
					while ln < len(lines):
						line = lines[ln]
						if is_block2(line):
							param_arr = line.split(',')
							func_param = {'arr':param_arr, 'desc':u''}
							func_params.append(func_param)
							ln = ln + 1
							while ln < len(lines):
								line = lines[ln]
								if is_block3(line):
									func_param['desc'] = func_param['desc'] + line + '\n';
								else:
									break;
								ln = ln + 1
							ln = ln - 1
						else:
							break;
						ln = ln + 1
					ln = ln - 1
				elif name == u'Return':
					ln = ln + 1
					line = lines[ln]
					if is_block2(line):
						func_ret_type = line
						ln = ln + 1
						while ln < len(lines):
							line = lines[ln]
							if is_block3(line):
								func_ret_desc = func_ret_desc + line + '\n'
							else:
								break
							ln = ln + 1
					ln = ln - 1
			else:
				process_die(path, ln, 'Invalid function comment parameter')
		ln = ln + 1
	# 把文档写入文件
	of = codecs.open(os.path.join(document_path, target_file), 'w', 'utf8')
	if file_doc != None:
		of.write(file_doc.get_html())
	of.close()

	# 把函数信息写入文件
	funchtml = funchtml.replace('{{{{Links}}}}', funclinkshtml)
	of = codecs.open(os.path.join(document_path, '__func__%s.html' % source_file), 'w', 'utf8')
	of.write(funchtml)
	of.close()

def build(project_path, document_path, root):
	dirhtml = htmlldr.load_part('dir');
	dirlinkshtml = u''
	if not root:
		dirlinkhtml = htmlldr.load_part('dirlink')
		dirlinkhtml = dirlinkhtml.replace(u'{{{{Link}}}}', u'../__dir__.html')
		dirlinkhtml = dirlinkhtml.replace(u'{{{{Name}}}}', u'..')
		dirlinkshtml = dirlinkshtml + dirlinkhtml
	if not os.path.exists(document_path):
		os.mkdir(document_path)
	dir_list = sorted(os.listdir(project_path))
	for item in dir_list:
		if item == '.' or item == '..' or item[0] == '.':
			continue
		full_path = os.path.join(project_path, item)
		if os.path.isfile(full_path) and (os.path.splitext(item)[1] == '.c' or os.path.splitext(item)[1] == '.h'):
			process(full_path, document_path)
			filelinkhtml = htmlldr.load_part('filelink')
			filelinkhtml = filelinkhtml.replace('{{{{Link}}}}', item + '.html')
			filelinkhtml = filelinkhtml.replace('{{{{FuncLink}}}}', '__func__%s.html' % item)
			filelinkhtml = filelinkhtml.replace('{{{{Name}}}}', item)
			dirlinkshtml = dirlinkshtml + filelinkhtml
		if os.path.isdir(full_path):
			build(os.path.join(project_path, item), os.path.join(document_path, item), False)
			dirlinkhtml = htmlldr.load_part('dirlink')
			dirlinkhtml = dirlinkhtml.replace(u'{{{{Link}}}}', item + '/__dir__.html')
			dirlinkhtml = dirlinkhtml.replace(u'{{{{Name}}}}', item)
			dirlinkshtml = dirlinkshtml + dirlinkhtml
	indexhtml = htmlldr.load_part('index')
	of = codecs.open(os.path.join(document_path, 'index.html'), 'w', 'utf8')
	of.write(indexhtml)
	of.close()
	# 把目录信息写入文件
	dirhtml = dirhtml.replace(u'{{{{Links}}}}', dirlinkshtml);
	of = codecs.open(os.path.join(document_path, '__dir__.html'), 'w', 'utf8')
	of.write(dirhtml)
	of.close()

# python idoc.py {project path} {document path}
def main():
	if len(sys.argv) != 3:
		print 'python idoc.py {project path} {document path}'
		return
	project_path = sys.argv[1]
	document_path = sys.argv[2]
	build(project_path, document_path, True)

	global total_line
	print "Total line:", total_line

if __name__ == '__main__':
	main()
