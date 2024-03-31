# Multi Expression Programming library
**version 2024.03.31.0-beta**

Implements the Multi Expression Programming (MEP) technique for solving symbolic regression, classification (binary and multiclass) and time-series prediction problems. 

MEP is a [Genetic Programming (GP)](https://en.wikipedia.org/wiki/Genetic_programming) variant with linear representation of chromosomes.

MEP introduced a unique feature: the ability to encode multiple solutions in the same chromosome. This means that we can explore much more from the search space compared to other techniques which encode a single solution in the chromosome. In most cases this advantage comes with no penalty regarding the running time or the involved resources.

# To compile: 

C++ 11 is required due to the use of C++ 11 threads.

Create a new project and add:

- all files from [src](src) and 
- one file from [tests](tests) folder (which contains the **main** function). 

Include paths must point to the [include](include) folder of this project.

# To run:

You need some file with training data.
We provided several files (located in the [data](data) folder) for test:

- bulding1.csv for regression problems,
- cancer1.csv for classification problems.

Make sure that the instruction (from the **main** function):

	if (!training_data->from_csv("../data/building1.csv")) ...
	
has the correct path of the file.

# Documentation

[https://github.com/mepx/libmep/wiki](https://github.com/mepx/libmep/wiki)

It is obsolete.
It will be updated soon.

# More info:

[www.mepx.org](https://www.mepx.org)

[mepx.github.io](https://mepx.github.io)

[https://github.com/mepx](https://github.com/mepx)

# Discussion Group:

[https://groups.google.com/d/forum/mepx](https://groups.google.com/d/forum/mepx)

# Contact author:

mihai.oltean@gmail.com
