def gload(path):
	with open(path, 'rb') as f:
		return eval('['+f.readline().replace('^','**')+']')

def pload(path):
	with open(path,'rb') as f:
		return eval(f.readline().replace('^','**'))

def gsave(path, g):
	with open(path, 'wb') as f:
		f.write(', '.join([str(p) for p in g]))

def psave(path, p):
	with open(path, 'wb') as f:
		f.write(str(p))
