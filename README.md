# Multi Expression Programming library
**version 2024.04.18.0-beta**

Implements the Multi Expression Programming (MEP) technique for solving symbolic regression, classification (binary and multiclass), and time-series prediction problems. 

MEP is a [Genetic Programming (GP)](https://en.wikipedia.org/wiki/Genetic_programming) variant with a linear representation of chromosomes.

MEP introduced a unique feature: the ability to encode multiple solutions in the same chromosome. This means that we can explore much more from the search space compared to other techniques that encode a single solution in the chromosome. In most cases, this advantage comes with no penalty regarding running time or resources involved.

# To compile: 

C++ 11 is required due to the use of C++ 11 threads.

Create a new project and add:

- all files from [src](src) and 
- one file from [main](main) folder (which contains the **main** function). 

Include paths must point to the [include](include) folder of this project.

If you use the MS compiler, add _CRT_SECURE_NO_WARNINGS and _CRT_NONSTDC_NO_DEPRECATE to the preprocessor definitions.

# To run:

You need some a file with training data.
We provided several files (located in the [data](data) folder) for test:

- _bulding1.csv_ for symbolic regression problems,
- _cancer1.csv_ for binary classification problems with 0/1 output.
- _cancer1_output1-1.csv_ for binary classification problems with -1/1 output.
- _iris.txt_ for multiclass classification problems.
- _fibonacci.txt_ for univariate time-series.
- _wage\_growth.csv_ for multi-variate time-series.

Make sure that the instruction (from the **main** function):

	if (!training_data->from_tabular_file("../data/building1.csv")) ...
	
has the correct path of the file.

# Graphical user interface

**libmep** is used by [MEPX](https://mepx.org).

# Documentation

[https://github.com/mepx/libmep/wiki](https://github.com/mepx/libmep/wiki)

Documentation is currently obsoleted.
It will be updated soon.

# More info:

[mepx.org](https://mepx.org)

[mepx.github.io](https://mepx.github.io)

[https://github.com/mepx](https://github.com/mepx)

# Discussion Group:

[https://groups.google.com/d/forum/mepx](https://groups.google.com/d/forum/mepx)

# Contact author:

mihai.oltean@gmail.com