/* HTML 4.0 entities */
/* entity: &nbsp;  dec: &#160;   hex: &#xA0; */

typedef struct {
	char html[20];
	char decimal[8];
	char hex[9];
} entities;

entities html_entities[] = {
	/* Special Entities */
	{ "&quot;",   "&#34;",   "&#x22;"  },  /* quotation mark */
	{ "&amp;",    "&#38;",   "&#x26;"  },  /* ampersand */
	{ "&lt;",     "&#60;",   "&#x3C;"  },  /* less-than sign */
	{ "&gt;",     "&#62;",   "&#x3E;"  },  /* greater-than sign */
	{ "&OElig;",  "&#338;",  "&#x152;" },  /* Latin cap ligature OE */
	{ "&oelig;",  "&#339;",  "&#x153;" },  /* Latin small ligature OE */
	{ "&Scaron;", "&#352;",  "&#x160;" },  /* Latin cap S with caron */
	{ "&scaron;", "&#353;",  "&#x161;" },  /* Latin cap S with caron */
	{ "&Yuml;",   "&#376;",  "&#x178;" },  /* Latin cap y with diaeresis */
	{ "&circ;",   "&#710;",  "&#x2C6;" },  /* modifier letter circumflex */
	{ "&tilde;",  "&#732;",  "&#x2DC;" },  /* small tilde */
	{ "&ensp;",   "&#8194;", "&#x2002;" }, /* en space */
	{ "&emsp;",   "&#8195;", "&#x2003;" }, /* em space */
	{ "&thinsp;", "&#8201;", "&#x2009;" }, /* thin space */
	{ "&zwnj;",   "&#8204;", "&#x200C;" }, /* zero width non-joiner */
	{ "&zwj;",    "&#8205;", "&#x200D;" }, /* zero width joiner */
	{ "&lrm;",    "&#8206;", "&#x200E;" }, /* left-to-right mark */
	{ "&rlm;",    "&#8207;", "&#x200F;" }, /* right-to-left mark */
	{ "&ndash;",  "&#8211;", "&#x2013;" }, /* en dash */
	{ "&mdash;",  "&#8212;", "&#x2014;" }, /* em dash */
	{ "&lsquo;",  "&#8216;", "&#x2018;" }, /* left single quotation mark */
	{ "&rsquo;",  "&#8217;", "&#x2019;" }, /* right single quot. mark */
	{ "&sbquo;",  "&#8218;", "&#x201A;" }, /* single low-9 quot. mark */
	{ "&ldquo;",  "&#8220;", "&#x201C;" }, /* left double quot. mark */
	{ "&rdquo;",  "&#8221;", "&#x201D;" }, /* right double quot. mark */
	{ "&bdquo;",  "&#8222;", "&#x201E;" }, /* double low-9 quot. mark */
	{ "&dagger;", "&#8224;", "&#x2020;" }, /* dagger */
	{ "&Dagger;", "&#8225;", "&#x2021;" }, /* double dagger */
	{ "&permil;", "&#8240;", "&#x2030;" }, /* per mille sign */
	{ "&lsaquo;", "&#8249;", "&#x2039;" }, /* sin. left angle quot mark */
	{ "&rsaquo;", "&#8250;", "&#x203A;" }, /* sin. right angle quot mark */
	{ "&euro;",   "&#8364;", "&#x20AC;" }, /* euro sign */
	/* Symbols and Greek characters */
	{ "&fnof;",   "&#402;", "&#x192;" }, /* small f with hook = function */
	{ "&Alpha;",  "&#913;", "&#x391;" }, /* capital alpha */
	{ "&Beta;",   "&#914;", "&#x392;" }, /* capital beta */
	{ "&Gamma;",  "&#915;", "&#x393;" }, /* capital gamma */
	{ "&Delta;",  "&#916;", "&#x394;" }, /* capital delta */
	{ "&Epsilon;","&#917;", "&#x395;" }, /* capital epsilon */
	{ "&Zeta;",   "&#918;", "&#x396;" }, /* capital zeta */
	{ "&Eta;",    "&#919;", "&#x397;" }, /* capital eta */
	{ "&Theta;",  "&#920;", "&#x398;" }, /* capital theta */
	{ "&Iota;",   "&#921;", "&#x399;" }, /* capital iota */
	{ "&Kappa;",  "&#922;", "&#x39A;" }, /* capital kappa */
	{ "&Lambda;", "&#923;", "&#x39B;" }, /* capital lambda */
	{ "&Mu;",     "&#924;", "&#x39C;" }, /* capital mu */
	{ "&Nu;",     "&#925;", "&#x39D;" }, /* capital nu */
	{ "&Xi;",     "&#926;", "&#x39E;" }, /* capital xi */
	{ "&Omicron;","&#927;", "&#x39F;" }, /* capital omicron */
	{ "&Pi;",     "&#928;", "&#x3A0;" }, /* capital pi */
	{ "&Rho;",    "&#929;", "&#x3A1;" }, /* capital rho */
	{ "&Sigma;",  "&#931;", "&#x3A3;" }, /* capital sigma */
	{ "&Tau;",    "&#932;", "&#x3A4;" }, /* capital tau */
	{ "&Upsilon;","&#933;", "&#x3A5;" }, /* capital upsilon */
	{ "&Phi;",    "&#934;", "&#x3A6;" }, /* capital phi */
	{ "&Chi;",    "&#935;", "&#x3A7;" }, /* capital chi */
	{ "&Psi;",    "&#936;", "&#x3A8;" }, /* capital psi */
	{ "&Omega;",  "&#937;", "&#x3A9;" }, /* capital omega */
	{ "&alpha;",  "&#945;", "&#x3B1;" }, /* small alpha */
	{ "&beta;",   "&#946;", "&#x3B2;" }, /* small beta */
	{ "&gamma;",  "&#947;", "&#x3B3;" }, /* small gamma */
	{ "&delta;",  "&#948;", "&#x3B4;" }, /* small delta */
	{ "&epsilon;","&#949;", "&#x3B5;" }, /* small epsilon */
	{ "&zeta;",   "&#950;", "&#x3B6;" }, /* small zeta */
	{ "&eta;",    "&#951;", "&#x3B7;" }, /* small eta */
	{ "&theta;",  "&#952;", "&#x3B8;" }, /* small theta */
	{ "&iota;",   "&#953;", "&#x3B9;" }, /* small iota */
	{ "&kappa;",  "&#954;", "&#x3BA;" }, /* small kappa */
	{ "&lambda;", "&#955;", "&#x3BB;" }, /* small lambda */
	{ "&mu;",     "&#956;", "&#x3BC;" }, /* small mu */
	{ "&nu;",     "&#957;", "&#x3BD;" }, /* small nu */
	{ "&xi;",     "&#958;", "&#x3BE;" }, /* small xi */
	{ "&omicron;","&#959;", "&#x3BF;" }, /* small omicron */
	{ "&pi;",     "&#960;", "&#x3C0;" }, /* small pi */
	{ "&rho;",    "&#961;", "&#x3C1;" }, /* small rho */
	{ "&sigmaf;", "&#962;", "&#x3C2;" }, /* small final sigma */
	{ "&sigma;",  "&#963;", "&#x3C3;" }, /* small simga */
	{ "&tau;",    "&#964;", "&#x3C4;" }, /* small tau */
	{ "&upsilon;","&#965;", "&#x3C5;" }, /* small upsilon */
	{ "&phi;",    "&#966;", "&#x3C6;" }, /* small phi */
	{ "&chi;",    "&#967;", "&#x3C7;" }, /* small chi */
	{ "&psi;",    "&#968;", "&#x3C8;" }, /* small psi */
	{ "&omega;",  "&#969;", "&#x3C9;" }, /* small omega */
	{ "&thetasym;","&#977;","&#x3D1;" }, /* small theta symbol */
	{ "&upsih;",  "&#978;", "&#x3D2;" }, /* small upsilon with hook */
	{ "&piv;",    "&#982;", "&#x3D6;" }, /* pi symbol */
	{ "&bull;",   "&#8226;","&#x2022;"}, /* bullet = small blk circle */
	{ "&hellip;", "&#8230;","&#x2026;"}, /* horizontal ellipsis */
	{ "&prime;",  "&#8242;","&#x2032;"}, /* prime = minutes = feet */
	{ "&Prime;",  "&#8243;","&#x2033;"}, /* double prime */
	{ "&oline;",  "&#8254;","&#x203E;"}, /* overline */
	{ "&frasl;",  "&#8260;","&#x2044;"}, /* fraction slash */
	{ "&weierp;", "&#8472;","&#x2118;"}, /* Weierstrass p = power set */
	{ "&image;",  "&#8465;","&#x2111;"}, /* imaginary part-black cap I */
	{ "&real;",   "&#8476;","&#x211C;"}, /* real part-black cap R */
	{ "&trade;",  "&#8482;","&#x2122;"}, /* trademark sign */
	{ "&alefsym;","&#8501;","&#x2135;"}, /* alef symbol */
	{ "&larr;",   "&#8592;","&#x2190;"}, /* left arrow */
	{ "&uarr;",   "&#8593;","&#x2191;"}, /* up arrow */
	{ "&rarr;",   "&#8594;","&#x2192;"}, /* right arrow */
	{ "&darr;",   "&#8595;","&#x2193;"}, /* down arrow */
	{ "&harr;",   "&#8596;","&#x2194;"}, /* left/right arrow */
	{ "&crarr;",  "&#8629;","&#x21B5;"}, /* down arrow with corner left */
	{ "&lArr;",   "&#8656;","&#x21D0;"}, /* left double arrow */
	{ "&uArr;",   "&#8657;","&#x21D1;"}, /* up double arrow */
	{ "&rArr;",   "&#8658;","&#x21D2;"}, /* up double arrow */
	{ "&dArr;",   "&#8659;","&#x21D3;"}, /* up double arrow */
	{ "&hArr;",   "&#8660;","&#x21D4;"}, /* up double arrow */
	{ "&forall;", "&#8704;","&#x2200;"}, /* for all */
	{ "&part;",   "&#8706;","&#x2202;"}, /* partial differential */
	{ "&exist;",  "&#8707;","&#x2203;"}, /* there exists */
	{ "&empty;",  "&#8709;","&#x2205;"}, /* empty set */
	{ "&nabla;",  "&#8711;","&#x2207;"}, /* nabla=backwards difference */
	{ "&isin;",   "&#8712;","&#x2208;"}, /* element of */
	{ "&notin;",  "&#8713;","&#x2209;"}, /* not an element of */
	{ "&ni;",     "&#8715;","&#x220B;"}, /* contains as member */
	{ "&prod;",   "&#8719;","&#x220F;"}, /* n-ary product */
	{ "&sum;",    "&#8721;","&#x2211;"}, /* n-ary summation */
	{ "&minus;",  "&#8722;","&#x2212;"}, /* minuss sign */
	{ "&lowast;", "&#8727;","&#x2217;"}, /* asterisk operator */
	{ "&radic;",  "&#8730;","&#x221A;"}, /* square root */
	{ "&prop;",   "&#8733;","&#x221D;"}, /* proportional to */
	{ "&infin;",  "&#8734;","&#x221E;"}, /* infinity */
	{ "&ang;",    "&#8736;","&#x2220;"}, /* angle */
	{ "&and;",    "&#8743;","&#x2227;"}, /* logical and */
	{ "&or;",     "&#8744;","&#x2228;"}, /* logical or */
	{ "&cap;",    "&#8745;","&#x2229;"}, /* intersection */
	{ "&cup;",    "&#8746;","&#x222A;"}, /* union */
	{ "&int;",    "&#8747;","&#x222B;"}, /* integral */
	{ "&there4;", "&#8756;","&#x2234;"}, /* therefore */
	{ "&sim;",    "&#8764;","&#x223C;"}, /* tilde operator */
	{ "&cong;",   "&#8773;","&#x2245;"}, /* approximately equal to */
	{ "&asymp;",  "&#8776;","&#x2248;"}, /* asymptotic to */
	{ "&ne;",     "&#8800;","&#x2260;"}, /* not equal to */
	{ "&equiv;",  "&#8801;","&#x2261;"}, /* identical to */
	{ "&le;",     "&#8804;","&#x2264;"}, /* less-than or equal to */
	{ "&ge;",     "&#8805;","&#x2265;"}, /* greater-than or equal to */
	{ "&sub;",    "&#8834;","&#x2282;"}, /* subset of */
	{ "&sup;",    "&#8835;","&#x2283;"}, /* superset of */
	{ "&nsub;",   "&#8836;","&#x2284;"}, /* not a subset of */
	{ "&sube;",   "&#8838;","&#x2286;"}, /* subset of or equal to */
	{ "&supe;",   "&#8839;","&#x2287;"}, /* superset of or equal to */
	{ "&oplus;",  "&#8853;","&#x2295;"}, /* circled plus = direct sum */
	{ "&otimes;", "&#8855;","&#x2297;"}, /* circled times = vec prod */
	{ "&perp;",   "&#8869;","&#x22A5;"}, /* perpendicular */
	{ "&sdot;",   "&#8901;","&#x22C5;"}, /* dot operator */
	{ "&lceil;",  "&#8968;","&#x2308;"}, /* left ceiling */
	{ "&rceil;",  "&#8969;","&#x2309;"}, /* right ceiling */
	{ "&lfloor;", "&#8970;","&#x230A;"}, /* left floor */
	{ "&rfloor;", "&#8971;","&#x230B;"}, /* right floor */
	{ "&lang;",   "&#9001;","&#x2329;"}, /* left angle bracket */
	{ "&rang;",   "&#9002;","&#x232A;"}, /* right angle bracket */
	{ "&loz;",    "&#9674;","&#x25CA;"}, /* lozenge */
	{ "&spades;", "&#9824;","&#x2660;"}, /* spades */
	{ "&clubs;",  "&#9827;","&#x2663;"}, /* clubs */
	{ "&hearts;", "&#9829;","&#x2665;"}, /* hearts */
	{ "&diams;",  "&#9830;","&#x2666;"}, /* diamonds */
	/* Latin-1 */
	{ "&nbsp;",   "&#160;", "&#xA0;" },  /* non-breaking space */
	{ "&iexcl;",  "&#161;", "&#xA1;" },  /* inverted exclamation mark */
	{ "&cent;",   "&#162;", "&#xA2;" },  /* cent sign */
	{ "&pound;",  "&#163;", "&#xA3;" },  /* pound sign */
	{ "&curren;", "&#164;", "&#xA4;" },  /* currency sign */
	{ "&yen;",    "&#165;", "&#xA5;" },  /* yen sign */
	{ "&brvbar;", "&#166;", "&#xA6;" },  /* broken vertical bar */
	{ "&sect;",   "&#167;", "&#xA7;" },  /* section sign */
	{ "&uml;",    "&#168;", "&#xA8;" },  /* diaeresis - spacing diaeresis */
	{ "&copy;",   "&#169;", "&#xA9;" },  /* copyright sign */
	{ "&ordf;",   "&#170;", "&#xAA;" },  /* feminine ordinal indicator */
	{ "&laquo;",  "&#171;", "&#xAB;" },  /* left-pointing guillemet */
	{ "&not;",    "&#172;", "&#xAC;" },  /* not sign */
	{ "&shy;",    "&#173;", "&#xAD;" },  /* soft (discretionary) hyphen */
	{ "&reg;",    "&#174;", "&#xAE;" },  /* registered sign */
	{ "&macr;",   "&#175;", "&#xAF;" },  /* macron = overline */
	{ "&deg;",    "&#176;", "&#xB0;" },  /* degree sign */
	{ "&plusmn;", "&#177;", "&#xB1;" },  /* plus-minus sign */
	{ "&sup2;",   "&#178;", "&#xB2;" },  /* superscript two */
	{ "&sup3;",   "&#179;", "&#xB3;" },  /* superscript three */
	{ "&acute;",  "&#180;", "&#xB4;" },  /* acute accent = spacing acute */
	{ "&micro;",  "&#181;", "&#xB5;" },  /* micro sign */
	{ "&para;",   "&#182;", "&#xB6;" },  /* pilcrow (paragraph) sign */
	{ "&middot;", "&#183;", "&#xB7;" },  /* middle dot (georgian comma) */
	{ "&cedil;",  "&#184;", "&#xB8;" },  /* cedilla = spacing cedilla */
	{ "&sup1;",   "&#185;", "&#xB9;" },  /* superscript one */
	{ "&ordm;",   "&#186;", "&#xBA;" },  /* masculine ordinal indicator */
	{ "&raquo;",  "&#187;", "&#xBB;" },  /* right pointing guillemet */
	{ "&frac14;", "&#188;", "&#xBC;" },  /* 1/4 */
	{ "&frac12;", "&#189;", "&#xBD;" },  /* 1/2 */
	{ "&frac34;", "&#190;", "&#xBE;" },  /* 3/4 */
	{ "&iquest;", "&#191;", "&#xBF;" },  /* inverted question mark */
	{ "&Agrave;", "&#192;", "&#xC0;" },  /* cap A with grave */
	{ "&Aacute;", "&#193;", "&#xC1;" },  /* cap A with acute */
	{ "&Acirc;",  "&#194;", "&#xC2;" },  /* cap A with circumflex */
	{ "&Atilde;", "&#195;", "&#xC3;" },  /* cap A with tilde */
	{ "&Auml;",   "&#196;", "&#xC4;" },  /* cap A with diaeresis */
	{ "&Aring;",  "&#197;", "&#xC5;" },  /* cap A with ring */
	{ "&AElig;",  "&#198;", "&#xC6;" },  /* cap AE ligature */
	{ "&Ccedil;", "&#199;", "&#xC7;" },  /* cap C with cedilla */
	{ "&Egrave;", "&#200;", "&#xC8;" },  /* cap E with grave */
	{ "&Eacute;", "&#201;", "&#xC9;" },  /* cap E with acute */
	{ "&Ecirc;",  "&#202;", "&#xCA;" },  /* cap E with circumflex */
	{ "&Euml;",   "&#203;", "&#xCB;" },  /* cap E with diaeresis */
	{ "&Igrave;", "&#204;", "&#xCC;" },  /* cap I with grave */
	{ "&Iacute;", "&#205;", "&#xCD;" },  /* cap I with acute */
	{ "&Icirc;",  "&#206;", "&#xCE;" },  /* cap I with circumflex */
	{ "&Iuml;",   "&#207;", "&#xCF;" },  /* cap I with diaeresis */
	{ "&ETH;",    "&#208;", "&#xD0;" },  /* cap letter ETH */
	{ "&Ntilde;", "&#209;", "&#xD1;" },  /* cap N with tilde */
	{ "&Ograve;", "&#210;", "&#xD2;" },  /* cap O with grave */
	{ "&Oacute;", "&#211;", "&#xD3;" },  /* cap O with acute */
	{ "&Ocirc;",  "&#212;", "&#xD4;" },  /* cap O with circumflex */
	{ "&Otilde;", "&#213;", "&#xD5;" },  /* cap O with tilde */
	{ "&Ouml;",   "&#214;", "&#xD6;" },  /* cap O with diaeresis */
	{ "&times;",  "&#215;", "&#xD7;" },  /* multiplication sign */
	{ "&Oslash;", "&#216;", "&#xD8;" },  /* cap O with stroke */
	{ "&Ugrave;", "&#217;", "&#xD9;" },  /* cap U with grave */
	{ "&Uacute;", "&#218;", "&#xDA;" },  /* cap U with acute */
	{ "&Ucirc;",  "&#219;", "&#xDB;" },  /* cap U with circumflex */
	{ "&Uuml;",   "&#220;", "&#xDC;" },  /* cap U with diaeresis */
	{ "&Yacute;", "&#221;", "&#xDD;" },  /* cap Y with acute */
	{ "&THORN;",  "&#222;", "&#xDE;" },  /* cap letter THORN */
	{ "&szlig;",  "&#223;", "&#xDF;" },  /* small sharp s = ess-zed */
	{ "&agrave;", "&#224;", "&#xE0;" },  /* small a with grave */
	{ "&aacute;", "&#225;", "&#xE1;" },  /* small a with acute */
	{ "&acirc;",  "&#226;", "&#xE2;" },  /* small a with cirucmflex */
	{ "&atilde;", "&#227;", "&#xE3;" },  /* small a with tilde */
	{ "&amul;",   "&#228;", "&#xE4;" },  /* small a with diaeresis */
	{ "&aring;",  "&#229;", "&#xE5;" },  /* small a with ring */
	{ "&aelig;",  "&#230;", "&#xE6;" },  /* small ligature ae */
	{ "&ccedil;", "&#231;", "&#xE7;" },  /* small c with cedilla */
	{ "&egrave;", "&#232;", "&#xE8;" },  /* small e with grave */
	{ "&eacute;", "&#233;", "&#xE9;" },  /* small e with acute */
	{ "&ecirc;",  "&#234;", "&#xEA;" },  /* small e with circumflex */
	{ "&emul;",   "&#235;", "&#xEB;" },  /* small e with diaeresis */
	{ "&igrave;", "&#236;", "&#xEC;" },  /* small i with grave */
	{ "&iacute;", "&#237;", "&#xED;" },  /* small i with acute */
	{ "&icirc;",  "&#238;", "&#xEE;" },  /* small i with circumflex */
	{ "&iuml;",   "&#239;", "&#xEF;" },  /* small i with diaeresis */
	{ "&eth;",    "&#240;", "&#xF0;" },  /* latin small letter eth */
	{ "&ntilde;", "&#241;", "&#xF1;" },  /* small n with tilde */
	{ "&ograve;", "&#242;", "&#xF2;" },  /* small o with grave */
	{ "&oacute;", "&#243;", "&#xF3;" },  /* small o with acute */
	{ "&ocirc;",  "&#244;", "&#xF4;" },  /* small o with circumflex */
	{ "&otilde;", "&#245;", "&#xF5;" },  /* small o with tilde */
	{ "&ouml;",   "&#246;", "&#xF6;" },  /* small o with diaeresis */
	{ "&divide;", "&#247;", "&#xF7;" },  /* division sign */
	{ "&oslash;", "&#248;", "&#xF8;" },  /* small o with slash */
	{ "&ugrave;", "&#249;", "&#xF9;" },  /* small u with grave */
	{ "&uacute;", "&#250;", "&#xFA;" },  /* small u with acute */
	{ "&ucirc;",  "&#251;", "&#xFB;" },  /* small u with circumflex */
	{ "&uuml;",   "&#252;", "&#xFC;" },  /* small u with diaeresis */
	{ "&yacute;", "&#253;", "&#xFD;" },  /* small y with acute */
	{ "&thorn;",  "&#254;", "&#xFE;" },  /* latin small letter thorn */
	{ "&yuml;",   "&#255;", "&#xFF;" },  /* small y with diaeresis */
};

