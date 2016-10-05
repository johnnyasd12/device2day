#include <iostream>
#include <vector>
#include <set>
#include <tuple>
#include <string>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <exception>
#include "any.h"
#include "CSVReader.h"
#include "CSVWriter.h"
//#include <cunistd>

using namespace std;
using namespace any_type;


enum input_Label_CSV {
	HWADDR,
	COMETIME,
	GOTIME,
	STAYTIME,
	RSSI,
	HTTPPACKETS,
	HTTPINTERVAL,
};

enum output_Label_CSV {
	NAME,
	WEEKDAY,
	STAY1,
	STAY2,
	STAY3,
	STAY4,
	STAY5,
};



int main(int argc, char** argv) {
	if (argc < 2) { 
		cout << "Should have at least 1 argument with filename." << endl; 
		system("pause");
		return 1;
	}
	//====================many input files============================
	int numFile = argc - 1;
	fstream outputf;
	string ofname = "c2d.csv";
	vector<any> vvDays;
	//make csv header
	vvDays.push_back(vector<string>(numFile, ""));//one csv file per day
	vvDays.push_back(vector<int>(numFile, -1));//day of week
	vvDays.push_back(vector<int>(numFile, 0));//stay <30min, numFile of elements assigned to 0
	vvDays.push_back(vector<int>(numFile, 0));//stay 30~60min
	vvDays.push_back(vector<int>(numFile, 0));//stay 1~3hours
	vvDays.push_back(vector<int>(numFile, 0));//stay >3hours
	vvDays.push_back(vector<int>(numFile, 0));//stay <3min
	for (int fnum = 0; fnum < numFile; fnum++) {//for every input file
		fstream inputf;
		string inputfpath = argv[fnum + 1];
		string inputfname = inputfpath.substr(inputfpath.find_last_of("\\/")+1);//get filename from path

		clock_t start1;
		start1 = clock();
		
		any_cast<vector<string>>(vvDays[NAME])[fnum] = inputfname;
		inputf.open(inputfpath, std::ios::in);
		if (!inputf) {
			cout << "Cannot open file: " << inputfpath << endl;
			system("pause");
			return 1;
		}
		vector<vector<string>> data; //"HWAddress","Come Time","Go Time","Leave Time","HTTP Packets","HTTP Interval"
		readCSV(inputf, data);
		
		//get date
		time_t epocht = static_cast<time_t>(stod(data[COMETIME][1]));// epoch seconds
		struct tm tmptime;
		localtime_s(&tmptime,&epocht);//convert to time
		int tmpwday = tmptime.tm_wday;
		//cout << "week day = " << tmpwday << endl;
		any_cast<vector<int>>(vvDays[WEEKDAY])[fnum] = tmpwday;

		for (int i = 1; i < data[0].size(); i++) {
			//count device with staytime
			vector<int> stayRange(4, 0);//4 elements assigned to 0
			//fill(stayRange.begin(), stayRange.end(), 0);
			double tmp;
			tmp = stod(data[STAYTIME][i], nullptr);
			tmp = tmp / 60;
			if (tmp < 3) {
				any_cast<vector<int>>(vvDays[STAY5])[fnum]++;
			}else if (tmp < 30) {
				stayRange[0]++;
				any_cast<vector<int>>(vvDays[STAY1])[fnum]++;
			}
			else if (tmp < 60) {
				stayRange[1]++;
				any_cast<vector<int>>(vvDays[STAY2])[fnum]++;
			}
			else if (tmp < 180) {
				stayRange[2]++;
				any_cast<vector<int>>(vvDays[STAY3])[fnum]++;
			}
			else {
				stayRange[3]++;
				any_cast<vector<int>>(vvDays[STAY4])[fnum]++;
			}
		}
		/*
		for (int i = 0; i < stayRange.size(); i++) {
			any_cast<vector<int>>(vvDays[i]).push_back(stayRange[i]);
		}*/
		start1 = clock()-start1;
		cout << inputfname << " take " << start1 << " msec."<< endl;

	}
	//===================many input files=========================

	

	outputf.open(ofname, ios::out | ios::trunc);
	write_CSV(outputf, vector<string>({ "","weekday","<30min","30~60min","1~3h",">3h","<3min" }), vvDays);










	system("pause");
}
