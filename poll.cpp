#include<iostream>
#include<string>
#include<cctype>
#include <cassert>
using namespace std;

bool isValidUppercaseStateCode(string stateCode);
bool hasProperSyntax(string pollData);
bool processRorDorI(string text, int& pos);
int tallySeats(string pollData, char party, int& seatTally);
void convertToUpperCase(string& pollData);

int main()
{
	assert(hasProperSyntax("CT5D,NY9R17D1I,VT,ne3r00D"));
	assert(!hasProperSyntax("ZT5D,NY9R17D1I,VT,ne3r00D"));
	assert(hasProperSyntax("")); //no state forecasts
	assert(hasProperSyntax("CT")); //one state forecast, no party results
	assert(hasProperSyntax("ct")); //state forecast lowercase
	assert(!hasProperSyntax("as")); //not a state forecast
	assert(hasProperSyntax("CT5D")); //one state forecast, one party result, one digit
	assert(hasProperSyntax("CT12D")); //one state forecast, one party result, two digits
	assert(!hasProperSyntax("CT123D")); //one state forecast, one party result, three digits
	assert(hasProperSyntax("CT12d")); //party result lowercase
	assert(hasProperSyntax("NY9R17D1I")); //one state forecast with multiple party results
	assert(!hasProperSyntax("NYR12D")); //no number before party code
	assert(hasProperSyntax("NY9R17D1I,VT,NJ3D5R4D")); //multiple state forecasts
	assert(!hasProperSyntax("NY9R17D1IVTNJ3D5R4D")); //no comma between state codes
	assert(!hasProperSyntax("KS4R, NV3D1R")); //space in poll data string
	assert(hasProperSyntax("KS4R,NV3D1G")); //letters other than D, R, or I
	

	int seats;
	seats = -999;    // so we can detect whether tallySeats sets seats

	assert(tallySeats("CT5D,NY9R17D1I,VT,ne3r00D", 'd', seats) == 0 && seats == 22);
	seats = -999;    // so we can detect whether tallySeats sets seats
	assert(tallySeats("CT5D,NY9R17D1I,VT,ne3r00D", '%', seats) == 2 && seats == -999); //party not a letter
	assert(tallySeats("CT7D,NY4D9R17D1I,VT,ne3r00D", 'r', seats) == 0 && seats == 12);
	assert(tallySeats("CT7D,NY4D9R17D1I,VT,ne3r0D9M", 'd', seats) == 0 && seats == 28);  //  use M for other minor parties
	assert(tallySeats("hello", 'd', seats) == 1); //pollData is not a poll data string
	assert(tallySeats("", 'd', seats) == 0); //no state forecasts
	assert(tallySeats("CT", 'd', seats) == 0); //one state forecast with no party results
	assert(tallySeats("CT5D", 'd', seats) == 0 && seats == 5); //one party result with one digit
	assert(tallySeats("CT53D", 'd', seats) == 0 && seats == 53); //one party result with two digits
	assert(tallySeats("CT123D", 'd', seats) == 1); //one party result with three digits
	assert(tallySeats("NY9R17D1I", 'd', seats) == 0 && seats == 17); //one state forecast, multiple party results
	assert(tallySeats("NYR", 'r', seats) == 1); //no number before party code
	assert(tallySeats("NY9R17D1I,VT,NJ3D5R4D", 'd', seats) == 0 && seats == 24); //multiple state forecasts
	assert(tallySeats("NY9G8D", 'g', seats) == 0 && seats == 9); //party other than D, R, or I
	assert(tallySeats("NY9R17D", 'I', seats) == 0 && seats == 0); //party letter not in poll data string

	cerr << "All tests succeeded" << endl;
}

//*************************************
//  isValidUppercaseStateCode
//*************************************

// Return true if the argument is a two-uppercase-letter state code, or
// false otherwise.

bool isValidUppercaseStateCode(string stateCode)
{
	const string codes =
		"AL.AK.AZ.AR.CA.CO.CT.DE.FL.GA.HI.ID.IL.IN.IA.KS.KY."
		"LA.ME.MD.MA.MI.MN.MS.MO.MT.NE.NV.NH.NJ.NM.NY.NC.ND."
		"OH.OK.OR.PA.RI.SC.SD.TN.TX.UT.VT.VA.WA.WV.WI.WY";
	return (stateCode.size() == 2 &&
		stateCode.find('.') == string::npos  &&  // no '.' in stateCode
		codes.find(stateCode) != string::npos);  // match found
}

bool hasProperSyntax(string pollData)
{
	int k = 0;
	convertToUpperCase(pollData);
	//if there are no state forecasts
	if (pollData == "")
		return true;
	//if there is one state forecast with no party results
	if (pollData.size() == 2 && isValidUppercaseStateCode(pollData))
		return true;
	//if there is one state forecast with party results
	if (isValidUppercaseStateCode(pollData.substr(k, 2)))
		k += 2;
	else
		return false;
	while (k != pollData.size())
	{
		if (!processRorDorI(pollData, k) && pollData[k] != ',')
			return false;
		else if (k >= pollData.size())  // make sure not out of bound
			return false;
		else if (isalpha(pollData[k])) // use alpha for all letters, D for Democratic, R for Republican, I for Independent, and other letters for various minor parties
		{
			if (k == pollData.size() - 1)
				return true;
			else
				k++;
		}
		else if (pollData[k] == ',')
		{
			k++;
			if (isValidUppercaseStateCode(pollData.substr(k, 2)))
				k += 2;
		}
		else //Party doesn't start with a letter, so error, D for Democratic, R for Republican, I for Independent, and other letters for various minor parties
			return false;

	}

	return false;
}

int tallySeats(string pollData, char party, int& seatTally)
{
	//pollData is not a poll data string
	if (!hasProperSyntax(pollData))
		return 1;
	//party is not a letter
	else if (!isalpha(party))
		return 2;
	else
	{
		seatTally = 0;
		party = toupper(party);
		convertToUpperCase(pollData);
		//calculate seatTally
		for (int i = 0; i < pollData.size(); i++)
		{
			if (pollData[i] == party)
			{
				seatTally += (pollData[i - 1] - '0'); //add digit in position before party letter
				if (isdigit(pollData[i - 2]))
					seatTally += (pollData[i - 2] - '0') * 10;//if party result is two digits
			}
		}
		return 0;
	}
}

void convertToUpperCase(string& pollData)
{
	//iterate through pollData and turn all lowercase letters to uppercase
	for (int i = 0; i != pollData.size(); i++)
	{
		if (isalpha(pollData[i]))
			pollData[i] = toupper(pollData[i]);
	}
}

bool processRorDorI(string text, int& pos)
{

	if (pos >= text.size()) return false;  // make sure not out of bound

	string vote = "";
	for (int i = 0; i < 2; i++) //party result has 0, 1, or 2 digits
	{
		//get votes
		if (isdigit(text[pos]))
		{
			vote += text[pos];
			if (pos<text.size() - 1)   // make sure not out of bound
				pos++;
			else
				break;
		}

	}
	if (vote == "")  // if there were no digits, error
		return false;
	return true;
}

