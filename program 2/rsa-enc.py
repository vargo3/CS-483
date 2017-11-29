#Jacob Vargo

import random
import argparse

parser = argparse.ArgumentParser(description='Encrypt some integer.')
parser.add_argument('-i', '--input_file', required=True)
parser.add_argument('-o', '--out_file', required=True)
parser.add_argument('-k', '--key_file', required=True)
args = parser.parse_args()

with open(args.input_file, 'r') as file:
	m = int(file.readline())
	#print m
with open(args.key_file, 'r') as file:
	bit_len_n = int(file.readline()[:-1])
	N = int(file.readline()[:-1])
	e = int(file.readline())
#print m, bit_len_n, N, e
r = random.getrandbits(bit_len_n/2)
r_list = list(hex(r)[2:-1])
#replace bytes in r with new random bytes that are not 0x00
for i in range(len(r_list)-1):
	#cannot have 2 0's in a row
	if r_list[i] == '0' and r_list[i+1] == '0':
		#replace the first '0' with a random number (1-15)
		new = random.randint(1, 15)
		if new <= 9:
			new += 48
		else:
			new += 97 - 10
		r_list[i] = chr(new)
m_list = list(hex(m)[2:])
#if bit length of m < (n/2 -24), then pad front with 0's(doesn't change m)
while len(m_list)*4 < (bit_len_n/2 -24):
	m_list.insert(0, '0')
#m = (0x00 || 0x02 || r || 0x00 || m)
r_list = ['0', '0', '0', '2'] + r_list + ['0', '0'] + m_list
m = int("".join(r_list),16)
if m != m%N:
	print "m is too large!"
c = pow(m, e, N)
with open(args.out_file, 'w') as file:
	file.write(str(c))
