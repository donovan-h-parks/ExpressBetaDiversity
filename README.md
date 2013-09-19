Express Beta Diversity (EBD)
============================

Taxon- and phylogenetic-based beta diversity measures.

-------------------------------------------------------------------------------

EBD is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

EBD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with EBD.  If not, see <http://www.gnu.org/licenses/>.


Installation:
-------------------------------------------------------------------------------

EBD is a command-line program written in C++.  To install EBD, download 
and uncompress it with the unzip command:
```
  unzip EBD_1_0_4.zip
```

To compile EBD on OSX or Linux simply type 'make' from within the source 
directory of EBD. The resulting executable will be in the bin directory. 
A precompiled executables for Windows is provided in the bin directory. 
Please note that under Windows, EBD must be run from the command-line 
(i.e., the DOS prompt).


Program usage:
-------------------------------------------------------------------------------
```
Usage: EBD [OPTIONS]
Calculates taxon- and phylogenetic-based beta diversity measures.

Options:
 -h, --help           Produce help message.
 -l, --list-calc      List all supported calculators.
 -u, --unit-tests     Execute unit tests.

 -t, --tree-file      Tree in Newick format (if phylogenetic beta-diversity is desired).
 -s, --seq-count-file Sequence count file.
 -p, --output-prefix  Output prefix.
 
 -g, --clustering     Hierarchical clustering method: UPGMA, SingleLinkage, CompleteLinkage, NJ (default = UPGMA).
 
 -j, --jackknife      Number of jackknife replicates to perform (default = 0).
 -d, --seqs-to-draw   Number of sequence to draw for jackknife replicates.
 -z, --sample-size    Print number of sequences in each sample.

 -c, --calculator     Desired calculator (e.g., Bray-Curtis, Canberra).
 -w, --weighted       Indicated if sequence abundance data should be used.
 -m, --mrca           Apply 'MRCA weightings' to each branch (experimental).
 -r, --strict-mrca    Restrict calculator to MRCA subtree.
 -y, --count          Use count data as opposed to relative proportions.

 -x, --max-data-vecs  Maximum number of profiles (data vectors) to have in memory at once (default = 1000).
 
 -a, --all            Apply all calculators and cluster calculators at the specified threshold.
 -b, --threshold      Correlation threshold for clustering calculators (default = 0.8).
 -o, --output-file    Output file for cluster of calculators (default = clusters.txt).

 -v, --verbose        Provide additional information on program execution.
```

Example of applying a specific calculator:
```
./ExpressBetaDiversity -t input.tre -s seq.txt -p bray_curtis -c Bray-Curtis -w
```
which will result in two output files, the raw dissimilarity matrix in bray_curtis.diss 
and a UPGMA hierarchical cluster tree in bray_curtis.tre.
 
Example of querying number of sequences in each sample:
```
./ExpressBetaDiversity -s seq.txt -z
```
which will result in the number of sequences in each sample being written to standard out.
 
Example of applying a specific calculator with jackknife replicates:
```
./ExpressBetaDiversity -t input.tre -s seq.txt -p bray_curtis -c Bray-Curtis -w -j 100 -d 500
```
which will result in two output files, the raw dissimilarity matrix in bray_curtis.diss 
and a UPGMA hierarchical cluster tree in bray_curtis.tre with jackknife support values.
 
Example of applying all calculators and clustering these based on their Pearson correlation:
```
./ExpressBetaDiversity -t input.tre -s seq.txt -a -b 0.9 -o clusters.txt
```
which will result in the output file clusters.txt (see file format below).


Verifying software installation:
-------------------------------------------------------------------------------

A set of unit tests is included to verify proper installation of the EBD 
software. The unit tests can be run with:
```
./ExpressBetaDiversity -u
```

The software should not be used if any of the unit tests fail.


Input file formats:
-------------------------------------------------------------------------------

EBD uses Newick formatted trees as input. Information on this tree format can
be found at: http://evolution.genetics.washington.edu/phylip/newicktree.html.
Here is a simple Newick tree with three leaf nodes labelled A, B, and C:
```
(A:1,(B:1,C:1):1);
```
 
Taxon-based beta-diversity is calculated if an input tree is not specified.

Sequence count information must be specified as a tab-delimited table where
each row is a sample and each column is the name of a leaf node in the provided
tree. Data must be provided for all leaf nodes in the tree. Consider the 
following example:
```
	A	B	C
Sample1	1	2	3
Sample2	10	1	0
Sample3	0	0	1
```
The first row begins indicates each leaf node in the tree seperated by a tab. 
Please note that this line MUST start with a tab. The number of sequences 
associated with each leaf node is then indicated for each sample on a seperate
row. In this example, the first sample is labelled 'Sample1' and contains 1
instance of sequence/OTU A, 2 instances of B, and 3 instances of C. Sample3
contains only instances of C, but note that zeros must be specified for the
other sequence/OTU types.

Example input files are avaliable in the unit-tests directory. 


Converting from QIIME/UniFrac file formats:
-------------------------------------------------------------------------------

The script convertToEBD.py in the scripts directory can be used to 
convert sparse or dense UniFrac-style OTU tables into the format
required by EBD. The UniFrac format is used by many popular services
including the UniFrac web services and QIIME. EBD uses a different
input file format in order to efficently handle data sets consisting
of thousands of samples. The script can be run as follows:
```
./convertUniFracToEBD.py <input file> <ouput file>
```


Dissimilarity output file format:
-------------------------------------------------------------------------------

The resulting dissimilarity between samples is written as a tab-delimited, 
lower-triangular dissimilarity matrix with the first line indicating the number
of samples. Consider the following output:
```
3
A
B	1
C	2	3
```
The first line indicates that there are 3 samples. The dissimilarity between 
samples A and B is 1, A and C is 2, and B and C is 3.

Clustering output file format:
-------------------------------------------------------------------------------

The clustering file indicates clusters of calculators which are correlated. The 
clustering threshold is specified by the user with the --threshold (-b) 
parameter. All calculators in a cluster will be at least as correlated as the 
specified threshold. Results are reported as follows:  
```
Minimum r	Calculators
[0.0]	uChi-squared;
[0.86]	Canberra;CS;uCanberra;uCS;uGower;uManhattan;
[0.91]	uBray-Curtis;uSoergel;uKulczynski;
[0.81]	Bray-Curtis;Kulczynski;Soergel;
...

Complete linkage cluster tree (branch lengths are 1 - Pearson's correlation): 
((('Bray-Curtis':5.60596e-006,'Kulczynski':5.60596e-006):4.13975e-005 ...
```

The first line indicates the column headers. Each subsequent line indicates a
cluster of calculators. The number within the brackets indicates that minimum 
Pearson's correlation between any pair of calculators in the cluster. A 
semicolon seperated list indicates which calculators are in the cluster. The
last line of the file gives the complete linkage tree used to cluster measures.
This can be copied into a seperate file and visualized in any program which
can read a Newick tree file. 

The dissimilarity matrix for calculator X is saved to the file 'X.cluster.diss'
within the same directory as the EBD executable.
 

Citing EBD:
-------------------------------------------------------------------------------

If you use EBD in your research, please cite:

Parks, D.H. and Beiko, R.G. 2013. Measures of phylogenetic differentiation provide 
  robust and complementary insights into microbial communities. ISME J, 7:173-83.


Contact Information:
-------------------------------------------------------------------------------

Donovan Parks
donovan.parks@gmail.com

Robert Beiko
beiko@cs.dal.ca

Program website: http://kiwi.cs.dal.ca/Software/EBD
