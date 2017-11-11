#Jacob Vargo

import random
import argparse

parser = argparse.ArgumentParser(description='Encrypt some integer.')
parser.add_argument('-i', '--input_file', required=True)
parser.add_argument('-o', '--out_file', required=True)
parser.add_argument('-k', '--key_file', required=True)
args = parser.parse_args()

with open(args.input_file, 'r') as file:
	c = int(file.readline()[:-1])
with open(args.key_file, 'r') as file:
	N = int(file.readline()[:-1])
	d = int(file.readline()[:-1])

m = pow(c, d, N)
m_list = list(hex(m)[2:-1])
#m == (0x00 || 0x02 || r || 0x00 || m)
#size of r = n/2, so 0x00 dividing r and m is in the second half
for i in range(len(m_list)/2):
	if m_list[i+len(m_list)/2] == '0' and m_list[i+len(m_list)/2+1] == '0':
		start = i+3
		break
m = int("".join(m_list[start:]),16)
with open(args.out_file, 'w') as file:
	file.write(str(c))
