#Jacob Vargo
#never run on current directory. assume directory given has no subdirectories.

import argparse
import sys
import subprocess
import fnmatch
import os

parser = argparse.ArgumentParser(description='Encrypt some integer.')
parser.add_argument('-d', '--directory', required=True)
parser.add_argument('-p', '--public_file', required=True)
parser.add_argument('-r', '--private_file', required=True)
parser.add_argument('-vk', '--validating_key_file', required=True)
args = parser.parse_args()

#Verify the integrity of the unlocking party's public key information.
with open(args.public_file + "-casig", 'r') as file:
	sig = file.readline()
#call rsa-sign similiarly to calling from command line. save stdout from call to result
result = subprocess.Popen(['python', 'rsa-validate.py', '-k', args.validating_key_file, "-m", args.public_file, "-s", args.public_file + "-casig"], stdout=subprocess.PIPE).communicate()[0]
if result == "False\n":
	print "Key did not validate"
	sys.exit()
#print "True"

##Verify the integrity of the symmetric key manifest using the locking party's public key.
with open("manifest", 'r') as file:
	sig = file.readline()
#call rsa-sign similiarly to calling from command line. save stdout from call to result
result = subprocess.Popen(['python', 'rsa-validate.py', '-k', args.public_file, "-m", "manifest", "-s", "manifest-casig"], stdout=subprocess.PIPE).communicate()[0]
if result == "False\n":
	print "Manifest did not validate"
	sys.exit()
#print "True"

##Generate a random AES key for encryption and tagging, encrypt that key with the unlocking party's public key, write that information to a file, called the symmetric key manifest.
sign_call = "python rsa-dec.py -i manifest -o aes_key_file -k " + args.private_file
subprocess.call(sign_call, shell=True)

##Verify the integrity of the encrypted files in the directory based on their CBC-MAC tag files, removing the tag files after validation.
for filename in os.listdir(args.directory):
	if fnmatch.fnmatch(filename, '*-tag'):
		filename = args.directory + '/' +filename[:-4]
		print filename
		print 'Verifying: ', filename
		with open("manifest", 'r') as file:
			sig = file.readline()
		#call similiarly to calling from command line. save stdout from call to result
		result = subprocess.Popen(["./cbcmac-validate", "-k", "aes_key_file", "-m", filename + "-enc",  "-t", filename + "-tag"], stdout=subprocess.PIPE).communicate()[0]
		if result == "False\n":
			print filename, " did not validate"
			sys.exit()
		#print "True"
		#decrypt file
		sign_call = "./cbc-dec -k aes_key_file -i " + filename + "-enc -o " + filename
		subprocess.call(sign_call, shell=True)
		os.remove(filename + '-enc')
		os.remove(filename + '-tag')
os.remove("aes_key_file")




