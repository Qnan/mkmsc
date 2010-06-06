lens=[]
set_random_seed(70036)

for i in range(100):
	set_random_seed(i)
	G=R.ideal([R.random_element(4,6) for i in range(2)]).groebner_basis()
	I=R.ideal(G)
	l = len(G)
	if l > 10:
		continue
	p=sum([R.random_element(100,100)*G[i] for i in range(l)])
	t=cputime()
	r=I.reduce(p)
	t = cputime(t)
	if t > 10:
		with open('bases/g'+str(i)+'_'+str(l)+'_'+str(floor(t))+'.txt', 'wb') as f:
			f.write(str(G))			
	print l, t, r
