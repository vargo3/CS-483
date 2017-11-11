#Jacob Vargo

import random
import argparse
bit_len_n = 256

parser = argparse.ArgumentParser(description='Encrypt some integer.')
parser.add_argument('-i', '--input_file', required=True)
parser.add_argument('-o', '--out_file', required=True)
parser.add_argument('-k', '--key_file', required=True)
args = parser.parse_args()

with open(args.input_file, 'r') as file:
	m = int(file.readline()[:-1])
with open(args.key_file, 'r') as file:
	N = int(file.readline()[:-1])
	e = int(file.readline()[:-1])
print m, N, e
r = random.getrandbits(bit_len_n/2)
r_list = list(hex(r)[2:-1])
#replace bytes in r with new random bytes that are not 0x00
for i in range(len(r_list)-1):
	if r_list[i] == '0' and r_list[i+1] == '0':
		for j in range(2):
			new = random.randint(1, 15)
			if new <= 9:
				new += 48
			else:
				new += 97 - 10
			r_list[i+j] = chr(new)
m_list = list(hex(m)[2:-1])
#if bit length of m < (n/2 -24), then pad front with 0's(doesn't change m)
while len(m_list)*4 < (bit_len_n/2 -24):
	m_list.insert(0, '0')
#m = (0x00 || 0x02 || r || 0x00 || m)
r_list = ['0', '0', '0', '2'] + r_list + ['0', '0'] + m_list
m = int("".join(r_list),16)
c = pow(m%N, e, N)
with open(args.out_file, 'w') as file:
	file.write(str(c)+'\n')
