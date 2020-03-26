#!/usr/bin/env python

__author__ = 'Donovan Parks'
__copyright__ = 'Copyright 2013'
__credits__ = ['Donovan Parks']
__license__ = 'GPL3'
__version__ = '1.0.0'
__maintainer__ = 'Donovan Parks'
__email__ = 'donovan.parks@gmail.com'
__status__ = 'Development'

import argparse

def doWork(args):
	# read lower-triangular matrix
	sampleNames = []
	matrix = []
	with open(args.inputMatrix) as f:
		f.readline() # skip number of samples

		for line in f:
			lineSplit = line.split('\t')
			sampleNames.append(lineSplit[0].strip())
			matrix.append(lineSplit[1:])
			
	# write out full matrix
	fout = open(args.outputMatrix, 'w')
	for sampleName in sampleNames:
		fout.write('\t' + sampleName)
	fout.write('\n')
		
	for r in range(len(matrix)):
		fout.write(sampleNames[r])
		
		for c in range(len(matrix)):
			if c < r:
				fout.write('\t' + matrix[r][c].strip())
			elif c == r:
				fout.write('\t0')
			else:
				fout.write('\t' + matrix[c][r].strip())
				
		fout.write('\n')
	
	print('Full matrix written to: ' + args.outputMatrix)
	
if __name__ == '__main__':
	parser = argparse.ArgumentParser(description="Convert Phylip lower-triangular matrix produced by EBD to full matrix.")
	parser.add_argument('inputMatrix', help='Dissimilarity matrix produced by EBD.')
	parser.add_argument('outputMatrix', help='Output full dissimilarity matrix.')

	args = parser.parse_args()        

	doWork(args)