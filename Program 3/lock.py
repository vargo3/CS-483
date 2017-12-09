#Jacob Vargo
#never run on current directory. assume directory given has no subdirectories.

import argparse
import sys
import subprocess
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
#if verify fails: destroy everything
if result == "False\n":
	print "Did not validate"
	sys.exit()
#print "True"

##Generate a random AES key for encryption and tagging, encrypt that key with the unlocking party's public key, write that information to a file, called the symmetric key manifest.
aes_key = int('0123456789ab', 16)
with open("aes_key_file", 'w') as file:
	file.write(str(aes_key))
sign_call = "python rsa-enc.py -i aes_key_file -o manifest -k " + args.public_file
subprocess.call(sign_call, shell=True)

##Sign the symmetric key manifest file with the locker's private key.
sign_call = "python rsa-sign.py -k " + args.private_file + " -m manifest -s manifest-casig"
subprocess.call(sign_call, shell=True)

##Encrypt all files in the given directory using aes in CBC mode, deleting the plain text files after encryption.
for filename in os.listdir(args.directory):
	filename = args.directory + '/' +filename
	print 'Encrpyting: ', filename
	sign_call = "./cbc-enc -k aes_key_file -i " + filename + " -o " + filename + "-enc"
	subprocess.call(sign_call, shell=True)
	os.remove(filename)
	##Generate CBC-MAC tags for the resulting cipher text files.
	sign_call = "./cbcmac-tag -k aes_key_file -m " + filename + "-enc -t " + filename + "-tag"
	subprocess.call(sign_call, shell=True)
os.remove("aes_key_file")




