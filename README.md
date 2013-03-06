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
  unzip EBD_1_0_1.zip

To compile EBD on OSX or Linux simply type 'make' from within the source 
directory of EBD. The resulting executable will be in the bin directory. 
A precompiled executables for Windows is provided in the bin directory. 
Please note that even under Windows, EBD must be run from the command-line 
(i.e., the DOS prompt).

The ClusterTree program can be used to generating hierarchical cluster trees
from the dissimilarity matrices produced by EBD. The source code is contained
in the cluster_tree directory. Again, just run 'make' to build on OSX or Linux.
An exectuable is provided for Windows. 

ProjectTree can be used to project a tree onto a specific set of taxa. EBD
assumes a tree contain only taxa present in at least one sample. The source 
code is contained in the project_tree directory. Again, just run 'make' to 
build on OSX or Linux. An exectuable is provided for Windows. 


Program usage:
-------------------------------------------------------------------------------

Usage: EBD [OPTIONS]
Calculates taxon- and phylogenetic-basec beta diversity measures.

Options:
 -h, --help           Produce help message.
 -l, --list-calc      List all supported calculators.
 -u, --unit-tests     Execute unit tests.

 -t, --tree-file      Tree in Newick format (if phylogenetic beta-diversity is desired).
 -s, --seq-count-file Sequence count file.
 -d, --diss-file      File to write dissimilarity matrix to.

 -c, --calculator     Desired calculator (e.g., Bray-Curtis, Canberra).
 -w, --weighted       Indicated if sequence abundance data should be used.
 -m, --mrca           Apply 'MRCA weightings' to each branch (experimental).
 -r, --strict-mrca    Restrict calculator to MRCA subtree.
 -y, --count          Use count data as opposed to relative proportions.

 -x, --max-data-vecs  Maximum number of profiles (data vectors) to have in memory at once (default = 1000).
 
 -a, --all            Apply all calculators and cluster calculators at the specified threshold.
 -b, --threshold      Correlation threshold for clustering calculators (default = 0.8).
 -o, --output-file    File to write clusters to (default = clusters.txt.

 -v, --verbose        Provide additional information on program execution.

Examples of Use:
 ./ExpressBetaDiversity -t input.tre -s seq.txt -d output.txt -c Bray-Curtis -w
 ./ExpressBetaDiversity -t input.tre -s seq.txt -a -b 0.9 -o clusters.txt


 
Verifying software installation:
-------------------------------------------------------------------------------

A set of unit tests is included to verify proper installation of the EBD 
software. The unit tests can be run with:
  ./ExpressBetaDiversity -u

The software should not be used if any of the unit tests fail.


Projecting a phylogenetic tree:
-------------------------------------------------------------------------------

EBD assumes a tree spans only the taxa present in your set of samples. The
program ProjectTree can be used to ensure this is true. It can be run as 
follows:

  ./ProjectTree <seq-count-file> <input tree> <output tree>

The format of the <seq-count-file> is described in the next section (Input file
formats). It is the same file used by EBD to specify the distribution of 
sequences within each sample. The input tree must contain all taxa specified
in the sequence count file. ProjectTree prunes the tree to remove any
taxa not present in the sequence count file.


 
Input file formats:
-------------------------------------------------------------------------------

EBD uses Newick formatted trees as input. Information on this tree format can
be found at: http://evolution.genetics.washington.edu/phylip/newicktree.html.
Here is a simple Newick tree with three leaf nodes labelled A, B, and C:
 (A:1,(B:1,C:1):1);
 
Taxon-based beta-diversity is calculated if an input tree is not specified.

Sequence count information must be specified as a tab-delimited table where
each row is a sample and each column is the name of a leaf node in the provided
tree. Data must be provided for all leaf nodes in the tree. Consider the 
following example:

  A	B	C
Sample1	1	2	3
Sample2	10	1	0
Sample3	0	0	1

The first row begins indicates each leaf node in the tree seperated by a tab. 
Please note that this line MUST start with a tab. The number of sequences 
associated with each leaf node is then indicated for each sample on a seperate
row. In this example, the first sample is labelled 'Sample1' and contains 1
instance of sequence/OTU A, 2 instances of B, and 3 instances of C. Sample3
contains only instances of C, but note that zeros must be specified for the
other sequence/OTU types.

Example input files are avaliable in the unit-tests directory. 



Converting from UniFrac file format:
-------------------------------------------------------------------------------

The script convertUniFracToEBD.py in the scripts directory can be used to 
convert sample data formatted for the UniFrac or Fast UniFrac web services 
into the format expected by EBD. The script can be run as follows:

  ./convertUniFracToEBD.py <input UniFrac file> <ouput EBD file>



Dissimilarity output file format:
-------------------------------------------------------------------------------

The resulting dissimilarity between samples is written as a tab-delimited, 
lower-triangular dissimilarity matrix with the first line indicating the number
of samples. Consider the following output:

3
A
B	1
C	2	3

The first line indicates that there are 3 samples. The dissimilarity between 
samples A and B is 1, A and C is 2, and B and C is 3.



Building hierarchical cluster trees:
-------------------------------------------------------------------------------

A seperate program is provided for generating hierarchical cluster trees from
the dissimilarity matrices generated by EBD. The executable for this program
is provided in the bin directory. It can be invoked as follows:

  ./ClusterTree <method> <input matrix> <output tree>

Method can be either NJ, UPGMA, SingleLinkage, or CompleteLinkage.


Clustering output file format:
-------------------------------------------------------------------------------

The clustering file indicates clusters of calculators which are correlated. The 
clustering threshold is specified by the user with the --threshold (-b) 
parameter. All calculators in a cluster will be at least as correlated as the 
specified threshold. Results are reported as follows:  

Minimum r	Calculators
[0.0]	uChi-squared;
[0.86]	Canberra;CS;uCanberra;uCS;uGower;uManhattan;
[0.91]	uBray-Curtis;uSoergel;uKulczynski;
[0.81]	Bray-Curtis;Kulczynski;Soergel;
...

Complete linkage cluster tree (branch lengths are 1 - Pearson's correlation): 
((('Bray-Curtis':5.60596e-006,'Kulczynski':5.60596e-006):4.13975e-005 ...

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

Parks, D.H. and Beiko, R.G. Measures of phylogenetic differentiation provide 
  robust and complementary insights into microbial communities. ISME J.


Contact Information:
-------------------------------------------------------------------------------

Donovan Parks
parks@cs.dal.ca

Robert Beiko
beiko@cs.dal.ca

Program website: http://kiwi.cs.dal.ca/Software/EBD
