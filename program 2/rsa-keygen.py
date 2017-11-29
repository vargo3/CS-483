#Jacob Vargo

import random
import argparse

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
	if n % 2 == 0:
		return False
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

parser = argparse.ArgumentParser(description='Encrypt some integer.')
parser.add_argument('-p', '--public_key_file', required=True)
parser.add_argument('-s', '--secret_key_file', required=True)
parser.add_argument('-n', '--number_of_bits', required=True)
args = parser.parse_args()

bit_len_n = int(args.number_of_bits)
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
with open(args.public_key_file, 'w') as file:
	file.write(str(bit_len_n) + '\n')
	file.write(str(N) + '\n')
	file.write(str(e))
with open(args.secret_key_file, 'w') as file:
	file.write(str(bit_len_n) + '\n')
	file.write(str(N) + '\n')
	file.write(str(d))



