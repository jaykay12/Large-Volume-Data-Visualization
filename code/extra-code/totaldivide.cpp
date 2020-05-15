#include <bits/stdc++.h>

using namespace std;

#define width 1760
#define height 1024
#define depth 1880


typedef unsigned char VolumeType;
int b[3];
VolumeType *d2;
ofstream checking;
//to get the binary value
void binary(long dec) {
	b[0] = 0;
	b[1] = 0;
	b[2] = 0;
	long rem,i=0,sum=0, ll;
	ll = dec;
    do
    {
        rem=dec%2;
        dec=dec/2;
		b[i]= rem;
		i++;
    } while (dec>0);
}

void print(long int i, char *ss, int bri, int inc) {
	long int j, k, l, t, p;
	t = i;
	ifstream fin;
	fin.open("VH_1760_1024_1880.raw",  fstream::binary);
	ofstream fout;
	fout.open(ss,  fstream::binary);
	char gg;
	int flag = 0;
	for (j = 0; j < depth/bri; j = j  + inc) {
		i = t + j * height * width;
		for (k = 0; k < height/bri; k = k + inc) {
			p = i + k * width;
			fin.seekg(p, fin.beg);
			for (l = 0; l < width/bri; l = l + inc) {
				fin.seekg(inc, fin.cur);
				gg  = fin.get();
				if (gg != '\0') {
					flag  = 1;
				}
				fout.put(gg);
			}
		}
	}
	if (flag == 0) {
		checking << ss << " " << 0 << endl;
	} else {
		checking << ss << " " << 1 << endl;
	}
	//checking << "nikni";
	//cout << "jkvfdjknddkjbndbn\n\n\n";
	fin.close();
	fout.close();
}

void print8(long int i, char *ss) {
	long int j, k, l, t;
	t = i;
	ifstream fin;
	//ofstream fintemp;
	fin.open("VH_1760_1024_1880.raw",  fstream::binary);
	ofstream fout;
	fout.open(ss,  fstream::binary);
	//cout << fout << endl;
	char *s = new char [width/8];
	// cout <<  ss << "\n";
	int flag = 0;

	long int size = width/8;
	// cout << size << endl;
	for (j = 0; j < depth/8; j++) {
		i = t + j * height * width;
		for (k = 0; k < height/8; k++) {
			i = i + width;
			fin.seekg(i, fin.beg);
			fin.read(s, size);
			if (strcmp(s, "") != 0) {
				// cout << s ;
				flag = 1;
			} 
			fout.write(s, size);
		}
	}
	if (flag == 0) {
		checking << ss << " " << 0 << endl;
	} else {
		checking << ss << " " << 1 << endl;
	}
	
	fin.close();
	fout.close();
	cout << "\nhello\n";
	cout << endl;
}

int main() 
{
	FILE *fp;
	checking.open("check.txt");
	string kk, ss;
	unsigned long long int j, k, l, jval, kval, lval;
	//cout << "niki";
	unsigned long long int aa, bb, cc;

	char rstr[20];
	sprintf(rstr, "root.raw");
	print(0, rstr, 1, 8);
//	cout << "klnl";
//exit(0);
	for (j = 0; j < 8; j++) {
		binary(j);
		aa = width * height;
		aa = aa *  depth;
		aa = aa / 2;
		bb = width * height;
		bb = bb / 2;
		char str1[20];
		jval = (b[2])* aa + (b[1])* bb + (b[0])* width/2;
		sprintf(str1, "/media/nikita/ACE473A0E4736B88/divided/first_level/%lld.raw", j);
		cout << str1 << endl;
		print(jval, str1, 2, 4);
		exit(0);
		continue;
		for (k = 0; k < 8; k++) {
			binary(k);
			aa = width * height;
			aa = aa *  depth;
			aa = aa / 4;
			bb = width * height;
			bb = bb / 4;
			char str2[20];
			kval = jval + (b[2])* aa +  (b[1])* bb + (b[0])* width/4;
			sprintf(str2, "/media/nikita/ACE473A0E4736B88/divided/%lld%lld.raw", j,k);
			print(kval, str2, 4, 2);

			for (l = 0; l < 8; l++) {
				binary(l);
				aa = width * height;
				aa = aa *  depth;
				aa = aa / 8;
				bb = width * height;
				bb = bb / 8;
				char str3[20];
				lval = kval + (b[2])*aa + (b[1])* bb + (b[0])* width/8;
				
				sprintf(str3, "/media/nikita/ACE473A0E4736B88/divided/%lld%lld%lld.raw", j,k,l);
				print8(lval, str3);
				//exit(0);
			}
		}
	}	
	checking.close();
	return 0;
}

