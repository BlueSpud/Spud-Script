// Declare a variable
int a = 25;

// Print out a greeting, this is a function from C++
greeting();

// Print out numbers 0 through 4
// prints() is a C++ function as well
prints("Printing 1 through 4...");
for (int i = 0; i < 5; i = i + 1) {

	prints(i);

}

prints("Done!");

// Declare a function
func count(int c) {

	// Check if c is less than 5
	if (c < 5) {

		prints(c);
		count(c + 1);

	} else {

		// If c is greater than or equal to 5, we're done
		prints("Done!");

	}

}

// Print out numbers 0 through 4 with recursion
prints("Printing 1 through 4 with recursion...");
count(0);

func scriptFunction(int c) {

	// Return a call to a C++ function
	return cppFunction(c + 4, 7);

}

// Assign a to a function call to scriptFunction()
a = scriptFunction(4);
