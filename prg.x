
@"mod.x";


def a = 11+0;
def x = 0+a;

x = x + 0;
 
func add(def y, def j) {
	print y + j;
}

func printNum(def value) {
	print value;
}

func printHi() {
	print "Hi Ruslan!";
}

if (1==2) then {
	add(10, 20);
}
else {
	printHi();
	printNum(10);
	printNum(20);
}  

#* 

	block comment
	



print "Hello World!";

print "That's just a begin!!!!";

# line comment
# x = 12 + a + (23 * 45 + 12) + (a * (12 - 23)) + a * x;

while (x < 20)  {
	x = x + 1;
	if (x < 15) then {
		continue;
	}
	break;
}

print x;

def y = 0;
if (a == 11) then { 
	if ((x + a) == (x + x)) then { 
		x = 18;
		def y = 12;
		print y;
	}
	else {
	 	def y = 32;
	 	print y;
	}
		
}  
else 
	x = 12;
	
print x + y;


def simple = 12;
def complex = simple * 100;
def zetta = simple + complex;

simple = simple + complex / simple - zetta;
complex = complex + simple;

print complex;
*#