int nhtml_entities = sizeof( html_entities ) / sizeof( entities );

#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>

int
main( void )
{
	char *html, *dec, *hex;
	int i, j, err=0, html_len, dec_len, hex_len, dec_val, hex_val;
	for ( i=0; i<nhtml_entities; ++i ) {
		html = &(html_entities[i].html[0]);
		html_len = strlen( html );
		if ( html[0]!='&' || html[html_len-1]!=';' ) {
			printf("%d %s: error in html '%s'\n",i+1,html,html);
			err ++;
		}
		dec  = &(html_entities[i].decimal[0]);
		dec_len = strlen( dec );
		if ( dec[0]!='&' || dec[1]!='#' || dec[dec_len-1]!=';' ) {
			printf("%d %s: error in dec '%s'\n",i+1,html,dec);
			err ++;
		}
		for ( j=2; j<dec_len-1; ++j )
			if ( !isdigit(dec[j]) ) {
				printf("%d %s: error in dec '%s'\n",i+1,html,dec);
				err ++;
			}
		hex  = &(html_entities[i].hex[0]);
		hex_len = strlen( hex );
		if ( hex[0]!='&' || hex[1]!='#' || hex[2]!='x' || hex[hex_len-1]!=';' ) {
			printf("%d %s: error in hex '%s'\n",i+1,html,hex);
			err ++;
		}
		for ( j=3; j<hex_len-1; ++j )
			if ( !isdigit(hex[j]) && !(hex[j]>='A' && hex[j]<='F') ) {
				printf("%d %s: error in hex '%s'\n",i+1,html,hex);
				err ++;
			}
	
		dec_val = atoi( &(dec[2]) );
		sscanf(&(hex[3]),"%x",&hex_val);
		if ( dec_val!=hex_val ) {
			printf("%d %s: disagreement in dec '%s'=%d and hex '%s'=%d\n",i+1,html,dec,dec_val,hex,hex_val);
			err ++;
		}
		for ( j=i+1; j<nhtml_entities; ++j ) {
			if ( !strcmp( html, &(html_entities[j].html[0]) )) {
				printf("%d %s: duplicate html at %d %s\n",
					i+1, html, j+1, &(html_entities[j].html[0]));
				err++;
			}
			if ( !strcmp( dec, &(html_entities[j].decimal[0]) )) {
				printf("%d %s: duplicate dec at %d %s\n",
					i+1, dec, j+1, &(html_entities[j].decimal[0]));
				err++;
			}
			if ( !strcmp( hex, &(html_entities[j].hex[0]) )) {
				printf("%d %s: duplicate hex at %d %s\n",
					i+1, hex, j+1, &(html_entities[j].hex[0]));
				err++;
			}
	
		}
	}
	printf("Number HTML entities = %d, errors = %d\n",nhtml_entities,err);
}

#endif
