defgload(path):	withopen(path,'rb')asf:		returneval(''+f.readline().replace('^','**')+'')defpload(path):	withopen(path,'rb')asf:		returneval(f.readline().replace('^','**'))defgsave(path,g):	withopen(path,'wb')asf:		f.write(','.join(str(p)forping))defpsave(path,p):	withopen(path,'wb')asf:		f.write(str(p))