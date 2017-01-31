func test() {

	prints("Calling the function test");
	return 10;

}

int a = test();
prints(a);

a = cppReturn();
prints(a);

Test t;

prints(t.o.a);
