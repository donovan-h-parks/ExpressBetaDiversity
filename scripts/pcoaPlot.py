###############################################################################
#
# pcoaPlot.py - create plots of PCoA'ed data matrix
#
###############################################################################
#                                                                             #
#    This program is free software: you can redistribute it and/or modify     #
#    it under the terms of the GNU General Public License as published by     #
#    the Free Software Foundation, either version 3 of the License, or        #
#    (at your option) any later version.                                      #
#                                                                             #
#    This program is distributed in the hope that it will be useful,          #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#    GNU General Public License for more details.                             #
#                                                                             #
#    You should have received a copy of the GNU General Public License        #
#    along with this program. If not, see <http://www.gnu.org/licenses/>.     #
#                                                                             #
###############################################################################

__prog_name__ = 'pcoaPlot'
__prog_desc__ = 'PCoA plot of EBD dissimilarity matrix'

__author__ = 'Donovan Parks'
__copyright__ = 'Copyright 2014'
__credits__ = ['Donovan Parks']
__license__ = 'GPL3'
__version__ = '1.0.0'
__maintainer__ = 'Donovan Parks'
__email__ = 'donovan.parks@gmail.com'
__status__ = 'Development'

import argparse
import sys
import numpy as np

from matplotlib.ticker import MaxNLocator

from AbstractPlot import AbstractPlot

class PcoaPlot(AbstractPlot):
	def __init__(self, options):
		AbstractPlot.__init__(self, options)

	def plot(self, f, seqIds, pc, variance): 
		# ensure pc matrix has at least 3 dimensions
		if pc.shape[1] == 1:
			pc = np.append(pc, np.zeros((pc.shape[0], 2)), 1)
			variance = np.append(variance[0], np.ones(2))
		elif pc.shape[1] == 2:
			pc = np.append(pc, np.zeros((pc.shape[0], 1)), 1)
			variance = np.append(variance[0:2], np.ones(1))
		
		# Set size of figure
		self.fig.clear()
		self.fig.set_size_inches(self.options.width, self.options.height)

		axesPC1vsPC2 = self.fig.add_subplot(221)
		axesPC2vsPC3 = self.fig.add_subplot(222)
		axesPC1vsPC3 = self.fig.add_subplot(223)
		axesVariance = self.fig.add_subplot(224)
		
		# get sequence in bin
		seqs = readFasta(f)
		
		binIndices = []
		for rowIndex, seqId in enumerate(seqIds):
			if seqId in list(seqs.keys()):
				binIndices.append(rowIndex)

		# plot sequence in bin
		axesPC1vsPC2.scatter(pc[:,0], pc[:,1], s=10, lw=0.5, facecolor=(0.8, 0.8, 0.8), marker="o")  
		axesPC1vsPC2.scatter(pc[binIndices,0], pc[binIndices,1], s=10, lw=0.5, facecolor="r", marker="o")    
		axesPC1vsPC2.set_xlabel('PC1 (%.1f%%)' % (variance[0]*100))
		axesPC1vsPC2.set_ylabel('PC2 (%.1f%%)' % (variance[1]*100))

		axesPC2vsPC3.scatter(pc[:,2], pc[:,1], s=10, lw=0.5, facecolor=(0.8, 0.8, 0.8), marker="o") 
		axesPC2vsPC3.scatter(pc[binIndices,2], pc[binIndices,1], s=10, lw=0.5, facecolor="r", marker="o")    
		axesPC2vsPC3.set_xlabel('PC3 (%.1f%%)' % (variance[2]*100))
		axesPC2vsPC3.set_ylabel('PC2 (%.1f%%)' % (variance[1]*100))
		
		axesPC1vsPC3.scatter(pc[:,0], pc[:,2], s=10, lw=0.5, facecolor=(0.8, 0.8, 0.8), marker="o") 
		axesPC1vsPC3.scatter(pc[binIndices,0], pc[binIndices,2], s=10, lw=0.5, facecolor="r", marker="o")    
		axesPC1vsPC3.set_xlabel('PC1 (%.1f%%)' % (variance[0]*100))
		axesPC1vsPC3.set_ylabel('PC3 (%.1f%%)' % (variance[2]*100))
		
		axesVariance.plot(np.arange(len(variance), dtype=int)+1, np.cumsum(variance))
		axesVariance.set_xlabel('Principal Component')
		axesVariance.set_ylabel('Percentage of Cumulative Variance')
		#axesVariance.vlines(3, 0, 1.0, linestyle='dashed', color=self.axesColour, zorder=0, lw=0.5)
		axesVariance.set_ylim([0, 1.02])
		axesVariance.set_xlim([0, len(variance)])
		
		axesVariance.get_xaxis().set_major_locator(MaxNLocator(integer=True))
		xticks = axesVariance.get_xticks()
		if 0 in xticks and 1 not in xticks:
			xticks = np.append(np.array([1]),  xticks[1:])
		axesVariance.set_xticks(xticks)
		
		# Prettify plot  
		for axes in [axesPC1vsPC2, axesPC2vsPC3, axesPC1vsPC3, axesVariance]:    
			for a in axes.yaxis.majorTicks:
				a.tick1On=True
				a.tick2On=False
					
			for a in axes.xaxis.majorTicks:
				a.tick1On=True
				a.tick2On=False
				
			for line in axes.yaxis.get_ticklines(): 
				line.set_color(self.axesColour)
					
			for line in axes.xaxis.get_ticklines(): 
				line.set_color(self.axesColour)
				
			for loc, spine in axes.spines.items():
				if loc in ['right','top']:
					spine.set_color('none') 
				else:
					spine.set_color(self.axesColour)
				
		self.fig.tight_layout(pad=5, w_pad=15, h_pad=15)
		self.savePlot(outputFile, dpi=options.dpi)
		
if __name__ == "__main__":
	print(__prog_name__ + ' v' + __version__ + ': ' + __prog_desc__)
	print('  by ' + __author__ + ' (' + __email__ + ')' + '\n')

	parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument('diss_matrix_file', help="dissimilarity matrix")
	parser.add_argument('output_file', help="output file")
	parser.add_argument('--width', type = float, default = 6.5, help='width of output image')
	parser.add_argument('--height', type = float, default = 6.5, help='height of output image')
	parser.add_argument('--dpi', type = int, default = 300, help='desired DPI of output image')
	parser.add_argument('--font_size', type = int, default = 8, help='Desired font size')
	
	args = parser.parse_args()
	
	try:
		pcoaPlot = PcoaPlot(args)
		pcoaPlot.plot(args.diss_matrix_file, args.width, args.height, args.image_type, args.output_file)
	except SystemExit:
		print("\nControlled exit resulting from an unrecoverable error or warning.")
	except:
		print("\nUnexpected error:", sys.exc_info()[0])
	raise
