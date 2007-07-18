typedef class parser
{
public:
	void parseline(char *xbuffer);//Parses the line
	bool getlineelement(int element, char **tbuffer, int *length);//Get element from parsed line
private:
	int currmember,totalmembers;
	char *memberstart[10], *memberend[10];//Pointers for up to ten members
	char parserbuffer[40];
} PARSER;
