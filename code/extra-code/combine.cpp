#include <bits/stdc++.h>

using namespace std;
#define lld long long int
#define MAX_LEVEL 0
#define XX 1760
#define YY 1024
#define ZZ 1880

//ofstream my;
//my.open("myfile.txt", ofstream::out);

string joinxy(string f1, string f2, int level, lld tag, string file, lld width, lld length, lld depth) { //y - combination
	string filet;
//	string file;
	filet = file;
	f1 =  f1 + ".raw";
	f2 =   f2 + ".raw";
	f1 = "/media/nikita/ACE473A0E4736B88/divided/" + f1;
	f2 = "/media/nikita/ACE473A0E4736B88/divided/" + f2;

	ifstream one;
	one.open(f1.c_str(), ifstream::binary);
	ifstream two; 
	two.open(f2.c_str(), ifstream::binary);

	lld i, j, k;
	char *s = new char [width];
	char *s1 = new char [width];

	
	file = filet + ".raw";
	file = "/media/nikita/ACE473A0E4736B88/divided/" + file;

	ofstream ofs;
//	 string file = f1 + f2;
	ofs.open(file.c_str(), fstream::binary);

	if (tag == 0) {//0 & 1
		for (i = 0; i < depth; i++) {
			for (j = 0; j < length; j++) {
    			one.read(s, width);
    			two.read(s1, width);
    			ofs.write(s, width);
    			ofs.write(s1, width);
			}
		}	
	}
	
	if (tag == 1) {//0 & 2
		for (i = 0; i < depth; i++) {
			for (j = 0; j < length; j++) {
    			one.read(s, width);
    			ofs.write(s, width);
			}
			for (j = 0; j < length; j++) {
    			two.read(s, width);
    			ofs.write(s, width);
			}
		}	
	}
	
	if (tag == 2) {//0 & 4
		for (i = 0; i < depth; i++) {
			for (j = 0; j < length; j++) {
    			one.read(s, width);
    			ofs.write(s, width);
			}
		}
		for (i = 0; i < depth; i++) {
			for (j = 0; j < length; j++) {
    			two.read(s, width);
    			ofs.write(s, width);
			}
		}
	}
	lld si = width * depth * length;
	lld dd = si / (1024);
	cout << "size: " <<  dd << "  dimensions: " << width << " X " << length << " X " << depth << " ----> " << filet << endl;
	one.close();
	two.close();
	ofs.close();
	return filet;
}


string joinz(int level, string b0, string b1, string b2, string b3, string b4, string b5, string b6, string b7, string dig) {
	string py[4];
//	string dig;
	level = level;
	int level1 = 3;
	cout << level << endl;
	lld div1, div2, div3;
	// cout << "joinxy\n";
	div1 = pow(2, level1);
	div2 = pow(2, level1);
	div3 = pow(2, level1);
	
	lld x, y,z;
	x = XX/div1;
	y = YY/div1;
	z = ZZ/div3;
	
	string tmp = dig + "b01";
	py[0] = joinxy(b0, b1, level + 1, 0, tmp, x, y, z);
//	cout << b0 << " " << b1 << " " << b2 << " " << b3 << " " << b4 << " " << b5 << " " << b6 << " " << b7 << endl;;
	tmp = dig + "b23";
	py[1] = joinxy(b2, b3, level + 1, 0, tmp, x, y, z);
	tmp = dig + "b45";
	py[2] = joinxy(b4, b5, level + 1, 0, tmp, x, y, z);
	tmp = dig + "b67";
	py[3] = joinxy(b6, b7, level + 1, 0, tmp, x, y, z);
	string px[2];
	x = x * 2;
	tmp = dig + "y01";
	px[0] = joinxy(py[0], py[1], level + 1, 1, tmp, x, y, z);
	tmp = dig + "y23";
	px[1] = joinxy(py[2], py[3], level + 1, 1, tmp, x, y, z);
	tmp = dig + "x01";
	y = y * 2;
	string pz = joinxy(px[0], px[1], level + 1, 2, tmp, x, y, z);
	return pz;
}

string join_rec(int level, string val) {
	// cout << "level: " << level << endl;
	string p[8];
	string tmp = val;
	int i;
	if (level == MAX_LEVEL) {
		cout << "MAX_LEVEL\n";
		return joinz(level, tmp + '0', tmp + '1', tmp + '2', tmp + '3', tmp + '4', tmp + '5', tmp + '6', tmp + '7', tmp);
	}

	for (i = 0; i < 8; i++) {
		tmp = val + (char)(i + 48);
		p[i] = join_rec(level + 1, tmp);
//		cout << tmp << endl;
	}

	string out = joinz(level, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], val);
	cout << "complete 1 octant: " << out << "\n\n";
	return out;
}

int main() {
	ifstream one; 
	// one.open("test3.raw", ifstream::binary);
	freopen("out.txt", "w", stdout);
	ifstream two; 
	// two.open("test4567.raw", ifstream::binary);
	string start; 
	// stitching(one, two, 256, 440, 470, 2);
	cout << "stating point\n";
	cout << join_rec(0, "") << endl;
	return 0;
}
