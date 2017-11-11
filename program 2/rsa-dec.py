#Jacob Vargo

import random
import argparse
bit_len_n = 128

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
#m = (0x00 || 0x02 || r || 0x00 || m)
#m_list = ['0', '0', '0', '2'] + r_list + ['0', '0'] + m_list
print m_list
for i in range(len(m_list)-1):
	if m_list[i] == '0' and m_list[i+1] == '0':
		print 'hit'
m = int("".join(m_list),16)
with open(args.out_file, 'w') as file:
	file.write(str(c))
