#Jacob Vargo
#Generates a valid RSA signature for a given message.

import argparse
import hashlib

parser = argparse.ArgumentParser(description='Encrypt some integer.')
parser.add_argument('-k', '--key_file', required=True)
parser.add_argument('-m', '--message_file', required=True)
parser.add_argument('-s', '--signature_file', required=True)
args = parser.parse_args()

with open(args.message_file, 'r') as file:
	m = file.readline()
with open(args.signature_file, 'r') as file:
	sig = int(file.readline())
with open(args.key_file, 'r') as file:
	bit_len_n = int(file.readline()[:-1])
	N = int(file.readline()[:-1])
	e = int(file.readline())

h = int(hashlib.sha256(m).hexdigest(), 16)
v = pow(sig, e, N)

if v == h:
	print "True"
else:
	print "False"
