lens=[]
set_random_seed(70036)
with open('../bases-c31/g6_7_20.txt') as f:
	G=[eval(pstr) for pstr in f.readLine().split(',')]
for i in range(100):
	I=R.ideal(G)
	p=sum([R.random_element(100,100)*g for g in G])
	t=cputime()
	r=I.reduce(p)
	t = cputime(t)
	print t, r
