/*  GIMP header image file format (RGB): C:\Users\BTH\Desktop\water.h  */

static const unsigned int WATER_WIDTH = 64;
static const unsigned int WATER_HEIGHT = 64;

/*  Call this macro repeatedly.  After each use, the pixel data can be extracted  */

#define HEADER_PIXEL(data,pixel) {\
pixel[0] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4)); \
pixel[1] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)); \
pixel[2] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33))); \
data += 4; \
}
static const char *header_data =
"M.+[W@TELN8!D<?BB+_=A[[<C\\?IB\\'DB[_EBK[D@;G=B,+FB\\?HC,CIB<CHD,WK"
"FM/PL.<#G]#OGLOKQ^`1X0PLIMCTD,CBC\\;CGM7RI=CVI=CVJN'_C\\KGCLOIC\\[N"
"C,;JD,CLDL?JE<KKF=#NC\\KGC\\SJAL7CB</GC<?KELSQH-;YH-7VELGGA[30P?$+"
"NN`.FM'QG=#OP?43U@\\JN_,/JM?UKMOYFM+LG-3NI]CXLN0#F]/SG]SZH-CZJN,$"
"J-3PV@<BJMWYE\\[KB\\3ACL;FC<7GB\\'DC,+GB;_DB\\7IB,3EA<'BC<GJD\\[OD,CH"
"ELWKIMSYJ-;VR/04[@`WN^<'E<?CB\\'<H]GVM.L'OO(/L>4\"I-GXEL_LC\\KIBL;G"
"D,SOB</GD\\GLFM'QH-CXI>#_DL_ME=3RE='RD,OLD,;II-GZG]+QC\\#?S`(=N>T("
"BL'?A[[>G]+QH];U@KG6H]GVS/H7S?L8I-KUEL[HO_$/H-'QL.D(L.P*L>T-JN,$"
"GL7EY``QI]GUG-7QC\\_JA\\;DB\\'DC\\7HC<CIA\\/DAL+C@K[?C\\OLD<SKD\\7GF<KJ"
"H=3SKN'`U@$A\\0`ZR/04JM?WG,KJO_$/SO`>O?$.L^<%J-OZH]3TI]KYE\\_OB<;D"
"B<7FAL+CC,?HD,OLH-CZG]KYD]#ND]+PCL_LE=3RF]+PR?L:L=[^U0,BN.`,DL[I"
"C<CGCL;FB\\#?E\\SKG]7RD\\;DK=SYM./`KN']M.@#J]S[J][]E\\[NB<3CD,OLF=3U"
"SO,5[P`XR_H9L^L(IN0!C,GGD<?JF<[QC<7GC,3FDLKLE<WMG-3TH-7TKMS]N.4&"
"S/`>W``PU`,CS/46N.0$O>L*V08EU0,BL>,!H];THM7TGM'PF,GIGL_OG=3THMW\\"
"G-GWG=KXD<SMEM'RGM;XDLWNB\\CFB<CFALCBB\\CCE<CFP.X+[``[K-W\\D,OHB\\KH"
"CLOIF='QG-/SE,GHE\\SKGM'OF,;FJ=CUMND$T0,>L^4#F<SJC\\;DD,CHD\\[OC\\KK"
"S?07T/D;F,3FM^@*Q/<8LN@(E<ONE\\SOI]?ZNNL-O.T/Q/85S?P<T?`?T/\\>S/X<"
"PO84I]KYM^8&Q?(3V00ET_`AP.\\/I=CWG=#OF,OJE<CGELGHH-+TH]CYIMW]J.#`"
"J>4!IN'`G]?YF,[QD<GMD<SM?KO9B<;BB,'=CL3=D\\#<V@8APN`-H-/RA\\3BD-'P"
"E]/TE]+QF='QE,OIFLWLI]CWH<_OI=/SONT,Y0`RL>,!I=CVG=3RF]3QE=#OF-/R"
"S?07JM7WEL3GC\\'CI-;XKN0$OO47R?\\AT/\\AQ?(5Q?04R/H9LN#`K^#_J=SZBL#="
"F<[MD<;EH=3SD\\3DQ?04JMCYG=#OG-'PE\\[LE,OIC\\3CE,GHHMGYH-?WFM'QF-#P"
"F=3QL>H)K^<&ELONC</FA+S>C,?FBL'?CL#<FL?CVP0>RO<2J-GXE\\[LC,CIC<SN"
"D\\_PD\\[ODLKJDLGIE,?FG,WMR/<6Q?03RO@7ZP`XO>\\-G]+PJ-W\\K^D%F]/SD,OJ"
"MM[`L=[^G-'PC,7BD]#LF-7QI]__H=;WG,SOD\\/FF<SKD\\CGB+_=C,CCC\\OFE-#K"
"F]3QE,SLDLKJG-/SIMCZF,KLD<CHDLKJD,OJC,?FCL7EE\\[NH][]J^<%F='QDLKJ"
"D\\KJH-?WO_45ELOLB<#@A[[>A;K9E</CF\\3CW`<DX0@EM^3`H=3RF=#PD<GKD\\WQ"
"B\\;GD\\[OC\\?GDLGID\\CGG]#PL^(!W`TLJ-7UWPTLQ_D7G]+PG]3SH=KWI]__F];U"
"S?87ONP+I-KWD<SIC,[HEM;QG-3TC\\3ED,7FD<;GHMGWC<CEBLKED='LB<;BD<[J"
"E]3RB\\CFD<SKG-?VF=#PDLGIE\\_OC<CGBL?EC,GGC<CGA\\+ACLOIK>L(G=CWE<WM"
"ELOLHM?XI-GZNN`0I=KYE\\KIG,KJL-OZ\\@`VV@$@S/@6S/L:P_D7H=CXF,[QD<GK"
"D,OLA\\/DC\\?GD,?GD\\CGHM7TO^T,T/X=G\\WMR?@7XP`PH=+QG=+QI^#]H]O[I-_^"
"T?D8X`\\MG,[JE<SIDL_JE-'LD<KGC\\;DC<CGD,WKF]KXD=+OC,WJF-?UE]+QDLKJ"
"FM?UC,GGC<CGEM'PF-/RD\\[MD<SKD,OJC<CGCLOIBLGGB\\KHB<3CG=7UGMGXHMKZ"
"HM?ZG-'TJ^0#GM?TI=OXKMWZT/H8\\0`WR_43P.X-N.H)G=+QI]S]J>#`D\\KJDLWN"
"DL[OC<GJC<7ED<CHE,OIH-/RNN<(U@$BG,WLG]+PX``RJ]GZH=CVF]3QI^#]MN`,"
"L=KY[0`VROH6K=_[F<SJG,_MFLWLFL_NE<WME]3RG^#]D-/OC\\[NC,CIFM#SGM/V"
"FM7TC\\KIAK[>E,SLE=+PDL_MFM+RE,SLDLKJD<SKC<SJAL7CFM+RGM7UFM'QD\\KJ"
"D<?JELSOE]+QKN@#M.L%P_,-PNP(O^D'K-KZNNX,K^<&J^0#I-O[GM7UF-#PC,?F"
"BL;GB\\?HD<CHC\\3ED,GFFL_NO>H+TOX?BL#=CL3AV@@HI-+SE,OIDLWJC<;CL>L'"
"Q?,0HL_MUP(@V/`?Y`HMWP$DJ];WGLSMI-;XJ^0#G-O[E-7TDLKNB[_ED,+HH]7["
"I-G\\E\\[NAKW=B<'ADLKJH-CXG]3WF,WPELOLE\\_OC<CGDLWLF-#PHM?XF\\[MC\\3E"
"D,CJC,?HE,_LHMOUF]'JF<OCO.P&K-[ZJ^#_H]O[C<CIBL7FA\\+CD<SKF-/RD,WI"
"BL?EB\\;EBK_@E,GJC\\CEGM7SN>8'Q?(3C\\;DD<CFPO03H]3TEL_LB,/@F]/SG-/S"
"K>#^F\\WIDK_?P^P-ZPLO]P`[V?X@O>8(N>8'J-W^G=GZI.0#D,KND,;KD,'JDL+I"
"I]G]HM?XB\\+BB<'AC,/CD\\KJELONI-G\\FL_PD<CHF-#PF]/SHMKZI=K[K-W]EL;I"
"D,CJD,OJDLGGD<GCG]7NO_8.LND#J>#]E]#MC<CGC<CIB,3GA\\/DAL+CDL_ME]+O"
"F-/RG-3TH-7VELOLD<SIF-'NO.L+ONT-D\\SIC,7BOO(0I=CWB\\3AG=CUF]/SI=S\\"
"CLGFC<;BC\\;DB[[=G<OLON<)]P`\\V?P@J];XF<SKGMGXG=[[E-?UB\\SKC,3HE<?M"
"G\\_TL^8'D\\OKA;W=@[K:A[S=CL/FH=;YGM/TE,OKGM7UI-O[J>4!K^<$O>P,K]W^"
"CL/DF,_MD<3BD\\;DE,KEO_D2O_P6E,_LD\\KJD<CHB,#BAL'BAL7EA<3DG=CWI-S\\"
"K>8%L.D(H]CWF]+PD\\SIK.8\"NNX,L.0\"FM/PF=3QL^D)F,WNE<WMB<3CE,SLE<SL"
"B,7CAL/?BL7DA\\#=AKW;EL3EO^0'\\@`XO.@)F\\SLD,CHD,_ME-?UE-?UB\\?HD,;I"
"F\\WOL.,$JN,\"BL7D@KG9A;S<@K?:I=O^H=CXH-?WG=7UH-CXG=CUJ^4!N^T,TO\\@"
"IM/SD\\'AH=+QG,_MF=#NE]+OP/P:DLKJC,'BC<+CB\\+BBL+DB<7FF=3UG]7XHMGY"
"F=#PG-/SHMGWE\\[LE]#MG-7RJM_^J-W\\F]3QEM'NH-?WFM'QF='QC<7EC\\?GE,OK"
"B\\/EB<'A@KW>A\\+ABL+BC,/CC+W=W@HKV`@IK=O\\H-/RI=SZB\\;EBL?ECLOIB\\;E"
"ELWMF]+RL>H)I^,!D]#N@[[=A;W?O_88HMGYFM7TG=SZDM'OE=+NF-'NJ=SZR?<6"
"\\@`VQ^P,K-KZLNH'I-_^EL[PU`TOBKS@G,SOB[W?BK_@B<#@D\\KJL.<)I]G_F<OO"
"F,WNFM'OH=CVH]KXGM7SHMOXGM?TIM_\\H]SYF-'NI]__F='QIM[^BL+BD<GKDLKL"
"BL#CCL;HD,OLB\\?HB<7FB,/BB\\3AC\\3CW@PNM.,%F,;GE\\CHIMOZD\\KHB,7AA\\3@"
"C,7BFL_NH];UMNH(K^4#F]+PKN0$OO85G-3TC\\SJF=CVH-_]F-'NE\\SKC+W<G,?F"
"U?D7\\@`SS/@8L.0\"J>(!J.#`W``RI]7XI-3WE,?FB\\+@A[[<B+_?F,WNL^0*IM;["
"E\\SMH]KXK^<$FM'OD,?EELWKE<WMF=3SH=S[H=GYF='QD<CFM>L)F,WLELOLD,?G"
"B\\#AB<#@B<3EB<7FC\\OLB\\CFC,?DCL;FM>L-KM\\#R?89KMK\\F,7EM>D$E-#KB<;A"
"C<;BD<3@GL?FI\\SJU?D7\\0`UY``RQ_X:L>T+JN@%F-/TF]+RR_T<H]#PDKW<G,CD"
"N.'`ILWMZP\\OX@LIJ=SXG-+MT`,?J-;VF,OIBL'?D,GFC<7EC,/CC\\3EJ=?ZJ=G\\"
"I-S\\K>D'G]CUC<3AC,'@E,OKD<GKD\\WQD<OOE,SNI=KYI]CWQO82T?\\<F,;FFLOK"
"A[[>A[[>A[_AB,/DB,3EB,3EB,7CB<3CO?06GM#THM#SK]O]Q/,2I=?SM>P&I=WW"
"L.<!OO$*WP8C\\@`QY08CT?<4H,WKD\\;DB\\3AA+_>E\\[NG<_QG\\WNQO03W`TJO>L("
"NNH&J-7SG,/CY0XJQOD2ROT4VP0@M-O[I-+RDL?FBL+BCLGHCL;FE\\[NH-'QG=#O"
"EL[NG-?VE<[KC<3BC\\;DE\\[NCLCLC<?KD<OQGM;XL.8&MN@&K=SYUP0A[0`YJ-7U"
"A[_AA[_AB+_?C,/CA;W?B</GBL3HB\\/EI]S]JM_^H=;UC\\;DA+G:S`,APNX,T/H6"
"NN@#S/P6T?\\:P.`*L^8!F<SHF<SJELOJE,OID,?EE,GHD,7DE<CGELGHH=3RQ?P8"
"R/`<GM;PELGEI=GRN>`(T/X9]@`U^``XVO\\ANN<(G];VB<3CE,_NE]+QGM?TD\\SI"
"G-7REL_LC<3BC\\;FCL;FC<CIB\\;GD<SMGM;ZHMK\\C,/C@[K8A;K9H]3SW0HI[0`V"
"B,#DA[_AB\\#AC,'BAKS?C</HDLCMC\\7HG=3TJN'_EM'NF];SA+S<E,7EW0$AZ`\\M"
"W@<AN^H\"H=7NJN#[LNP'F=+OE,OIC<3BD,7DE<KIB\\+@I][\\BL/@CL?DG];VG];V"
"F=+OE<[JB,'=CL;@N?(+O>L&X_XAJ\\/JK='UN.<'R/`?T0PJOOH8IN'`F];SI-WZ"
"E<SIDLCED,?EE<SLCL;HBL7FCLGJF=3SF]/UD<GKA<#A@+O:A[_?J=[]M^8%L=[\\"
"B;_BA[W@C,+EBL#CBK_BCL/FELOND,;IH-OZH-W[D='LBL?CA[[<CKO;SO$4\\P`U"
"X0HDKMWVIMSWH-GUE]+QCLGHCL7EG-'RE,?FE\\WJG=;REM+MBL?CD,OJGM;XG=/V"
"C\\;FE\\[LB+_=C\\;CK.3^PO40P^@*D+37CKS=@[C7?+34CL;FJ=[]N_$/M>L)L>4\""
"H]7QG,[JD<CFC<CGC,3FD\\OME,OKDLGGG=CWEM/QAL7G@+_AA+S@J-W^K=[]HM'N"
"B\\'DBL#CCL;JB\\/GD,7HD<;GF,WNDLGIG]O\\C\\[NC\\SJB\\3ABKW;I=+R[0`SKM+U"
"L-SXU`HCX``TK^<$EL[NGM;XG-'TGM#RJ=S[JN#]F]3PF]CSF]CVI-_`G]?[F<_T"
"EL[PC,3DC<+CCL/BJN#]MNH%E</CC;O;B;[=D<KGF='QE,OKG=#OI-?VMN@'O_$/"
"L^#\\J]KUGM/RFM+RDLKLE,KME<KKE\\[LD\\[KD,_M@L/D>[G>A\\'EK^8(J=S[F\\WI"
"C</FCL3IC\\;MC\\;MDLCKE,OKE,OID<KGG=GZH-S]E<SLB;S;G,GGW@LHH\\_QI-+S"
"F,KFIMCTD\\;DM>D'T0,DT0,DI-3WFLKMN>T+HM?VK^D%M?$-K^L)IM[`G]7ZF,[S"
"CLKKB<3CELCJF<KJHMCUL>@$B;_<H]KWF-'NDLOHC<3DG=+SELOJG,_NG]#OL^(!"
"U@,>R?<4L^(!I=7XF,WNELWMD\\SIE,WJE]#MC\\KI@\\+B@<#BF=CZJN8&E\\KIF\\GI"
"BL+FC<7IC,3HD,CLD,CJDLKJC<;CC,?DF=3SH-?WE\\?JH<_PV08EI=/SD,7DCL7C"
"F<_LG,_MELGHEL?GI]CXN>L*M.,%IM?WM^D(L>4#E,OKCL7EC\\?ID,CLF,[SD\\OO"
"CLKMEM'RDL?HFLSNF]#OL>D%E<[JF=7PF=+OH-?UI=S\\IMW]G];TF]#OF<?GDK_?"
"FL?EQ?,0W0@HROD8J=KZGM/RG-/QG-7RE,WJBL7D?[O<B<7FIN,%H][_F<[OC;_A"
"AL+G@[WAD,CJD,CHEM'RE]/TD<[LE=#OJM_^M^8%I=+RW`PMH='TD,?GB<3AB\\;C"
"D,7FBKS>E<CGD,/BF<SKJMW[O.X-Q?<6L.(!F,CKA[G=AKO>A[WBA;WAAK[BBL+F"
"B\\7IF]/UF]+RG]3UD\\KHKN8#IM_\\DLOHN?$0L>D(D\\OKCLGHH=SYOO83E,+CF,/D"
"E+_@H\\[MH<[LVPHFT?\\>MN4$I-?VF,_OE]3RAL7CBL;GH][]IMS_DL?JE<OPB\\/G"
"@;_DA\\/FB,#@D<KGCLGHE,_NE,_PGM7UQ?03U`4BU@$AJ-;WF,WPC,?HB\\OFA<+>"
"B+[AD<;IB<#@D,?EG=3QI]WZH=+QX``QM>0$BKK=AKK@A[WBB\\/GC<7GE,SND,OL"
"E,_PI>$!D\\[MF=+OF]#OKN0\"H=CVQ_\\<GM/TB+W>C<7EC<CGD\\[MG-7RU0@FS_X>"
"L-S^NN@'L][]N^@#RO84H<[NF,OJF=#PB\\?HAL7EF=3SH=GYGM/TC\\3GC<7ICLCN"
"AL+EA<'BC<CGAL'>C,7BB<#>C\\'CD\\'BLMW^XP`OI-+SE,GHD,OJC,GGB,?EB\\CF"
"A;_CB\\/GA+_>A+_<E,WGM.L%K-[ZX@`OM^8&B[W?B<'E@K[AAL+CDL[OD,CHF-/R"
"F-?WGM_^EM?TCLGFKM__R?<6Q_85F<KJD<'DC+[@E<KMD\\KJD\\OKF='QC\\KGJN/`"
"TP@FZ``ZZP`RX`LHQ.X*L-_\\ELOLC\\?IC<7IH]O]F]+PG-/QE<SJAK[>D,?NE\\[V"
"B<'AAL'@A+_>A\\+?CL7CDL7CE,'AJ-/TY0`PM^0%GM'PD,GFC<CEDL_MB\\KH@+_?"
"AL+E@;W@B\\?HA<+>E,WGKN3_N>D%WPTLM.,#D\\CIB,/DB,?GC<SLCLOIF]/SI=W]"
"HM_]F-?UFMKUD,GEP^\\/\\@`[F<?GF,OICL/DB\\#AB\\#CC,'DA+G:C\\;FBL?EF=;R"
"H-?WI-7UN^<'W`DGY``QK][[F,WNELSOHMC[E,OKC\\;DGM7RBL/@BL7DD,CLF=/Y"
"BKW<C,'@A[_AB<'AC+_=C[S:L-?U[@`RP>\\.I=;VG-/QD<KGG=;RG-?TC,CI@+_A"
"@+_A@+_A?[O<B\\;CB<#=C<#<H]+OZ0`TQ/,2D,/BD<KGB\\;ED\\[ME]+QG=7UG];V"
"H]O[J>(!M>\\+JMW[XPHKUO`>B<#=@\\/=A,/AD,WKAK[@BK_BD</GC\\'CDLKJEM'P"
"F]+RE<KKHM7THM/RNND(XP`PO_$0F<OMD<CHD,GFE,WJF=+OCL7ED<GIE=#QG]O^"
"D,'AD\\;EC\\3ED,+DF,;FN.3`Z@`OR/(.P.\\.O.`.GM7SD\\SIGM7RE,WJA\\+CB\\?J"
"A,#C?KK;BL+DA[[>D,7DDL7CL>,!P_,/W@TIS/P8R/<6L^4#F<[MF]+PD<GIE<WM"
"E,OKG]3SI]WZR?@7\\P`YK=;UA\\#<A<?@C,WJB,?EAK[@B\\'DB\\#CE,GLA[_?D<GI"
"H-?WI=S\\I][^H-?UF\\[MI]CXS_X>N.H(D,7DELWKE<[KD<GIEL[ND<SKHMW^G]SZ"
"M^@*N^H*Q_44R_<5V`<E[``YS/H7R/@4OO$*N.L&Q?D6V0XLJ>#`C\\7HD\\OMA[_A"
"B\\/ED,CJE<KMI]G]HM3VP/,4N>\\-G,WLF\\CDT/@5V_T<\\P`VSO<8FLOKC\\SJB<CF"
"E<WMG=+QH-/QU00@XPHIDKO:B,'=B<;AD,CHC<3DBL+DBL+DAL'B@[[?BL+DD<GI"
"FM+RJN,\"GM;XC\\?IE<OND,+DL^4#SP$<O_$/BL#=B\\/CC\\OLE-#QFM;WHMW\\GMGV"
"O^`2R/H9P>\\.R/02Q?,2H]3SH-/RF]'NH];RG,_KJ-GXH=3SO_47H=?ZC<7GD<GI"
"D<CHE<KKF\\WQGM#TMNP.I][^G];VH]CWF\\KGFL/@P>4#YPHJY@TNH<_PC<CGB\\KH"
"C\\CEF,_LI=CVZ@`UZP`RF\\;GC,/@@[[;C<3DCL7EAK[@AL'B@[_@A,#AB\\/EC<7G"
"D<GIDLWLD<SMCL;JD,CJD\\KJD\\;EPO02QO@6L>@$DLKJD,SMFM;WG]O\\H-OZKN@$"
"I]KYGM/RC,'@E<SIC\\CED\\SIE=#OD<GID\\KJH-7VJ-K\\FLSNELONHMC[E<WOC,?F"
"C\\?GELWKL>0%M.<(F='QC<CGE]+QI-S\\GM/TELGHCL'@E,7ET/T>NND)H=;UFM/P"
"EM'NFM/OF<SJXP`PV`<GF\\?ICL/BB,#@BL+BBL7FA\\+CB<'CA[_AD,CJB,#BC,3F"
"DLKJD,CHC\\?IDLKLC<7ID,CJD<CHELOLP?<5P_D7J>#^EL[NF]/SC<CGDLWNF];U"
"G-/QC<;CD,GFC<CEC,?FF]CVD,OLC,?HE,SNG=7WE\\WPELSOE,KMD\\GLD<GID,CH"
"BL/@C\\CEI-GZKN8%FM7TD<SKH]O]J.$\"F<_RELSOB,#@C,3DF<SKO?$/HM?VF]+P"
"E-#KEL_KE,7DR_D8U0$BF<?HDL3FB+_?B\\;GC,CIB\\;GC\\?IAKS?DLCKCL;HD\\OM"
"D,CHD,CHD,CJD<GKC\\?KCL;JC<7GC<7GJ-W^O?,3OO02D<;EC\\;FD\\OKDLWNF-/T"
"F=;TD,OJD\\[MEL[ND\\GLF]'TE\\_QCL;HD<SMG-CYDL[OD,SMD,OJCLGHD<GID<GI"
"C\\CEB,/@C<CGH-W[H=S[HMKZK^4%I=?YC\\'ED,7HB<#@D,CHD\\[MEL[NFM/PG=3R"
"CL7BG=/NH]'QN><&W04GE<'CG<_QC<7EB<7FC<GJEM'RE,SNFM+TDLKLC,3FBL+D"
"D,CHDLKJD<GKDLKLC\\GMB\\7IB\\/EBL+DC\\3EI]S]R?\\=L^D'DLGICL;FCLGJE,_P"
"G]SZE=+PDLWLF='QF<_RF<_RG=7WC\\?IB<7FE='RE-/SC\\[ND,WKDL_MB\\;ED<GI"
"E<[KD,GFD,WKG-GWG=7UG];VI=K[E<KKC<+ECL3GE,OKC\\?GC\\SJDL_ME,_LI=SZ"
"K-_]ELCDN>@'U/`@S_89J=3VH-+TG-3TC\\OLE='RCLGJE\\_QH][_HMW^E,SNFM+T"
"E\\_OF='QEL[PE<WOD,KNC<?KB\\/EC,3FAKO<D\\CIHM?VSP0BIMW]HMGYE\\_QF=3U"
"H]O[E,SLB\\;GC,?HE,_PG=GZG-?XC,?HA<#ACLGJEM'PFM7TF-7SEM/QB<CFDL_M"
"DLOHDLGGFM/PG=3RE\\SKF<[ME,_NA\\;DBLGGB\\CFD,?GC\\;FBL7DG=CWG-3TF]#O"
"N^H)O.H'T?\\<W`LIUOX?J]/UJMW\\K.8\"G][\\EM7SEL[PD<GKI>(\"E]/TE,_PE<WO"
"F-#PF]/SFM+TFM+TD,OLD,OLB\\/EC<7GDL?HE\\SME\\SKK>,!N_,2G=3TG=7WE,SN"
"HMKZH-CXB\\;GB,/DBL;GEM+SH=W^E,_PA+S>C<7GD,CHF-#PF=3SFM?UDM'OC\\SJ"
"E<SJF<[MHMGWH=CVGM'PFL_NC\\KIA\\;DAL?DB\\CFC,/CD,?GF=3SEM'PE]+QD\\CG"
"IM/SZ``SK]SZT?T;X@8HI,SNG=#OI^#]F=CVD]+PE,SNG-3VE=3TD,_ODLKLC\\7H"
"C,3DC\\?GG-3VG-3VD<SMD\\[OB\\/ECL;HC<+CE<KKDL?FHM?VM>T,G];VFM#SE,SN"
"G=CYJ.0$DLWND,OLB,/DC,?HH][]I]__BL+BA[[>AKW=B,#@C\\?GG=CWF-/RCLGH"
"GM7SIMW[JN/`KN@$JM_^GM'PD\\CGD<CFD<CFD\\KHE\\[NCL;FDL_MFMGWD,SMF]+R"
"M^,#W08DG\\SJN^D&\\0`YL-CZH-/RGMGVH^+`I>4\"I-S\\EL[NC\\[N@L'AC,+EC,'D"
"A[[>AKW;I-S\\G]?WE]+SEM'RC\\?IC\\?ID\\CICL/DE,GHJ-W\\O?,1F,_MDLGID\\KJ"
"E=#QFM7VE]+SEM'RB\\;GDLKJFM+RLNH)GM/T@K?8@[C9A;K;CL7EI=S\\F='QFM+R"
"I-OYI-OYGM?TL>D&OO02L>4#I=;VF\\SLF\\SLD,/BD\\CIE<WMDL_MD,_MF-3UE<SL"
"L-O\\WP@FH=#MHM'NX0DJM-_`I=KYFM7RH-_]MO01IMW]D<CHB\\KJA<'BA;K=DL3F"
"C<+AB+_=LNH)I=W]H-CZF-/TD\\OMD,CJC,'BD<;GDL?FKN0\"P_D7D,7DD<CHKN8%"
"F='SE]+SE,_PCLGJC<CGD,OJE\\[NI=KYL^4$J-;WC;O<C;O<C\\#@N.X,I][^H-CX"
"G-/QE\\[LFL_NHM?VL^H&R_\\<ONT,KMO[J-;WEL?GFL_ND,GFE=#ODLWLDL[OD\\KJ"
"KMO[XPXLC+_=C\\+@RO@7M.(!D<CFCLGFB\\CFH=S[OO04D<;GAL/A@[[=C\\'CC[_B"
"G]+QE\\[KL>D&F='QE,SNE<WOD,CJC\\?IC,'BD<;GF<[MH]CWQ_T;D<;EE<SLJ^,\""
"F,[QGM;XD\\OMB<3CC<KHA+_<DLGGHM7TK]O]N>4&LMK]D;S>KMS]O?$/E<SLDLKJ"
"I-OYH=;UG<[NF\\SLD\\;DH=3RT`(AW0HIN^H*IM?WE,GHF=#NG=3TE\\[NB\\;GF-#P"
"L-[^R/@4F,_LC,/@K-W\\O.X,E<SJD,OHD\\[MH=GYJ-W^N.X.FM7TDLKJE\\?JM^8&"
"NNX+N>`,HMGWH]KXCL;HD,CJD,CJB\\/EDL?HC<+CE\\SKG-'PPO@6IMOZD,?GG=3T"
"K=`!E,GJI=W]I-_^A\\3BA<#=D\\KHD\\3CIM'RKM7WS/$3X0@IV04FQ/@6K.4$HMW\\"
"E<[KK>#_O>H+DK[@C;[=ELGGF\\WIJ=OWV0PIS`,@N.X,HM?VH-#SG,SOCL;HE<WM"
"M><&N^T+G]CUC\\KGJ=[]L.8$B<#@F]/SF]/SF]/SJ^$!H-7VG]?WJ.#`ROP;PO$1"
"L^4#L.<#GM7SH-?UF='SC,3FC\\?IB\\/EC<+CD<;GELOJJ=[]MNP*LN@&C,/CG-/S"
"J=[_P?<7F]+RJ^4!HMGWFM#MF<SJKMS\\O.@(V`0EZ``UY0`PK]W^G,_NF=#PF-#P"
"FM/PF,WLM.,#S_P=O.X,FLWKL-_\\J]KWM^D'Q?<5E,?FF<SKG,SOF\\WOFM#SGM;V"
"N^\\-L^D'F];SD]#LJ>+_I=SZE<WMC,3DC\\?GELWMH-7VLN@(M.T,OO85LN0#L-[^"
"H]3SG]+NF,[KFM'OG];VG]?YG=7WI]W`G-/SELOLD<;EI=KYN_$/JN'_F,_OFM'Q"
"F=3Q?K?4M>L)O>\\.S/H9R/02PNP*UP(@ZP`UR_L7KN'_DL7CE\\SKF]#OC\\3EC\\3E"
"C,3DDLKJD\\KHG-'PPO83XP`RZP`RX0@GVP(AP>T+LN0\"G=/PCL7CBL/@F-#PFM+R"
"HM?XK.0#G-GWD,_MG=CWC\\KIC,3DC,/CD\\KJC\\;FE\\SMG];VE<[KF-'NH-/RH=+Q"
"H]3SHM7QELSIF,_MIMW]H]KZJ^($K^8(F=#PG-/SFL_NMNP*NO(/IMW[ELWMD\\OK"
"F]OVD\\[K?[+1Q?,2Q>T,W@$AW`4BQ.P)I];SK>#\\M>\\*F-3OC\\CEC\\;DE,GJE\\SM"
"B\\/CG-?VCLGHCL?DGM'OJ=?WMM_^U/P;\\@`XV`4CKMS\\E,?EE<[KF];SE=#OH]O["
"H-?WJ>#`G=KXDM'OI.0!I>+`A+S<B<#@A;K;E,GJD,?GH-CXF=3QCL?DFL_NJ-OY"
"K-[ZI]GUL>4\"GM3QH-?WF]+RE,KMHMC[H]KZE<SLG=+QG-'PK>4\"G-/QF]/SD<GI"
"D,WIBL/@CK_>D+O<ZPXN]@`VQ^L)JM'OKMWXJ-[WF]?PDL_JC<KFE,_LD\\KJDLGI"
"E\\_QG-?VA\\+AE<SJH]3SHM#PG,OHF\\WII];SROH6Y0`QLN0\"E<SJE<[KDLKJE,OK"
"E\\SOFM#SH-O\\E-/SD,_OJN<'FM+T@K?:A;?9B;[?A[_?GMGXE=+NE,_LJ>#]I-KW"
"I]GUO.\\*L>4\"M^X*GM7UC\\;FELSOH-;YI=S\\G=3TIMOZI-GXHMGWHMGWH]SYH=KW"
"E,_LD,?EB;K:G\\SLZ``RP.@'V@,AW@DGR?D5F,OGE<[HE-#KB<;BFM7RF]/SIMW]"
"F]/UFM+TDLKJH-?WIMGXI]CXI]KXJ=_\\H]GVG,_ML>,!S`$@L.8$C<3BDLGIE<SL"
"E<KME<ONC\\KKD<[LEM/QI-_^L.@'F,WNA[S;@[C7AKW;LNP(HMWZH]SYF]+OFM#M"
"H-/OJ-OWP?,1O.X,L.(!H=+RF<[OJ^$!I]S]E,OKDLGIE,OIFM'OG-/QG]CUEL_L"
"ELOL@[C9F<GLXP`SJ=S[F<_LE\\WJJ][\\N^T+T?`?P?,1P_<5F]3QFM+RLNH)G];V"
"J-__L^L*M>T,K>4$I=S\\I-O[KN0$G]3UD\\KJD<CHE,OKN.X.R/D;MND*C,/AC<;C"
"DLGICL7EC\\KIB,/@D<CFH-?UJ^+_L>D%H-;SD,/AF\\GIP/(0F,[KE,OHI=OXE\\[K"
"F,_LE,OHG]#OQ/(/WP8EQ.P+L-[^M.@%ELCJCL/DD\\OKCLGHE,OKF,_OF-'NCLGF"
"F,KLDL3FTP4FGM/TD\\SICLGFFM7RF,_MEL?GF,;GP_(2O>\\.HM?XIMW]KN8%KN0$"
"H-7VH-7VH-?WG]?WFM+TDLKLF<[QE,GLE,KMB<3ED,OJC<7ER?H<QO<9I-OYE,WJ"
"I-OYF-'NC<;CC,7AD<3BFLWKG]+NL./_O.\\*N.@$TO`=QO44I=OXHMGVELWKJ^+`"
"HMOXC<3BD,'@C[K9K=+PT_D6U@$?O>P+M>D'F]+RF];WE,_PC\\?GE\\_OG=;SCL?D"
"F,OJV@TKI=K[E,OKB<'AC,3DE<WMC\\;FDL?HD,+DIM?WL^4$K^,!Q_L9H];UE,GH"
"BK_@B<#@C,+EBL+DC<7GC,3FD<;IC\\3GDLKLC<CIBL7DD,CHO^`2H-#SM^\\,LNP("
"D\\SIL>L&EL_KDLKDE<?CFL?EJ-'PO^<&[``WZP`TP?,1M.H(G]CUF];SGM;VE\\_O"
"OOH8K^<$F<KIEL/AK-GUG<GES_<6R?45F,WLG-3TDLKLD<GKEM+SD]#NFM/PE,OI"
"V0PJJ^#_D,?GC<7EC\\?GB+_?B+_?B<#@C,/CD\\KJF<[OJ-OZM><&V`TLJ=KZB[[="
"AKO<AKW=B+[A@[O=AL'BB<'CB\\#CB\\#CA[_AC<CIAL/AC\\?GO_(3GL[QE<SJD<KG"
"QOX:L.<!QOD4ONX*P_,/U`4B[``VWP8ENN8$H,[NFL_NBL/@CL;FCL;FCL;FE<WM"
"E<WML>D(U`LIW0XLS`(=R/@4Q^\\.S_@9I]S[E,SLELSOCL;HC<GJEM+SD,?GB+W<"
"I]KYDL?FE\\_OC,?FD\\GLC<+EBK_BC</FBL7FC<CGH-CXL><%K-K[YP`SHM#PB;K9"
"BK_@A[W@B,#BAK[@A\\+CA+_@A[W@B\\'DB<'CA\\+CCLOIB\\/CMND*HM+UI=SZE<SI"
"?*[*S_P7O^4\"U?L8R/(.RO@3M./`L^<\"NO(.FM/OC<CEEL[NFLSPF<GNB;O?F<[Q"
"E\\[NC\\;FG=3RN.\\+P_<4Z@`WZ@`UPN`/K-W]E\\SMC\\?IE='RF-3UF-/TE,OKCL#B"
"G=+QD\\KHC\\KID,OJC</FB+W@A[S?A[_AAL+CAL7EDLWLMNX+M^8&[0`XH<[LCKS<"
"AKO<B+[ABL+DA<#AAL'BA<#AC,+EB\\'DBL+DC<CIC,GGD,CHK=`!I]G[C\\CEE,OH"
"C\\'=FL/@[@`O]@`WQ>\\+J-7QJMGVJ][ZI=SYCLKEC\\SHCL;FE<?KE\\3JG<_UH=;Y"
"B;_BD<GIH-GVF=#NHM7TG]#ON.<&Y@`TR?@:H=/UE]+SE]/TD,SME=#QC\\3ECL/D"
"E\\[LC\\CEGMGXEL[NBL#CC<'GAKSAA;WAA<'DB<CHD<SKH]KXR?@8U`,CM=[]C[S:"
"B+W>C<7GC\\?IB,/DA<#AA<#AD,;IA[W@A[_AC\\KKE-'OCL;FL.,$D</EHMOXA\\#="
"B[[<DL'>ZP`TT/H8X`\\MS?D7M^#_H\\[MF\\CHF<KID<CFF-#PE\\WPGM3WG=7YE,SN"
"DLKLEL[NG]?WG]CUG=+QI]KYH-/RJMO[T/\\AROT>G-3TB<3CC<CGCL;FD<CHFM'Q"
"F-'NDLOHGM;VEL[NC,+GB;_DB+[E@[WC@L'CA,/CC<GJDLOHN^T,S_T:R/(0D;S;"
"C<+CB\\/EC<7GD,OLC,CIAL'BE,KMC\\7HEL[PD\\[OD,WKDLKJJMS^G<_QD\\KHA+O9"
"B[[=X``QNND(H<_OF\\GINNH&U`,AS?D7N^<'M>0#G]+QG=3RKN8%I]`!F=3UJ>4%"
"H][_K.@&H][]H-CXK>4$I]S[FL_NELGHF\\ONQOD:O/03IM[^DLGICL7EF]+RG-3T"
"O?D5M?$-H-?WE,OKB;_DCL3I@[WCA<'FA\\/FB<7FB<3EA\\#=D\\CGO.P(V0(@KMGX"
"C<+CD,CJC\\KKD,OLDL[OF];WE<OND,;IC,3FC\\KKC<KHC\\?GJ][]G,_NE,OIAKO:"
"T_`CM.$$F,CKE<KIH-7TFM#MC\\7BF,OIR/H8Q/84H]3SHM7TMNP*H]KZL.@'J>(!"
"J^<'I-_`G]K[F]/SG];VELWMD,?GD,?GE<CGG,_NS@,ANN`.F]#QHM?XEL[PC<CI"
"H^#\\O/@4L^L*CL/FD\\GNC,3HBL;KA<'FAL+E@KS@AL'BC\\?GC,/AELGGQO(0T_\\?"
"K.(\"E<WOB\\;GC<CIA,#AB\\;GD,;ID<?JD\\OMD<SKD<SKE,WJJMO[H]3TB\\#?T08D"
"M>('F<;LBK_BC<7ED,CHD<KGD,OHEL_KG=/PL>,!L=[^NN@'JMW[IMW[CL7CBL/@"
"C\\KIE]/TF-#RE<WOE\\[NE<SLB\\/CC<7ED<;EE\\KIK=[^R_T<L>($C<+CCL;HB\\?H"
"H=[\\E=#OJ-W`I]S_E,SPBL3HB,?IA\\;HA\\'EC,3HC<7GD,CHC<;CF<_LK=KZX0XM"
"F]#QB\\/E@+O<@KW>@K[?AL'BB+W@C<+EDLGIE]+QE=#OF]+PS?P<J-;VTPDEMNX+"
"D\\7IC\\/IBL3HB,3EBL+BA\\+?C,GEGMOWGM3QK-KZON8%X`LJK=SYH-;SG-7QEM'N"
"E-'OD<[LF='SE<WOJN$#E\\_QE,_NE=#OD<CFDL7DFLCHONT,O>P,M^L)E=#OD,_M"
"AL'@G-3TD\\GLF]'TF='UF-3WB\\KLA<'DCL;JA+K?D<?JB\\/EE<WMAKO:P_44SO`?"
"B+_?@KK<?KG:@KW>A\\+CA\\+CBK_BELOLC\\;FD,GFD<KGELWKT/X=Y0`QK-_]E<SJ"
"DL;LDLKNB\\?JA<3DC<CGCLGHCLGFG=;SP/01Q?,2SO,3\\@`XJ]KWH=?RJ^3`J.3_"
"FM7TD<SKC\\?IG-3VI-S^E]+SD,OJEL[NG-/QG=#NK=KZQ?,2SOT=O_43E,_NAL/A"
"B,#BA+S>ELSOE\\_QE,_PF-3UDL[OBL;GC,3FC\\7HB\\'DE,KMB<#@F]+RMNP,KN0$"
"M>T,C,3D@+C8BL7DE=#OF='QG,[PFLSNE\\SMCL7CE\\[LG-+OR_<7R?45F\\WIE<OH"
"E<ONC,;JBLOLA\\KHAL7CCLGHE\\SKG<[NQ_,3VP,DL]O]Z``WM^L(E\\[KH-?TI-WY"
"GM;VF]/SELSOG]?YD]+TD,_OD\\KJDL?HB\\3AE\\WJIM'RZ@`SQ_L8F]+PE<WMD,OJ"
"B+W@B+[AC<7GC\\KKD\\_PDL[OFM7VBL7FB\\/EELSOF,WPB\\#CD<;GJN(!EL[NEL[N"
"L>H)G]?WF]/SF]/SF-/RE,SLF,KLG,[PK>,!E<SJH-?TMNH'UP(BP>H+J]KWF<_L"
"D<GKCLKKCLWMBLOHA\\;DCL;FF,OJI]/URO,4Q.T.IM'RT`(AS`,@H=?TF,_LI][\\"
"K.0#IMW]G-+UE,_PA\\CI@L'AC</FBK_>C</@E,?CKMOYW0HGJMW[E<SID<GIB,#@"
"B[WAB;[AA\\+CBL;GDL_MG-?VE<WME,SLC\\?GC<3DE<KKELOLG=+UE,KMFM7VF=7V"
"F-#PG-3TH-CXF-#PF-/RE,SLDL3FE,;HELOJP/84L.<#Q?<5\\0`XNN0\"H,WKJ-[["
"DL_MD,_OC<CIC<7ED<SKE,OIEL?GI]/URO86Q.`0HL[PL=_`W0LKHM/SIMOZF]+R"
"L>0%M^T-F];WD=#P@+_AA<'BAL'BC<3BD;_?L-W[X0`LN>P'F<SJD<?DD<CHE,SL"
"BKK?C<+EC,?HB\\KJD]#NGMGXHMKZFM'QEL[NE<WMFL_PH=;WL^8)I=O^G-?XG-CY"
"FM7TD,CHE,SLDLKJC\\KID,CHF,KLELCJGM'PM.H(VP`MK^#_Y@\\MS_<6HL_MF]'N"
"DL_MB<CHD,CHDLGICL7CCL7CE\\CHI-+SP^\\/P.X-D+[?F<?HW@HKG<OLE<KKDLGI"
"F,CKM>L+IN,#A\\CG@L'CA,#AC,CIC\\;DONH*Y0XLQ_H3IMSUH=3RF]'ND\\KJGM;V"
"";