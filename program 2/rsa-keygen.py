#Jacob Vargo

import random

def egcd(test_val, b):
	"extended euclidean algorithm"
	if test_val == 0:
		return (b, 0, 1)
	g, y, x = egcd(b % test_val,test_val)
	return (g, x - (b//test_val) * y, y)

def mod_inverse(test_val, mod):
	"return modular inverse of test_val % mod"
	g, x, y = egcd(test_val, mod)
	if g != 1:
		return None
	return x % mod

def is_probable_prime(n):
	s = n - 1
	t = 0
	while s % 2 == 0:
		s = s // 2
		t += 1

	for tries in range(3): # try to falsify n's primality 5 times
		test_val = random.randrange(2, n - 1)
		val = pow(test_val, s, n)
		if val != 1:
		    i = 0
		    while val != (n - 1):
		        if i == t - 1:
		            return False
		        else:
		            i = i + 1
		            val = (val ** 2) % n
	return True

bit_len_n = 6
p = random.getrandbits(bit_len_n)
while not is_probable_prime(p):
	p = random.getrandbits(bit_len_n)
q = random.getrandbits(bit_len_n);
while not is_probable_prime(q):
	q = random.getrandbits(bit_len_n)
N = p*q
order = (p-1) * (q-1)
e = 17
d = mod_inverse(e, order)
with open('pubKey.txt', 'w') as file:
	file.write(str(N) + '\n')
	file.write(str(e) + '\n')
with open('privKey.txt', 'w') as file:
	file.write(str(N) + '\n')
	file.write(str(d) + '\n')
