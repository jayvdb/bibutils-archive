int 
is_ws( char ch )
{
	if (ch==' ' || ch=='\t' || ch=='\r' || ch=='\n' ) return 1;
	else return 0;
}
