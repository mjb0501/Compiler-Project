//Enumerated list of possible token types
enum tkID
{
	idTk = 1001,
	numTK = 1002,
	opTK = 1003,
	kwTK = 1004,
	eofTK = 1005
};
//Token struct
struct tokeType
{
	enum tkID tokenID;
	char *tokenInstance;
	char *lineCharNum;
};
