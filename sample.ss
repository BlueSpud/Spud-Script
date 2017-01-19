func test(int a) {

	int b = 0;

	if (a < 5) {

		b = b + 1;

		prints(a);
		a = a + 1;
		test(a);

	}

	prints(b);

}

test(1);
