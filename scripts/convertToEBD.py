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

def isNumber(s):
	try:
		float(s)
		return True
	except ValueError:
		return False

def doWork(args):
	fin = open(args.inputFile)
	data = fin.readlines()
	fin.close()
	
	# determine if format is sparse or dense
	bSparse = False
	for line in data:
		if line[0] == '#':
			continue
			
		lineSplit = line.split('\t')
		if len(lineSplit) == 2 or len(lineSplit) == 3:
			bSparse = True
		break
		
	# convert to EBD format
	sampleOTUs = {}
	otuIds = set([])
	if bSparse:
		print('Parsing OTU data in sparse biom-format.')
		for line in data:
			if line[0] == '#' or line.strip() == '':
				continue
				
			lineSplit = line.split('\t')
			otuId = lineSplit[0]
			sampleId = lineSplit[1]
			count = 1
			if len(lineSplit) == 3:
				count = float(lineSplit[2])
				
			if sampleId not in sampleOTUs:
				sampleOTUs[sampleId] = {}
			
			sampleOTUs[sampleId][otuId] = sampleOTUs[sampleId].get(otuId, 0) + count
			otuIds.add(otuId)
	else:
		print('Parsing OTU data in dense biom-format.')
		for line in data:
			if '#OTU ID' in line or 'OTU ID' in line or '#otu id' in line or 'otu id' in line:
				lineSplit = line.split('\t')
				
				if lineSplit[-1].strip() == 'Consensus Lineage':
					# QIIME-formatted OTU table with taxonomic information
					sampleIds = [x.strip() for x in lineSplit[1:-1]]
				else:
					# standard biom-formatted table
					sampleIds = [x.strip() for x in lineSplit[1:]]
					
				for sampleId in sampleIds:
					sampleOTUs[sampleId] = {}
					
				continue
				
			if line[0] == '#' or line.strip() == '':
				continue
				
			lineSplit = line.split('\t')
			otuId = lineSplit[0]
			counts = [float(x) for x in lineSplit[1:len(sampleIds)+1]]

			for i in range(0, len(sampleIds)):
				sampleOTUs[sampleIds[i]][otuId] = counts[i]
			otuIds.add(otuId)
			
	# write EBD OTU table
	fout = open(args.outputFile, 'w')
	sortedOTUs = sorted(list(otuIds))
	for otuId in sortedOTUs:
		fout.write('\t' + otuId)
	fout.write('\n')
	
	for sampleId in sampleOTUs:
		fout.write(sampleId)
		
		for otuId in sortedOTUs:
			if otuId in sampleOTUs[sampleId]:
				fout.write('\t' + str(sampleOTUs[sampleId][otuId]))
			else:
				fout.write('\t0')
		fout.write('\n')
		
	print('EBD formatted OTU data written to: ' + args.outputFile)
	
if __name__ == '__main__':
	parser = argparse.ArgumentParser(description="Convert UniFrac environment files for use with EBD.")
	parser.add_argument('inputFile', help='Input OTU table in sparse or dense UniFrac format.')
	parser.add_argument('outputFile', help='Output OTU table in EBD format.')

	args = parser.parse_args()        

	doWork(args)