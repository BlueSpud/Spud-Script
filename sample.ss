int ga = 0;
int gb = 2 * (2 + 5);

func test(int a, int c) {

	ga = a;
	gb = c;

	if (ga * gb) {

		prints(ga);
		prints(gb);
		prints(ga + gb);

	} else if (ga) {

	} else { prints(2); }

}


prints(ga);
prints(gb);

if (ga == gb) {

	prints(1);

} else {

	prints(2);

}

string s;

func poop(string n) {

	s = s + n;

}

poop(123 + 2245);
